//
//  CEThread.c
//  CoreEvent
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//


#include "CEThread.h"

#include "CEMemory.h"
#include "CEInternal.h"
#include "CEMainQueue.h"

//custom
#include "CEThreadBaseInternal.h"
#include "CELog.h"
#include "CETaskScheduler.h"


void _CEThreadDescript(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript);


const CETypeSpecific_s __CETypeSpecificThread = {
    .name = "CEThread",
    .descript = _CEThreadDescript,
    .deinit = CERuntimeDefaultDeinit,
};

size_t _CEThreadGetSize(CERef _Nonnull p) {
    return sizeof(CEThread_s);
}

const CEType_s __CETypeThread = CEType(CETypeBitHasRc | CETypeBitStatic, sizeof(CEThread_s), (uintptr_t)(&__CETypeThread), &__CETypeSpecificThread);

CETypeRef _Nonnull CETypeThread = &__CETypeThread;

void _CEThreadDescript(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript) {
    assert(object);
    CETypeRef type = CERefGetType(object);
    assert(CETypeIsEqual(type, CETypeThread));

    CEThread_s * thread = object;

    char buffer[32] = {};
    descript(handler, "<");
    descript(handler, type->specific->name);
    snprintf(buffer, 31, ":%p,", object);
    descript(handler, buffer);
    descript(handler, " threadName: ");
    descript(handler, thread->name);
    descript(handler, ">");
}



#if __APPLE__
_Bool CEIsMainThread(void) {
    return pthread_main_np() == 1;
}
#else
#if __has_include(<syscall.h>)
#include <syscall.h>
#else
#include <sys/syscall.h>
#endif
_Bool CEIsMainThread(void) {
    return syscall(SYS_gettid) == getpid();
}
#endif

CEThreadSpecificPtr _Nonnull CEThreadSpecificInit(void) {
    CEThreadSpecificPtr specific = CEAllocateClear(sizeof(CEThreadSpecific_s));
    return specific;
}

void CEThreadSpecificDeinit(CEThreadSpecificPtr _Nonnull specific) {
    specific->thread->status = CEThreadStatusFinished;
    
    if (NULL != specific->syncWaiter) {
        CEThreadSyncWaiterDestroy(specific->syncWaiter);
        specific->syncWaiter = NULL;
    }
    
}


void __CEThreadKeySharedValueDealloc(void * _Nullable value) {
}
static pthread_key_t __CEThreadKeyShared = 0;
void __CEThreadKeySharedOnceBlockFunc(void) {
    int result = pthread_key_create(&__CEThreadKeyShared, __CEThreadKeySharedValueDealloc);
    assert(result == 0);
}
pthread_key_t CEThreadSpecificKeyShared(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CEThreadKeySharedOnceBlockFunc);
    return __CEThreadKeyShared;
}

static char * __CEMainThreadDefaultName = "main";
CEThread_s * _Nonnull _CEThreadAllocInit(char * _Nullable threadName, CEThreadStatus_t status) {
    CEThread_s * thread = CETypeThread->alloctor->allocate(CETypeThread, sizeof(CEThread_s));
    thread->pthread = pthread_self();
    thread->status = status;
    if (threadName) {
        size_t len = strlen(threadName);
        assert(len <= 63);
        memcpy(thread->name, threadName, len + 1);
    } else {
        if (CEIsMainThread()) {
            size_t len = strlen(__CEMainThreadDefaultName);
            memcpy(thread->name, __CEMainThreadDefaultName, len + 1);
        } else {
            memset(thread->name, 0, 64);
        }
    }
    return thread;
}

CEThreadSpecificPtr _Nonnull _CEThreadSpecificLoad(char * _Nullable threadName, CEThreadStatus_t status, CETaskSchedulerPtr _Nullable scheduler) {
    CEThread_s * thread = _CEThreadAllocInit(threadName, CEThreadStatusExecuting);
    CEThreadSpecificPtr specific = CEThreadSpecificInit();
    specific = CEThreadSpecificInit();
    specific->thread = thread;
    specific->syncWaiter = CEThreadSyncWaiterCreate(thread);
    specific->scheduler = scheduler;
    return specific;
}

