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


typedef struct _CEThreadWaiter {
    sem_t * _Nonnull lock;
    sem_t lockValue;//private
#if __APPLE__
    char name[1024];
#endif
} CEThreadWaiter_s;


CEThreadWaiter_s * _Nonnull CEThreadWaiterInit(void);
void CEThreadWaiterDeinit(CEThreadWaiter_s * _Nonnull waiter);

pthread_key_t CERunLoopWaiterThreadKeyShared(void);

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




static inline CEThreadWaiter_s * _Nullable CEThreadGetWaiterIfCreated(void) {
    CEThreadWaiter_s * waiter = (CEThreadWaiter_s *)pthread_getspecific(CERunLoopWaiterThreadKeyShared());
    return waiter;
}
void __CERunLoopWaiterThreadKeySharedValueDealloc(void * _Nullable value) {
    CEThreadWaiter_s * waiter = CEThreadGetWaiterIfCreated();
    if (NULL != waiter) {
        CEThreadWaiterDeinit(waiter);
        pthread_setspecific(CERunLoopWaiterThreadKeyShared(), NULL);
    }
}
static pthread_key_t __CERunLoopWaiterThreadKeyShared = 0;
void __CERunLoopWaiterThreadKeySharedOnceBlockFunc(void) {
    int result = pthread_key_create(&__CERunLoopWaiterThreadKeyShared, __CERunLoopWaiterThreadKeySharedValueDealloc);
    assert(result == 0);
}
pthread_key_t CERunLoopWaiterThreadKeyShared(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CERunLoopWaiterThreadKeySharedOnceBlockFunc);
    return __CERunLoopThreadKeyShared;
}

CEThreadWaiter_s * _Nonnull CEThreadGetWaiter(void) {
    CEThreadWaiter_s * waiter = CEThreadGetWaiterIfCreated();
    if (NULL == waiter) {
        waiter = CEThreadWaiterInit();
        pthread_setspecific(CERunLoopWaiterThreadKeyShared(), waiter);
    }
    return waiter;
}

CEThreadWaiter_s * _Nonnull CEThreadWaiterInit(void) {
    CEThreadWaiter_s * waiter = CEAllocateClear(sizeof(CEThreadWaiter_s));
//    PSEMNAMLEN
#if __APPLE__
    snprintf(waiter->name, 1024, "/%x.%15llx.%x", getpid(), (long long)pthread_self(), arc4random() >> 16);
    sem_t * sem = sem_open(waiter->name, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (SEM_FAILED == sem) {
        fprintf(stderr, "CEThreadWaiterInit sem_open error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
    waiter->lock = sem;
#else
    if (0 != sem_init(&(waiter->lockValue), 0, 0)) {
        fprintf(stderr, "CEThreadWaiterInit sem_init error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
    waiter->lock = &(waiter->lockValue);
#endif

    return waiter;
}

void CEThreadWaiterDeinit(CEThreadWaiter_s * _Nonnull waiter) {
#if __APPLE__
    sem_close(waiter->lock);
    sem_unlink(waiter->name);
#else
    sem_destroy(waiter->lock);
#endif
    
    CEDeallocate(waiter);
}

void CEThreadWaiterWait(CEThreadWaiter_s * _Nonnull waiter) {
    if (0 != sem_wait(waiter->lock)) {
        fprintf(stderr, "CEThreadWaiterWait sem_wait error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
}
void CEThreadWaiterWakeUp(CEThreadWaiter_s * _Nonnull waiter) {
    if (0 != sem_post(waiter->lock)) {
        fprintf(stderr, "CEThreadWaiterWakeUp sem_post error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
}





typedef struct _CEThreadContext {
    void * _Nullable (* _Nonnull main)(void * _Nullable);
    CEThreadWaiter_s * _Nonnull waiter;
    CEThreadRef _Nullable thread;
} CEThreadContext_s;

void * __CEThreadMain(void * args) {
    CEThreadContext_s * eventLoop = (CEThreadContext_s *)args;
    
    CERunLoopRun(eventLoop);
    return NULL;
}

CEThreadRef _Nullable CEThreadCreate(CEThreadConfig_s config, CERunLoop_s * _Nonnull eventLoop, void * _Nullable (* _Nonnull main)(void * _Nullable)) {
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
        return 0;
    }
    result = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    if (0 != result) {
        CELogError("pthread_attr_setscope PTHREAD_SCOPE_SYSTEM %s\n", strerror(result));
        return 0;
    }
    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (0 != result) {
        CELogError("pthread_attr_setdetachstate PTHREAD_CREATE_DETACHED %s\n", strerror(result));
        return 0;
    }
    
    size_t stacksize = 0;
    result = pthread_attr_getstacksize(&attr, &stacksize);
    
    if (0 != result) {
        CELogError("pthread_attr_getstacksize error %s\n", strerror(result));
        return 0;
    }
    
    size_t rStacksize = 1024 * 1024 * 2;
    if (stacksize < rStacksize) {
        result = pthread_attr_setstacksize(&attr, rStacksize);//设置线程堆栈大小
        if (0 != result) {
            CELogError("pthread_attr_setstacksize error %s\n", strerror(result));
            return 0;
        }
    }
    
    
    int priority = sched_get_priority_max(policy);
    if (-1 == priority) {
        CELogError("sched_get_priority_max error %s\n", strerror(result));
        return 0;
    }
    
    param.sched_priority = priority;
    
    result = pthread_attr_setschedparam(&attr, &param);
    if (0 != result) {
        CELogError("pthread_attr_setschedparam %s\n", strerror(result));
        return 0;
    }
    CEThreadContext_s context = {};
    
    
    result = pthread_create(&tid, &attr, CERunLoopRunThread, eventLoop);
    if (0 != result) {
        CELogError("pthread_create %s\n", strerror(result));
        return 0;
    }
    return 1;
}


void CERunLoopRunDefault(void) {
    CERunLoop_s * eventLoop = CERunLoopShared();
    
    uint32_t runningStates = atomic_load(&(eventLoop->runningStates));
    if (0 != runningStates) {
        return;
    }
    assert(CERunLoopMakeThreadRunDefault(eventLoop));
}
void CERunLoopWakeUp(CERunLoop_s * _Nonnull eventLoop) {
    assert(eventLoop);
    CEApiWakeUp(eventLoop->api);
}
