//
//  CEBase.c
//  CoreEvent
//
//  Created by CaiLianfeng on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#include "CEBase.h"



void CESpinLockInit(CESpinLock_t * _Nonnull lockRef) {
    assert(lockRef);
#if __APPLE__
    *lockRef = OS_UNFAIR_LOCK_INIT;
#else
    int result = pthread_spin_init(lockRef, PTHREAD_PROCESS_PRIVATE);
    assert(result == 0);
#endif
}
void CESpinLockDeinit(CESpinLock_t * _Nonnull lockRef) {
    assert(lockRef);

#if __APPLE__
#else
    int result = pthread_spin_destroy(lockRef);
    assert(result == 0);
#endif
}
void CESpinLockLock(CESpinLock_t * _Nonnull lockRef) {
    assert(lockRef);

#if __APPLE__
    os_unfair_lock_lock(lockRef);
#else
    pthread_spin_lock(lockRef);
#endif
}

void CESpinLockUnlock(CESpinLock_t * _Nonnull lockRef) {
    assert(lockRef);
#if __APPLE__
    os_unfair_lock_unlock(lockRef);
#else
    pthread_spin_unlock(lockRef);
#endif
}
