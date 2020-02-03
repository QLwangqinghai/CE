//
//  CEThread.c
//  CoreEvent
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEThread.h"
#include "CEMemory.h"

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
    atomic_store(&(waiter->whoWakeUp), (uintptr_t)(NULL));
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