CEThreadSpecificPtr _Nonnull _CEThreadSpecificGetCurrent(char * _Nullable threadName) {
    CEThreadSpecificPtr specific = (CEThreadSpecificPtr)pthread_getspecific(CEThreadSpecificKeyShared());
    if (NULL == specific) {
        specific = _CEThreadSpecificLoad(threadName, CEThreadStatusExecuting, NULL);
        pthread_setspecific(CEThreadSpecificKeyShared(), specific);
    }
    return specific;
}

CEThreadSpecificPtr _Nonnull CEThreadSpecificGetCurrent(void) {
    return _CEThreadSpecificGetCurrent(NULL);
}

CEThreadRef _Nonnull CEThreadGetCurrent(void) {
    return CEThreadSpecificGetCurrent()->thread;
}


struct __CEThreadContext {
    void (* _Nullable beforeMain)(CEThreadSpecificPtr _Nonnull specific);
    void (* _Nonnull main)(void * _Nullable);
    void * _Nullable params;
    void (* _Nullable paramsDealloc)(void * _Nonnull);
    CESemPtr _Nonnull sem;
    CETaskSchedulerPtr _Nullable scheduler;
    CEThreadRef _Nullable thread;
};


CEThreadSpecificPtr _Nonnull __CEThreadCreateSpecific(struct __CEThreadContext * _Nonnull context) {
    CEThreadSpecificPtr specific = _CEThreadSpecificLoad(NULL, CEThreadStatusStarting, context->scheduler);
    pthread_setspecific(CEThreadSpecificKeyShared(), specific);
    return specific;
}

CEThreadSpecificPtr _Nonnull __CEThreadBeforeMain(struct __CEThreadContext * _Nonnull context) {
    CEThreadSpecificPtr specific = __CEThreadCreateSpecific(context);
    if (context->beforeMain) {
        context->beforeMain(specific);
    }
    return specific;
}


void * __CEThreadMain(void * args) {
    struct __CEThreadContext * context = (struct __CEThreadContext *)args;
    CEThreadSpecificPtr specific = __CEThreadCreateSpecific(context);
    context->thread = specific->thread;
    void (* _Nullable beforeMain)(CEThreadSpecificPtr _Nonnull specific) = context->beforeMain;
    void (* _Nonnull main)(void * _Nullable) = context->main;

    
    CESemSignal(context->sem);
    
    if (beforeMain) {
        beforeMain(specific);
    }
    void * _Nullable params = context->params;
    specific->thread->status = CEThreadStatusExecuting;
    main(params);
    specific->thread->status = CEThreadStatusFinished;
    
    void (* _Nullable paramsDealloc)(void * _Nonnull) = context->paramsDealloc;
    if (params && paramsDealloc) {
        paramsDealloc(params);
    };
    return NULL;
}


#define CEThreadCreateDeallocParmas if (params && paramsDealloc) { paramsDealloc(params); }

