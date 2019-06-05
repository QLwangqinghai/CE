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


/*
 struct _CETaskScheduler {
 CEThreadRef _Nonnull thread;
 CEQueuePtr _Nullable ownerQueue;
 
 CESourceRef _Nonnull source;
 
 CESpinLock_t lock;
 
 uint32_t id;
 
 _Atomic(uint_fast32_t) status;
 
  CETaskWorkerRef _Nullable worker;
 
 CETaskSyncContextPtr _Nullable syncContext;
 CETaskStackPtr _Nonnull taskStack;
 
 
 CESemPtr _Nonnull waiter;
 
 uint32_t type;//main queue
 };
 */
CETaskSchedulerPtr _Nonnull CETaskSchedulerInit(void) {
    CETaskSchedulerPtr scheduler = CEAllocateClear(sizeof(CETaskScheduler_s));
    
    CESpinLockInit(&(scheduler->lock));
    
    scheduler->syncTaskWaiter = CESemInit(0);
    scheduler->waiter = CESemInit(0);
    
    return scheduler;
}

void CETaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemDeinit(scheduler->waiter);
    CESemDeinit(scheduler->syncTaskWaiter);
    CESpinLockDeinit(&(scheduler->lock));
}


void CETaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemWait(scheduler->waiter);
}
void CETaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
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
        _CEThreadCreate(config, scheduler, CEQueueBeforeMainFunc, CEQueueMainFunc, NULL, NULL);
    } else if (CETaskSchedulerStatusWaiting == newStatus) {
        //do wake up
        CESemSignal(scheduler->waiter);
    } else {
        abort();
    }
}

