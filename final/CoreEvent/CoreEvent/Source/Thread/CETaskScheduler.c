//
//  CETaskScheduler.c
//  CoreEvent
//
//  Created by vector on 2019/6/1.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CETaskScheduler.h"
#include "CEMemory.h"
#include "CEQueue.h"

void CETaskSchedulerExecuteTask(CETaskSchedulerPtr _Nonnull scheduler, CETaskPtr _Nonnull task) {
    assert(scheduler);
    assert(task);
    
    if (NULL != task->syncTaskWaiter) {
        CEThreadSyncWaiterSignal(task->syncTaskWaiter);
    }
}


CETaskSchedulerPtr _Nonnull CETaskSchedulerCreate(CEQueue_s * _Nullable ownerQueue) {
    assert(signal);
    CETaskSchedulerPtr scheduler = CEAllocateClear(sizeof(CETaskScheduler_s));
    scheduler->lock = CESpinLockCreate();
    scheduler->waiter = CESemCreate(0);
    scheduler->ownerQueue = ownerQueue;
    return scheduler;
}

void CETaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemDestroy(scheduler->waiter);
    CESpinLockDestroy(scheduler->lock);
}
