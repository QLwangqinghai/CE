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
#include "CEThreadBaseInternal.h"
#include "CELog.h"

#include "CESem.h"

void _CEThreadDescript(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript);

size_t _CEThreadGetSize(CERef _Nonnull p) {
    return sizeof(CEThread_s);
}
const CEType_s __CETypeThread = {
    .type = CETypeMate,
    .version = CERuntimeVersion,
    .masks = CETypeMaskNoRc,
    .objectSize = sizeof(CEThread_s),
    .name = "CEThread",
    .identifier = (uintptr_t)(&__CETypeThread),
    .alloctor = &__CETypeDefaultAlloctor,
    
    .deinit = CETypeDefaultDeinit,
    .descript = _CEThreadDescript,
    
    .class = NULL,
};

CETypeRef _Nonnull CETypeThread = &__CETypeThread;

void _CEThreadDescript(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript) {
    assert(object);
    CETypeRef type = CERefGetType(object);
    assert(CETypeIsEqual(type, CETypeThread));

    CEThreadRef thread = object;
    
    char buffer[32] = {};
    descript(handler, "<");
    descript(handler, ((CEType_s *)object)->name);
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

CEThreadSpecificRef _Nonnull CEThreadSpecificInit(void) {
    CEThreadSpecificRef specific = CEAllocateClear(sizeof(CEThreadSpecific_s));
    return specific;
}

void CEThreadSpecificDeinit(CEThreadSpecificRef _Nonnull specific) {

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

CEThreadSpecificRef _Nonnull _CEThreadSpecificGetCurrent(char * _Nullable threadName, _Bool copyThreadName) {
    CEThreadSpecificRef specific = (CEThreadSpecificRef)pthread_getspecific(CEThreadSpecificKeyShared());
    if (NULL == specific) {
        specific = CEThreadSpecificInit();
        CEThread_s * thread = &(specific->thread);
        memcpy(&(thread->runtime), &CETypeThread, sizeof(CETypeRef));
        thread->pthread = pthread_self();
        if (threadName && !copyThreadName) {
            specific->thread.name = threadName;
        } else {
            char * name = NULL;
            size_t size = 0;
            if (threadName) {
                if (copyThreadName) {
                    size_t len = strlen(threadName);
                    if (len <= 63) {
                        size = 64;
                    } else {
                        size = (len /*(+ 1 - 1)*/ + CECpuWordByteSize ) / CECpuWordByteSize * CECpuWordByteSize;
                    }
                    name = CEAllocateClear(size);
                    memcpy(name, threadName, len);
                } else {
                    name = threadName;
                }
            } else {
                name = CEAllocateClear(64);
                snprintf(name, 63, "thread: %p", specific);
            }
            specific->thread.name = name;
            specific->thread.nameBufferLength = size;
        }
        pthread_setspecific(CEThreadSpecificKeyShared(), specific);
    }
    return specific;
}

CEThreadSpecificRef _Nonnull CEThreadSpecificGetCurrent(void) {
    if (CEIsMainThread()) {
        return _CEThreadSpecificGetCurrent(__CEMainThreadDefaultName, false);
    } else {
        return _CEThreadSpecificGetCurrent(NULL, false);
    }
}

CEThreadRef _Nonnull CEThreadGetCurrent(void) {
    return &(CEThreadSpecificGetCurrent()->thread);
}

CEThreadLoaderRef _Nonnull CEThreadLooperGetCurrent(void) {
    return CEThreadSpecificGetCurrent()->loader;
}


CEThreadLoaderRef _Nonnull __CEThreadLooperCreate(CEThreadRef _Nonnull thread) {
    CEThreadLoaderRef looper = CEAllocateClear(sizeof(CEThreadLoader_s));
    looper->thread = thread;
    return looper;
}


struct __CEThreadContext {
    void * _Nullable (* _Nonnull main)(void * _Nullable);
    void * _Nullable params;
    void (* _Nullable paramsDealloc)(void * _Nonnull);
    CESemPtr _Nonnull sem;
    CERunLoopRef _Nonnull (* _Nonnull runLoopLoader)(CEThreadRef _Nonnull);
    CEThreadRef _Nullable thread;
};

void * __CEThreadMain(void * args) {
    struct __CEThreadContext * context = (struct __CEThreadContext *)args;
    
    CEThreadRef thread = CEThreadGetCurrent();
    CEThreadLoaderRef looper = __CEThreadLooperCreate(thread);
    looper->loadRunLoop = context->runLoopLoader;
    looper->status = CEThreadStatusStarting;
    context->thread = thread;
    void * _Nullable params = context->params;
    void (* _Nullable paramsDealloc)(void * _Nonnull) = context->paramsDealloc;
    CESemWakeUp(context->sem);
    context = NULL;
    looper->status = CEThreadStatusExecuting;
    void * result = context->main(params);
    looper->status = CEThreadStatusFinished;
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
