//
//  CEQueueInternal.h
//  CoreEvent
//
//  Created by vector on 2019/6/11.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEQueueInternal_h
#define CEQueueInternal_h

#include "CEQueue.h"
#include "CETask.h"

static inline CEQueue_s * _Nonnull CEQueueCheck(CEQueueRef _Nonnull queuePtr) {
    assert(queuePtr);
    CETypeRef type = CERefGetType(queuePtr);
    assert(CETypeIsEqual(type, CETypeQueue));
    return (CEQueue_s *)queuePtr;
}

static inline void _CEQueueJoin(CEQueue_s * _Nonnull queue, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param, CEParamRef _Nullable result, CEThreadSyncWaiter_s * _Nullable syncTaskWaiter, _Bool isBarrier) {
    
    CETaskPtr task = CETaskCreate(execute, param, result, syncTaskWaiter, isBarrier);
    CESourceAppend(queue->source, task);

    if (syncTaskWaiter) {
        CEThreadSyncWaiterWait(syncTaskWaiter);
    }
}


extern CEQueue_s * _Nonnull CEQueueCreate(char * _Nullable label, uint32_t concurrencyCount, CEQueuePriority_t priority, CEQueueType_t type);




#endif /* CEQueueInternal_h */