CEThread_s * _Nullable _CEThreadCreate(CEThreadConfig_s config,
                                       CETaskSchedulerPtr _Nullable scheduler,
                                       void (* _Nullable beforeMain)(CEThreadSpecificPtr _Nonnull specific),
                                       void (* _Nonnull main)(void * _Nullable),
                                       void * _Nullable params,
                                       void (* _Nullable paramsDealloc)(void * _Nonnull)) {
    
    //check scheduler
    if (NULL != scheduler) {
        
        
        
        
    }
    
    pthread_t tid;
    pthread_attr_t attr;
    struct sched_param param;
    //初始化线程属性
    pthread_attr_init(&attr);
    int policy;
    int result = 0;
    
    result = pthread_getschedparam(pthread_self(), &policy, &param);
    if (0 != result) {
        CELogError("pthread_getschedparam %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    result = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    if (0 != result) {
        CELogError("pthread_attr_setscope PTHREAD_SCOPE_SYSTEM %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (0 != result) {
        CELogError("pthread_attr_setdetachstate PTHREAD_CREATE_DETACHED %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    
    size_t stacksize = 0;
    result = pthread_attr_getstacksize(&attr, &stacksize);
    
    if (0 != result) {
        CELogError("pthread_attr_getstacksize error %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    
//    size_t rStacksize = 1024 * 1024 * 2;
//    if (stacksize < rStacksize) {
//        result = pthread_attr_setstacksize(&attr, rStacksize);//设置线程堆栈大小
//        if (0 != result) {
//            CELogError("pthread_attr_setstacksize error %s\n", strerror(result));
//            return NULL;
//        }
//    }
    size_t rStacksize = config.stacksize;
    if (rStacksize > 0 && stacksize != rStacksize) {
        result = pthread_attr_setstacksize(&attr, rStacksize);//设置线程堆栈大小
        if (0 != result) {
            CELogError("pthread_attr_setstacksize error %s\n", strerror(result));
            CEThreadCreateDeallocParmas;
            return NULL;
        }
    }
    
    int maxPriority = sched_get_priority_max(policy);
    if (-1 == maxPriority) {
        CELogError("sched_get_priority_max error %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    int minPriority = sched_get_priority_max(policy);
    if (-1 == minPriority) {
        CELogError("sched_get_priority_max error %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    int priority = 0;
    if (config.schedPriority <= CEThreadConfigSchedPriorityMin) {
        priority = minPriority;
    } else if (config.schedPriority >= CEThreadConfigSchedPriorityMax) {
        priority = maxPriority;
    } else {
        double offset = (double)(config.schedPriority);
        priority = (int)(((double)maxPriority + (double)minPriority)/2.0 + ((double)maxPriority - (double)minPriority)*offset);
    }
    param.sched_priority = priority;
    
    result = pthread_attr_setschedparam(&attr, &param);
    if (0 != result) {
        CELogError("pthread_attr_setschedparam %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    struct __CEThreadContext context = {};
    context.beforeMain = beforeMain;
    context.main = main;
    context.sem = CESemCreate(0);
    context.scheduler = scheduler;
    context.params = params;
    context.paramsDealloc = paramsDealloc;

    result = pthread_create(&tid, &attr, __CEThreadMain, &context);
    if (0 != result) {
        CELogError("pthread_create %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    CESemWait(context.sem);

    CESemDestroy(context.sem);
    
    return context.thread;
}

CEThreadRef _Nullable CEThreadCreate(CEThreadConfig_s config,
                                      void (* _Nonnull main)(void * _Nullable),
                                      void * _Nullable params,
                                      void (* _Nullable paramsDealloc)(void * _Nonnull)) {
    return _CEThreadCreate(config, NULL, NULL, main, params, paramsDealloc);
}


static CEThread_s * __CEThreadMainShared = NULL;
static CEQueue_s * __CEQueueSharedMainQueue = NULL;

static void __CEInitialize(void) __attribute__ ((constructor));
static _Bool __CEInitializing = 0;
_Bool __CEInitialized = 0;
void __CEInitialize(void) {
    
    if (!__CEInitializing && !__CEInitialized) {
        assert(CEIsMainThread());

        CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
        __CEThreadMainShared = specific->thread;
        
        CETaskSchedulerPtr taskScheduler = CETaskSchedulerCreate();
        taskScheduler->thread = __CEThreadMainShared;
        specific->scheduler = taskScheduler;
        
        CEQueue_s * queue = CEQueueCreate("main", 1, UINT16_MAX, CEQueueTypeSerial);
        taskScheduler->ownerQueue = queue;
        
        CESourceSerialContext_s * context = CEAllocateClear(sizeof(CESourceSerialContext_s));
        context->scheduler = taskScheduler;
        CESource_s * source = _CESourceMainCreate(queue, context);
        queue->source = source;
        taskScheduler->source = source;
        
        __CEQueueSharedMainQueue = queue;
        
        __CEInitializing = 0;
        __CEInitialized = 1;
    }
}

CEQueueRef _Nonnull CEQueueSharedMainQueue(void) {
    return __CEQueueSharedMainQueue;
}
