//
//  CEQueue.c
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEQueue.h"
#include "CEThread.h"
#include "CEThreadBaseInternal.h"
#include "CERuntime.h"
#include "CEConditionLock.h"
#include "CESem.h"
#include "CETaskScheduler.h"
#include "CETask.h"

const CETypeSpecific_s CETypeSpecificQueue = {
    .name = "CEQueue",
    .descript = CETypeDefaultDescript,
    .deinit = CERuntimeDefaultDeinit,
};

const CEType_s __CETypeQueue = CEType(CETypeBitHasRc | CETypeBitStatic, 0, (uintptr_t)(&__CETypeQueue), &CETypeSpecificQueue);


CETypeRef _Nonnull CETypeQueue = &__CETypeQueue;

static inline CEQueue_s * _Nonnull CEQueueCheck(CEQueueRef _Nonnull queuePtr) {
    assert(queuePtr);
    CETypeRef type = CERefGetType(queuePtr);
    assert(CETypeIsEqual(type, CETypeQueue));
    return (CEQueue_s *)queuePtr;
}

static inline void CEQueueAppend(CEQueue_s * _Nonnull queue, CETaskPtr _Nonnull task) {
    queue->append(queue, task);
}


static inline void _CEQueueJoin(CEQueue_s * _Nonnull queue, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param, CEParamRef _Nullable result, CEThreadSyncWaiter_s * _Nullable syncTaskWaiter, _Bool isBarrier) {

    CETaskPtr task = CETaskCreate(execute, param, result, syncTaskWaiter, isBarrier);
    CEQueueAppend(queue, task);
    
    if (syncTaskWaiter) {
        CEThreadSyncWaiterWait(syncTaskWaiter);
    }
}

void CEQueueSync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param, CEParamRef _Nullable result) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    _CEQueueJoin(queue, execute, param, result, specific->syncWaiter, false);
}

void CEQueueAsync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    _CEQueueJoin(queue, execute, param, NULL, NULL, false);
}

void CEQueueBarrierSync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param, CEParamRef _Nullable result) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    _CEQueueJoin(queue, execute, param, result, specific->syncWaiter, true);
}
void CEQueueBarrierAsync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    _CEQueueJoin(queue, execute, param, NULL, NULL, true);
}


void CEGlobalThreadManagerDispatch(CESourceRef _Nonnull source) {
    CETaskSchedulerPtr scheduler = CEGlobalThreadManagerDeQueue();
    if (scheduler) {
        assert(CETaskSchedulerBindSource(scheduler, source));
        CETaskSchedulerSignal(scheduler);
    }
}
