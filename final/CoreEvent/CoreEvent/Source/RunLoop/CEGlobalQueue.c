//
//  CEGlobalQueue.c
//  CoreEvent
//
//  Created by vector on 2019/6/10.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEGlobalQueue.h"
#include "CETaskScheduler.h"

typedef struct _CEGlobalThreadTaskSchedulerContext {
    uint32_t id;
    _Atomic(uint_fast32_t) status;
} CEGlobalThreadTaskSchedulerContext_s;

//void CEGlobalThreadManagerDispatch(CESourceRef _Nonnull source) {
//    CETaskSchedulerPtr scheduler = CEGlobalThreadManagerDeQueue();
//    if (scheduler) {
//        assert(CETaskSchedulerBindSource(scheduler, source));
//        CETaskSchedulerSignal(scheduler);
//    }
//}


void CEGlobalTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler);


void CEGlobalQueueMainFunc(void * _Nullable param);
void CEGlobalQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific);


CETaskPtr _Nullable CEGlobalTaskSchedulerRemoveTask(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    
    CETaskPtr task = NULL;
    CESpinLockLock(scheduler->lock);
    if (NULL != scheduler->source) {
        task = CESourceRemove(scheduler->source);
    }
    CESpinLockUnlock(scheduler->lock);
    
    return task;
}

_Bool CEGlobalTaskSchedulerBindSource(CETaskSchedulerPtr _Nonnull scheduler, CESourceRef _Nonnull source) {
    assert(scheduler);
    _Bool result = true;
    CESpinLockLock(scheduler->lock);
    if (NULL == scheduler->source) {
        scheduler->source = source;
    } else {
        result = false;
    }
    CESpinLockUnlock(scheduler->lock);
    
    return result;
}
CESourceRef _Nullable CEGlobalTaskSchedulerUnbindSource(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    CESourceRef _Nonnull source = NULL;
    CESpinLockLock(scheduler->lock);
    source = scheduler->source;
    scheduler->source = NULL;
    CESpinLockUnlock(scheduler->lock);
    return source;
}


CETaskSchedulerPtr _Nonnull CEGlobalTaskSchedulerCreate(uint32_t id) {
    CETaskSchedulerPtr scheduler = CETaskSchedulerCreate(NULL, CEGlobalTaskSchedulerSignal);
    
    CEGlobalThreadTaskSchedulerContext_s * context = (CEGlobalThreadTaskSchedulerContext_s *)scheduler->context;
    context->id = id;
    atomic_store(&(context->status), CETaskSchedulerStatusNoThread);
    return scheduler;
}

void CEGlobalTaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler) {
    
    
    
    CETaskSchedulerDestroy(scheduler);
}

void CEGlobalTaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemWait(scheduler->waiter);
}
void CEGlobalTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    assert(CEQueueTypeGolbal == scheduler->source->queue->type);
    
    uint_fast32_t status = 0;
    uint_fast32_t newStatus = 0;
    uint32_t times = 0;
    CEGlobalThreadTaskSchedulerContext_s * context = (CEGlobalThreadTaskSchedulerContext_s *)scheduler->context;
    
    do {
        times += 1;
        assert(1 == times);
        status = atomic_load(&(context->status));
        
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
    } while (!atomic_compare_exchange_strong(&(context->status), &status, newStatus));
    
    
    if (CETaskSchedulerStatusCreatingThread == newStatus) {
        CEThreadConfig_s config = {};
        float schedPriority = 0;
        snprintf(config.name, 63, "CoreEvent.global.thread.%u", context->id);
        config.schedPriority = schedPriority;
        _CEThreadCreate(config, scheduler, CEGlobalQueueBeforeMainFunc, CEGlobalQueueMainFunc, NULL, NULL);
    } else if (CETaskSchedulerStatusRunning == newStatus) {
        //do wake up
        CESemSignal(scheduler->waiter);
    } else {
        abort();
    }
}


void CEGlobalQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific) {
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

void CEGlobalQueueMainFunc(void * _Nullable param) {
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    CEGlobalThreadTaskSchedulerContext_s * context = (CEGlobalThreadTaskSchedulerContext_s *)scheduler->context;
    
    uint_fast32_t status = atomic_load(&(context->status));
    assert(CETaskSchedulerStatusRunning == status);
    while (1) {
        switch (status) {
            case CETaskSchedulerStatusWaiting: {
                CEGlobalTaskSchedulerUnbindSource(scheduler);
                CEGlobalThreadManagerEnQueue(scheduler);
                
                CEGlobalTaskSchedulerWait(scheduler);
            }
                break;
            case CETaskSchedulerStatusRunning: {
                CETaskPtr task = CEGlobalTaskSchedulerRemoveTask(scheduler);
                if (task) {
                    //do task
                    
                    CETaskSchedulerExecuteTask(scheduler, task);
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


