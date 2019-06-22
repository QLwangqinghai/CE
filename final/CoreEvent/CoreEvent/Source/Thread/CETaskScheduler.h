//
//  CETaskScheduler.h
//  CoreEvent
//
//  Created by vector on 2019/6/1.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CETaskScheduler_h
#define CETaskScheduler_h

#include "CEThreadBaseInternal.h"





/*
 typedef struct _CETaskScheduler {
 CESpinLockPtr _Nonnull lock;
 CESemPtr _Nonnull waiter;
 
 CEThread_s * _Nonnull thread;
 CEQueueRef _Nullable ownerQueue;//当前queue， 如果是个串行队列的线程，ownerQueue 一直有值
 CESourceRef _Nonnull source;
  
 uint8_t context[64];
 } CETaskScheduler_s;

 */



static inline void CETaskSchedulerStoreTask(CETaskSchedulerPtr _Nonnull scheduler, CETaskPtr _Nonnull task) {
    uintptr_t t = atomic_load(&(scheduler->_t));
    assert((uintptr_t)NULL == t);
    uintptr_t newValue = (uintptr_t)task;
    assert(atomic_compare_exchange_strong(&(scheduler->_t), &t, newValue));
}

static inline CETaskPtr _Nonnull CETaskSchedulerRemoveTask(CETaskSchedulerPtr _Nonnull scheduler) {
    uintptr_t t = atomic_load(&(scheduler->_t));
    uintptr_t newValue = (uintptr_t)NULL;
    assert(newValue != t);
    assert(atomic_compare_exchange_strong(&(scheduler->_t), &t, newValue));
    return (CETaskPtr)t;
}





void CETaskSchedulerExecuteTask(CETaskSchedulerPtr _Nonnull scheduler, CETaskPtr _Nonnull task);

CETaskSchedulerPtr _Nonnull CETaskSchedulerCreate(uint16_t qid, uint16_t sid);

//void CETaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler);


#endif /* CETaskScheduler_h */
