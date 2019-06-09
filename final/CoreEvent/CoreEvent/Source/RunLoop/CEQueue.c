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
#include "CETaskContext.h"
#include "CERuntime.h"
#include "CEConditionLock.h"
#include "CESem.h"
#include "CETaskScheduler.h"

const CETypeSpecific_s CETypeSpecificQueue = {
    .name = "CEQueue",
    .descript = CETypeDefaultDescript,
    .deinit = CERuntimeDefaultDeinit,
};

const CEType_s __CETypeQueue = CEType(CETypeBitHasRc | CETypeBitStatic, 0, (uintptr_t)(&__CETypeQueue), &CETypeSpecificQueue);


CETypeRef _Nonnull CETypeQueue = &__CETypeQueue;

static inline CEQueue_s * _Nonnull CEQueueCheck(CEQueuePtr _Nonnull queuePtr) {
    assert(queuePtr);
    CETypeRef type = CERefGetType(queuePtr);
    assert(CETypeIsEqual(type, CETypeQueue));
    return (CEQueue_s *)queuePtr;
}

//void CEQueueSerialSync(CEQueuePtr _Nonnull queue, CESyncTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {
//}
//void CEQueueConcurrentSync(CEQueuePtr _Nonnull queue, CESyncTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {
//}

void CEQueueSync(CEQueuePtr _Nonnull queuePtr, CEParamRef _Nonnull param, CEParamRef _Nullable result, CEFunction_f _Nonnull execute) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    
    if (param) {
        CERetain(param);
    }
    if (result) {
        CERetain(result);
    }
    
    CEThreadSpecificRef specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    //获取sync任务的上下文 获取不到， 则创建
//    CETaskSyncContextPtr syncContext = specific->syncContext;
    
    CETaskContext_s context = CETaskContexPush();

    CETaskPtr task = NULL;
    task->isSyncTask = 1;
    task->syncTaskWaiter = specific->syncWaiter;
    CESourceAppend(queue->source, task);//转移
    CESemWait(specific->syncWaiter);
}

CETaskPtr _Nullable CETaskSchedulerRemoveTask(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);

    CETaskPtr task = NULL;
    CESpinLockLock(&(scheduler->lock));
    if (NULL != scheduler->source) {
        task = CESourceRemove(scheduler->source);
    }
    CESpinLockUnlock(&(scheduler->lock));

    return task;
}

_Bool CETaskSchedulerBindSource(CETaskSchedulerPtr _Nonnull scheduler, CESourceRef _Nonnull source) {
    assert(scheduler);
    _Bool result = true;
    CESpinLockLock(&(scheduler->lock));
    if (NULL == scheduler->source) {
        scheduler->source = source;
    } else {
        result = false;
    }
    CESpinLockUnlock(&(scheduler->lock));
    
    return result;
}
CESourceRef _Nullable CETaskSchedulerUnbindSource(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    CESourceRef _Nonnull source = NULL;
    CESpinLockLock(&(scheduler->lock));
    source = scheduler->source;
    scheduler->source = NULL;
    CESpinLockUnlock(&(scheduler->lock));
    return source;
}


void CEGlobalThreadManagerDispatch(CESourceRef _Nonnull source) {
    CETaskSchedulerPtr scheduler = CEGlobalThreadManagerDeQueue();
    if (scheduler) {
        assert(CETaskSchedulerBindSource(scheduler, source));
        CETaskSchedulerSignal(scheduler);
    }
}

void CEQueueBeforeMainFunc(CEThreadSpecificRef _Nonnull specific) {
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    CEGlobalThreadTaskSchedulerContext_s * context = (CEGlobalThreadTaskSchedulerContext_s *)scheduler->context;
    uint_fast32_t status = 0;
    uint_fast32_t newStatus = 0;
    uint32_t times = 0;
    do {
        times += 1;
        assert(1 == times);
        status = atomic_load(&(context->status));
        assert(CETaskSchedulerStatusCreatingThread == status);
        newStatus = CETaskSchedulerStatusRunning;
    } while (!atomic_compare_exchange_strong(&(context->status), &status, newStatus));
}


void CEQueueMainFunc(void * _Nullable param) {
    CEThreadSpecificRef specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    CEGlobalThreadTaskSchedulerContext_s * context = (CEGlobalThreadTaskSchedulerContext_s *)scheduler->context;

    uint_fast32_t status = atomic_load(&(context->status));
    assert(CETaskSchedulerStatusRunning == status);
    while (1) {
        switch (status) {
            case CETaskSchedulerStatusWaiting: {
                CETaskSchedulerUnbindSource(scheduler);
                CEGlobalThreadManagerEnQueue(scheduler);
                
                CETaskSchedulerWait(scheduler);
            }
                break;
            case CETaskSchedulerStatusRunning: {
                CETaskPtr task = CETaskSchedulerRemoveTask(scheduler);
                if (task) {
                    //do task
                    
                    
                    
                    
                    if (NULL != task->syncTaskWaiter) {
                        CESemSignal(task->syncTaskWaiter);
                    }
                } else {
                    //change status
                    uint_fast32_t newStatus = CETaskSchedulerStatusWaiting;
                    if (atomic_compare_exchange_strong(&(context->status), &status, newStatus)) {
                        status = newStatus;
                        continue;
                    }
                }
            }
                break;
            case CETaskSchedulerStatusCreatingThread://穿透
            case CETaskSchedulerStatusNoThread://穿透
            default: {
                abort();
            }
                break;
        }
        status = atomic_load(&(context->status));
    }
}
