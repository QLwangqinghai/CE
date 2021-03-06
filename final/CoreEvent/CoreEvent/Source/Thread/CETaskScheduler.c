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
#include "CEQueueLog.h"

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
    CEQueueLog("ce.task.execute.start(qid:%x, sid:%x, tag:%x)", scheduler->qid, scheduler->sid, task->tag);
    
    CEFunction_f execute = task->execute;
    assert(execute);
    execute(task->obj, task->param, task->resultReceiver);
    
    if (NULL != task->syncTaskWaiter) {
        CEThreadSyncWaiterSignal(task->syncTaskWaiter);
        CEQueueLog("ce.task.execute.signal(qid:%x, sid:%x, tag:%x)", scheduler->qid, scheduler->sid, task->tag);
    }
    CEQueueLog("ce.task.execute.finish(qid:%x, sid:%x, tag:%x)", scheduler->qid, scheduler->sid, task->tag);
    scheduler->executingTaskTag = 0;
}


CETaskSchedulerPtr _Nonnull CETaskSchedulerCreate(uint16_t qid, uint16_t sid) {
    assert(signal);
    CETaskSchedulerPtr scheduler = CEAllocateClear(sizeof(CETaskScheduler_s));
    scheduler->lock = CESpinLockCreate();
    scheduler->waiter = CESemCreate(0);
    scheduler->qid = qid;
    scheduler->sid = sid;
    uintptr_t thread = 0;
    atomic_init(&(scheduler->thread), thread);
    uintptr_t _t = 0;
    atomic_init(&(scheduler->_t), _t);
    CEQueueLog("ce.task.scheduler.create(qid:%x, sid:%x)", qid, sid);
    return scheduler;
}

//void CETaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler) {
//    CESemDestroy(scheduler->waiter);
//    CESpinLockDestroy(scheduler->lock);
//}


