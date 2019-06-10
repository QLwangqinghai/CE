//
//  CETaskScheduler.c
//  CoreEvent
//
//  Created by vector on 2019/6/1.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CETaskScheduler.h"
#include "CEMemory.h"
#include "CESem.h"
#include "CEQueue.h"

void CETaskSchedulerExecuteTask(CETaskSchedulerPtr _Nonnull scheduler, CETaskPtr _Nonnull task) {
    assert(scheduler);
    assert(task);
    
    if (NULL != task->syncTaskWaiter) {
        CESemSignal(task->syncTaskWaiter);
    }
}

void CEGlobalQueueMainFunc(void * _Nullable param);
void CEGlobalQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific);


CETaskPtr _Nullable CEGlobalTaskSchedulerRemoveTask(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    
    CETaskPtr task = NULL;
    CESpinLockLock(&(scheduler->lock));
    if (NULL != scheduler->source) {
        task = CESourceRemove(scheduler->source);
    }
    CESpinLockUnlock(&(scheduler->lock));
    
    return task;
}

_Bool CEGlobalTaskSchedulerBindSource(CETaskSchedulerPtr _Nonnull scheduler, CESourceRef _Nonnull source) {
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
CESourceRef _Nullable CEGlobalTaskSchedulerUnbindSource(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    CESourceRef _Nonnull source = NULL;
    CESpinLockLock(&(scheduler->lock));
    source = scheduler->source;
    scheduler->source = NULL;
    CESpinLockUnlock(&(scheduler->lock));
    return source;
}


CETaskSchedulerPtr _Nonnull CEGlobalTaskSchedulerInit(uint32_t id) {
    CETaskSchedulerPtr scheduler = CEAllocateClear(sizeof(CETaskScheduler_s));
    
    CESpinLockInit(&(scheduler->lock));
    scheduler->waiter = CESemInit(0);
    CEGlobalThreadTaskSchedulerContext_s * context = (CEGlobalThreadTaskSchedulerContext_s *)scheduler->context;
    context->id = id;
    atomic_store(&(context->status), CETaskSchedulerStatusNoThread);
    return scheduler;
}

void CEGlobalTaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemDeinit(scheduler->waiter);
    CESpinLockDeinit(&(scheduler->lock));
}


void CEGlobalTaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemWait(scheduler->waiter);
}
void CEGlobalTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
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
    
    
    if (CETaskSchedulerStatusNoThread == newStatus) {
        CEThreadConfig_s config = {};
        float schedPriority = 0;
        CEQueue_s * queue = (CEQueue_s *)(scheduler->ownerQueue);
        _Bool hasName = false;
        if (queue) {
            if (queue->isSerialQueue) {
                memcpy(config.name, queue->label, 64);
                schedPriority = (float)(queue->schedPriority) / 128.0f - 1.0f;
                hasName = true;
            }
        }
        if (!hasName) {
            snprintf(config.name, 63, "CoreEvent.global.thread.%u", context->id);
        }
        config.schedPriority = schedPriority;
        _CEThreadCreate(config, scheduler, CEGlobalQueueBeforeMainFunc, CEGlobalQueueMainFunc, NULL, NULL);
    } else if (CETaskSchedulerStatusWaiting == newStatus) {
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






void CESerialTaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler);
void CESerialTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler);



void CESerialQueueMainFunc(void * _Nullable param);
void CESerialQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific);



CETaskPtr _Nullable CESerialTaskSchedulerRemoveTask(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    
    CETaskPtr task = NULL;
    CESpinLockLock(&(scheduler->lock));
    if (NULL != scheduler->source) {
        task = CESourceRemove(scheduler->source);
    }
    CESpinLockUnlock(&(scheduler->lock));
    
    return task;
}


CETaskSchedulerPtr _Nonnull CESerialTaskSchedulerInit(uint32_t id) {
    CETaskSchedulerPtr scheduler = CEAllocateClear(sizeof(CETaskScheduler_s));
    
    CESpinLockInit(&(scheduler->lock));
    scheduler->waiter = CESemInit(0);
    CEGlobalThreadTaskSchedulerContext_s * context = (CEGlobalThreadTaskSchedulerContext_s *)scheduler->context;
    context->id = id;
    atomic_store(&(context->status), CETaskSchedulerStatusNoThread);
    return scheduler;
}

void CESerialTaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemDeinit(scheduler->waiter);
    CESpinLockDeinit(&(scheduler->lock));
}


void CESerialTaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemWait(scheduler->waiter);
}
void CESerialTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
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
    
    
    if (CETaskSchedulerStatusNoThread == newStatus) {
        CEThreadConfig_s config = {};
        float schedPriority = 0;
        CEQueue_s * queue = (CEQueue_s *)(scheduler->ownerQueue);
        _Bool hasName = false;
        if (queue) {
            if (queue->isSerialQueue) {
                memcpy(config.name, queue->label, 64);
                schedPriority = (float)(queue->schedPriority) / 128.0f - 1.0f;
                hasName = true;
            }
        }
        if (!hasName) {
            snprintf(config.name, 63, "CoreEvent.global.thread.%u", context->id);
        }
        config.schedPriority = schedPriority;
        _CEThreadCreate(config, scheduler, CEGlobalQueueBeforeMainFunc, CEGlobalQueueMainFunc, NULL, NULL);
    } else if (CETaskSchedulerStatusWaiting == newStatus) {
        //do wake up
        CESemSignal(scheduler->waiter);
    } else {
        abort();
    }
}





void CESerialQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific) {
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    assert(scheduler->ownerQueue);
    specific->owner = scheduler->ownerQueue;
}


void CESerialQueueMainFunc(void * _Nullable param) {
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);

    CETaskPtr task = CESerialTaskSchedulerRemoveTask(scheduler);
    if (task) {
        //do task
        CETaskSchedulerExecuteTask(scheduler, task);

    } else {
        CESerialTaskSchedulerWait(scheduler);
    }
}


