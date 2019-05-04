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


//custom
#include "CETime.h"
#include "CELog.h"

#include "CESem.h"



//
//typedef struct _CEThreadWaiter {
//    CESemRef _Nonnull sem;
//
//} CEThreadWaiter_s;


void __CERunLoopThreadKeySharedValueDealloc(void * _Nullable value) {
}
static pthread_key_t __CERunLoopThreadKeyShared = 0;
void __CERunLoopThreadKeySharedOnceBlockFunc(void) {
    int result = pthread_key_create(&__CERunLoopThreadKeyShared, __CERunLoopThreadKeySharedValueDealloc);
    assert(result == 0);
}
pthread_key_t CERunLoopThreadKeyShared(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CERunLoopThreadKeySharedOnceBlockFunc);
    return __CERunLoopThreadKeyShared;
}

CEThreadRef _Nullable CEThreadGetCurrent(void) {
    CEThreadRef thread = (CEThreadRef)pthread_getspecific(CERunLoopThreadKeyShared());
    return thread;
}


CEThreadRef _Nonnull __CEThreadCreate(void) {
    CEThreadRef result = CEAllocateClear(sizeof(CEThread_s));
    return result;
}


struct __CEThreadContext {
    void * _Nullable (* _Nonnull main)(void * _Nullable);
    void * _Nullable params;
    void (* _Nullable paramsDealloc)(void * _Nonnull);
    CESemRef _Nonnull sem;
    CERunLoopRef _Nonnull (* _Nonnull runLoopLoader)(CEThreadRef _Nonnull);
    CEThreadRef _Nullable thread;
    
};

void * __CEThreadMain(void * args) {
    struct __CEThreadContext * context = (struct __CEThreadContext *)args;
    
    CEThreadRef thread = __CEThreadCreate();
    thread->pthread = pthread_self();
    thread->runLoopLoader = context->runLoopLoader;
    thread->status = CEThreadStatusStarting;
    
    context->thread = thread;
    void * _Nullable params = context->params;
    void (* _Nullable paramsDealloc)(void * _Nonnull) = context->paramsDealloc;
    CESemWakeUp(context->sem);
    context = NULL;
    pthread_setspecific(CERunLoopThreadKeyShared(), thread);
    thread->status = CEThreadStatusExecuting;
    void * result = context->main(params);
    thread->status = CEThreadStatusFinished;
    if (params && paramsDealloc) {
        paramsDealloc(params);
    };
    return result;
}

#define CEThreadCreateDeallocParmas if (params && paramsDealloc) { paramsDealloc(params); }

CEThreadRef _Nullable CEThreadCreate(CEThreadConfig_s config,
                                     CERunLoopRef _Nonnull (* _Nonnull runLoopLoader)(CEThreadRef _Nonnull),
                                     void * _Nullable (* _Nonnull main)(void * _Nullable),
                                     void * _Nullable params,
                                     void (* _Nullable paramsDealloc)(void * _Nonnull)) {
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
    
#warning 待完善 优先级设置
    int priority = sched_get_priority_max(policy);
    if (-1 == priority) {
        CELogError("sched_get_priority_max error %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    
    param.sched_priority = priority;
    
    result = pthread_attr_setschedparam(&attr, &param);
    if (0 != result) {
        CELogError("pthread_attr_setschedparam %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    struct __CEThreadContext context = {};
    context.main = main;
    context.sem = CESemInit(0);
    context.runLoopLoader = runLoopLoader;
    context.params = params;
    context.paramsDealloc = paramsDealloc;

    result = pthread_create(&tid, &attr, __CEThreadMain, &context);
    if (0 != result) {
        CELogError("pthread_create %s\n", strerror(result));
        CEThreadCreateDeallocParmas;
        return NULL;
    }
    CESemWait(context.sem);
    CESemDeinit(context.sem);
    
    return context.thread;
}
