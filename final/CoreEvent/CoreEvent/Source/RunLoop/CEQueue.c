//
//  CEQueue.c
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEQueueInternal.h"
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


void CEQueueSync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param, CEParamRef _Nullable result) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    _CEQueueJoin(queue, execute, param, result, specific->syncWaiter, false);
}

void CEQueueAsync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    _CEQueueJoin(queue, execute, param, NULL, NULL, false);
}


void CEConcurrentQueueBarrierSync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param, CEParamRef _Nullable result) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    assert(CEQueueTypeConcurrent == queue->type);
    
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    _CEQueueJoin(queue, execute, param, result, specific->syncWaiter, true);
}
void CEConcurrentQueueBarrierAsync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    assert(CEQueueTypeConcurrent == queue->type);
    _CEQueueJoin(queue, execute, param, NULL, NULL, true);
}





CEQueue_s * _Nonnull CEQueueCreate(char * _Nullable label, uint32_t concurrencyCount, CEQueuePriority_t priority, CEQueueType_t type) {
    CEQueue_s * queue = CETypeQueue->alloctor->allocate(CETypeQueue, sizeof(CEQueue_s));;
    if (label) {
        snprintf(queue->label, 64, "%s", label);
    }
    queue->concurrencyCount = concurrencyCount;
    queue->priority = priority;
    queue->type = type;
    
    return queue;
}
