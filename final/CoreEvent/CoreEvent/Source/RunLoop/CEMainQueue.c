//
//  CEMainQueue.c
//  CoreEvent
//
//  Created by vector on 2019/6/11.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEMainQueue.h"
#include "CESerialQueue.h"
#include "CEThread.h"
#include "CETaskScheduler.h"
#include "CESource.h"
#include "CEQueueInternal.h"


void CEMainTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    //do wake up
    CESemSignal(scheduler->waiter);
}

void CEMainSourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    CESourceCount_t count = 0;
    CESpinLockLock(source->lock);
    CESourceSerialContext_s * context = source->context;
    CESourceTaskStoreAppend(&(context->tasks), task);
    context->count += 1;
    count = context->count;
    CESpinLockUnlock(source->lock);
    if (count == 1) {
        //weak up
        CEMainTaskSchedulerSignal(context->scheduler);
    }
}
CETaskPtr _Nonnull CESourceMainQueueRemove(CESourceRef _Nonnull source) {
    assert(source);
    CESourceSerialContext_s * context = source->context;
    CETaskPtr result = NULL;
    
    while (NULL == result) {
        CESpinLockLock(source->lock);
        result = CESourceTaskStoreRemove(&(context->highLevelTasks));
        if (NULL == result) {
            result = CESourceTaskStoreRemove(&(context->tasks));
        }
        CESpinLockUnlock(source->lock);
        if (NULL == result) {
            CESemWait(context->scheduler->waiter);
        }
    }
    return result;
}
CETaskPtr _Nonnull CESourceMainQueueFinishOneTaskAndRemove(CESourceRef _Nonnull source) {
    CESourceCount_t count = 0;
    CETaskPtr result = NULL;
    CESourceSerialContext_s * context = source->context;
    CESpinLockLock(source->lock);
    context->count -= 1;
    count = context->count;
    
    if (count == 0) {
        // wait
        CESpinLockUnlock(source->lock);
        CESemWait(context->scheduler->waiter);
        CESpinLockLock(source->lock);
    }
    
    result = CESourceTaskStoreRemove(&(context->highLevelTasks));
    if (NULL == result) {
        result = CESourceTaskStoreRemove(&(context->tasks));
    }
    CESpinLockUnlock(source->lock);
    assert(result);
    return result;
}

void CEQueueMain(void) {
    assert(CEIsMainThread());
    CEQueueSharedMainQueue();
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    
    CETaskPtr task = CESourceMainQueueRemove(scheduler->source);
    while (1) {
        CETaskSchedulerExecuteTask(scheduler, task);
        task = CESourceMainQueueFinishOneTaskAndRemove(scheduler->source);
    }
}


//source
CESource_s * _Nonnull _CESourceMainCreate(CEQueue_s * _Nonnull queue, CEPtr _Nonnull context) {
    assert(queue);
    assert(context);
    CESource_s * source = CESourceCreate(queue, context, CEMainSourceAppend);
    return source;
}
