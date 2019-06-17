//
//  CESem.c
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEThreadBase.h"
#include "CEMemory.h"
#include "CEInternal.h"
#include "CEConfig.h"


#if (CESemUseDispatch == CESemUseType)
#include <dispatch/dispatch.h>

struct _CESem {
     dispatch_semaphore_t _Nonnull lock;
};

CESemPtr _Nonnull CESemCreate(unsigned int value) {
    CESemPtr sem = CEAllocate(sizeof(CESem_s));
    dispatch_semaphore_t lock = dispatch_semaphore_create(value);
    assert(lock);
    sem->lock = lock;
    return sem;
}

void CESemDestroy(CESemPtr _Nonnull sem) {
    assert(sem);
    assert(sem->lock);
    dispatch_release(sem->lock);
    CEDeallocate(sem);
}

void CESemWait(CESemPtr _Nonnull sem) {
    assert(sem);
    assert(sem->lock);
    
    if (0 != dispatch_semaphore_wait(sem->lock, DISPATCH_TIME_FOREVER)) {
        CELogError("CEThreadWaiterWait sem_wait error %s; \n", strerror(errno));
        abort();
    }
}

void CESemSignal(CESemPtr _Nonnull sem) {
    assert(sem);
    assert(sem->lock);
    if (0 > dispatch_semaphore_signal(sem->lock)) {
        CELogError("CEThreadWaiterWakeUp sem_post error %s; \n", strerror(errno));
        abort();
    }
}


#elif (CESemUseFileSem == CESemUseType)
#include <semaphore.h>

struct _CESem {
    sem_t * _Nonnull lock;
    char name[32];
};

CESemPtr _Nonnull CESemCreate(unsigned int value) {
    CESemPtr sem = CEAllocate(sizeof(CESem_s));
    
    char name[48] = {};
    snprintf(name, 31, "/%x.%llx.", getpid(), (long long)pthread_self());
    size_t nameLen = strlen(name);
    
    char * tmpName = name + nameLen;
    while (nameLen < 30) {
        snprintf(tmpName, 48 - nameLen, "%x", arc4random());
        nameLen += 8;
        tmpName = name + nameLen;
    }
    tmpName = name + 30;
    memset(tmpName, 0, 18);
    memcpy(sem->name, name, 32);
    
    sem_t * s = sem_open(sem->name, O_CREAT, S_IRUSR | S_IWUSR, value);
    if (SEM_FAILED == s) {
        CELogError("CEThreadWaiterInit sem_open error %s; \n", strerror(errno));
        abort();
    }
    sem->lock = s;

    return sem;
}

void CESemDestroy(CESemPtr _Nonnull sem) {
    assert(sem);
    assert(sem->lock);

    sem_close(sem->lock);
    sem_unlink(sem->name);

    CEDeallocate(sem);
}

void CESemWait(CESemPtr _Nonnull sem) {
    assert(sem);
    assert(sem->lock);
    if (0 != sem_wait(sem->lock)) {
        CELogError("CEThreadWaiterWait sem_wait error %s; \n", strerror(errno));
        abort();
    }
}

void CESemSignal(CESemPtr _Nonnull sem) {
    assert(sem);
    assert(sem->lock);
    if (0 != sem_post(sem->lock)) {
        CELogError("CEThreadWaiterWakeUp sem_post error %s; \n", strerror(errno));
        abort();
    }
}

#elif (CESemUseDefaultSem == CESemUseType)
#include <semaphore.h>

struct _CESem {
    sem_t * _Nonnull lock;
    sem_t lockValue;//private
};
CESemPtr _Nonnull CESemCreate(unsigned int value) {
    CESemPtr sem = CEAllocate(sizeof(CESem_s));
    if (0 != sem_init(&(sem->lockValue), 0, value)) {
        CELogError("CEThreadWaiterInit sem_init error %s; \n", strerror(errno));
        abort();
    }
    sem->lock = &(sem->lockValue);
    return sem;
}
void CESemDestroy(CESemPtr _Nonnull sem) {
    assert(sem);
    assert(sem->lock);
    sem_destroy(sem->lock);
    CEDeallocate(sem);
}

void CESemWait(CESemPtr _Nonnull sem) {
    assert(sem);
    assert(sem->lock);
    if (0 != sem_wait(sem->lock)) {
        CELogError("CEThreadWaiterWait sem_wait error %s; \n", strerror(errno));
        abort();
    }
}

void CESemSignal(CESemPtr _Nonnull sem) {
    assert(sem);
    assert(sem->lock);
    if (0 != sem_post(sem->lock)) {
        CELogError("CEThreadWaiterWakeUp sem_post error %s; \n", strerror(errno));
        abort();
    }
}


#else
#error unkown CESemUseType
#endif
