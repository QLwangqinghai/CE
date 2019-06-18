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

/*
 struct _CETask {
 CETaskPtr _Nullable next;
 
 CEFunction_f _Nonnull execute;
 CETaskParamRef _Nullable param;
 CETaskParamRef _Nullable resultReceiver;
 CEThreadSyncWaiter_s * _Nullable syncTaskWaiter;
 
 uint32_t isBarrier: 1;
 uint32_t mask: 31;
 };
 */

void CETaskSchedulerExecuteTask(CETaskSchedulerPtr _Nonnull scheduler, CETaskPtr _Nonnull task) {
    assert(scheduler);
    assert(task);
    scheduler->executingTaskTag = task->tag;
    CEFunction_f execute = task->execute;
    assert(execute);
    execute(task->obj, task->param, task->resultReceiver);
    
    if (NULL != task->syncTaskWaiter) {
        CEThreadSyncWaiterSignal(task->syncTaskWaiter);
    }
    scheduler->executingTaskTag = 0;
}


CETaskSchedulerPtr _Nonnull CETaskSchedulerCreate(uint16_t qid, uint16_t sid) {
    assert(signal);
    CETaskSchedulerPtr scheduler = CEAllocateClear(sizeof(CETaskScheduler_s));
    scheduler->lock = CESpinLockCreate();
    scheduler->waiter = CESemCreate(0);
    scheduler->qid = qid;
    scheduler->sid = sid;
    return scheduler;
}

void CETaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemDestroy(scheduler->waiter);
    CESpinLockDestroy(scheduler->lock);
}
