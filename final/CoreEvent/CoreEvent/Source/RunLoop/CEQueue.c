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



void CEQueueSerialSync(CEQueuePtr _Nonnull queue, CESyncTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {
}
void CEQueueConcurrentSync(CEQueuePtr _Nonnull queue, CESyncTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {
}

void CEQueueSync(CEQueuePtr _Nonnull queuePtr, CEParamRef _Nonnull param, CEParamRef _Nullable result, CEFunction_f _Nonnull execute) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    
    if (param) {
        CERetain(param);
    }
    if (result) {
        CERetain(result);
    }
    
    
    
    CEThreadSpecificRef specific = CEThreadSpecificGetCurrent();
    CEThreadSchedulerRef scheduler = specific->scheduler;
    //获取sync任务的上下文 获取不到， 则创建
//    CETaskSyncContextPtr syncContext = specific->syncContext;

    CETaskContext_s context = CETaskContexPush();
    
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    CETaskPtr task = NULL;
    task->isSyncTask = 1;
    task->condPtr = &cond;
    CESourceAppend(queue->source, task);//转移
    assert(0 == pthread_cond_init(&cond, NULL));
    
    pthread_cond_wait(<#pthread_cond_t *restrict _Nonnull#>, <#pthread_mutex_t *restrict _Nonnull#>)
    //task 不可再用
    pthread_cond_destroy(&cond);
    
}

CETaskPtr _Nullable CETaskSchedulerRemoveTask(CETaskSchedulerRef _Nonnull scheduler) {
    assert(scheduler);

    CETaskPtr task = NULL;
    CESpinLockLock(&(scheduler->lock));
    if (NULL != scheduler->source) {
        task = CESourceRemove(scheduler->source);
    }
    CESpinLockUnlock(&(scheduler->lock));

    return task;
}

_Bool CETaskSchedulerBindSource(CETaskSchedulerRef _Nonnull scheduler, CESourceRef _Nonnull source) {
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
CESourceRef _Nullable CETaskSchedulerUnbindSource(CETaskSchedulerRef _Nonnull scheduler) {
    assert(scheduler);
    CESourceRef _Nonnull source = NULL;
    CESpinLockLock(&(scheduler->lock));
    source = scheduler->source;
    scheduler->source = NULL;
    CESpinLockUnlock(&(scheduler->lock));
    return source;
}
void CETaskSchedulerWait(CETaskSchedulerRef _Nonnull scheduler) {
    
    
    //wait
}
void CETaskSchedulerWakeUp(CETaskSchedulerRef _Nonnull scheduler) {
    uint_fast32_t status = 0;
    uint_fast32_t newStatus = 0;
    uint32_t times = 0;
    do {
        times += 1;
        assert(1 == times);
        status = atomic_load(&(scheduler->status));
        
        switch (status) {
            case CETaskSchedulerStatusNoThread: {
                newStatus = CETaskSchedulerStatusCreatingThread;
            }
                break;
            case CETaskSchedulerStatusWaiting: {
                newStatus = CETaskSchedulerStatusRunning;
            }
                break;
            case CETaskSchedulerStatusRunning://穿透
            default: {
                abort();
                return;
            }
                break;
        }
    } while (!atomic_compare_exchange_strong(&(scheduler->status), &status, newStatus));

    if (CETaskSchedulerStatusNoThread == newStatus) {
        CEThreadConfig_s config = {};
        snprintf(config.name, 63, "CoreEvent.global.thread.%u", scheduler->id);
        config.schedPriority = 0;
        _CEThreadCreate(config, scheduler, CEQueueBeforeMainFunc, CEQueueMainFunc, NULL, NULL);
    } else if (CETaskSchedulerStatusWaiting == newStatus) {
        //do wake up

    } else {
        abort();
    }
}

void CEGlobalThreadManagerDispatch(CESourceRef _Nonnull source) {
    CETaskSchedulerRef scheduler = CEGlobalThreadManagerDeQueue();
    if (scheduler) {
        assert(CETaskSchedulerBindSource(scheduler, source));
        CETaskSchedulerWakeUp(scheduler);
    }
}

void CEQueueBeforeMainFunc(CEThreadSpecificRef _Nonnull specific) {
    CETaskSchedulerRef scheduler = specific->scheduler;
    assert(scheduler);
    uint_fast32_t status = 0;
    uint_fast32_t newStatus = 0;
    uint32_t times = 0;
    do {
        times += 1;
        assert(1 == times);
        status = atomic_load(&(scheduler->status));
        assert(CETaskSchedulerStatusCreatingThread == status);
        newStatus = CETaskSchedulerStatusRunning;
    } while (!atomic_compare_exchange_strong(&(scheduler->status), &status, newStatus));
}


void CEQueueMainFunc(void * _Nullable param) {
    CEThreadSpecificRef specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerRef scheduler = specific->scheduler;
    assert(scheduler);
    
    uint_fast32_t status = atomic_load(&(scheduler->status));
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
                    
                    
                } else {
                    //change status
                    uint_fast32_t newStatus = CETaskSchedulerStatusWaiting;
                    if (atomic_compare_exchange_strong(&(scheduler->status), &status, newStatus)) {
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
        status = atomic_load(&(scheduler->status));
    }
}
