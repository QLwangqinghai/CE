//
//  CEThreadBase.c
//  CoreEvent
//
//  Created by vector on 2019/5/7.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEThreadBase.h"
#include "CEMemory.h"

CESpinLockPtr _Nonnull CESpinLockCreate(void) {
    CESpinLockPtr lockPtr = CEAllocateClear(sizeof(CESpinLock_t));
    CESpinLockInit(lockPtr);
    return lockPtr;
}
void CESpinLockDestroy(CESpinLockPtr _Nonnull lockPtr) {
    CESpinLockDeinit(lockPtr);
    CEDeallocate(lockPtr);
}

void CESpinLockInit(CESpinLockPtr _Nonnull lockPtr) {
    assert(lockPtr);
#if __APPLE__
    *lockPtr = OS_UNFAIR_LOCK_INIT;
#else
    int result = pthread_spin_init(lockPtr, PTHREAD_PROCESS_PRIVATE);
    assert(result == 0);
#endif
}
void CESpinLockDeinit(CESpinLockPtr _Nonnull lockPtr) {
    assert(lockPtr);
    
#if __APPLE__
#else
    int result = pthread_spin_destroy(lockPtr);
    assert(result == 0);
#endif
}

void CESpinLockLock(CESpinLockPtr _Nonnull lockPtr) {
    assert(lockPtr);
    
#if __APPLE__
    os_unfair_lock_lock(lockPtr);
#else
    pthread_spin_lock(lockPtr);
#endif
}

void CESpinLockUnlock(CESpinLockPtr _Nonnull lockPtr) {
    assert(lockPtr);
#if __APPLE__
    os_unfair_lock_unlock(lockPtr);
#else
    pthread_spin_unlock(lockPtr);
#endif
}
