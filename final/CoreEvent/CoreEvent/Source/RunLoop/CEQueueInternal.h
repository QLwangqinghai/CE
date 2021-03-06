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
#include "CESerialQueue.h"

static inline CEQueue_s * _Nonnull CEQueueCheck(CEQueueRef _Nonnull queuePtr) {
    assert(queuePtr);
    CETypeRef type = CERefGetType(queuePtr);
    assert(CETypeIsEqual(type, CETypeQueue));
    return (CEQueue_s *)queuePtr;
}

static inline void _CEQueueJoin(CEQueue_s * _Nonnull queue,
                                CEPtr _Nonnull object,
                                CETaskFinish_f _Nullable finish,
                                CEFunction_f _Nonnull execute,
                                CETaskParamRef _Nonnull param,
                                CETaskParamRef _Nullable result,
                                CEThreadSyncWaiter_s * _Nullable syncTaskWaiter,
                                uint32_t tag,
                                _Bool isBarrier) {
    if (0 == tag) {
        tag = CETaskTagNext();
    }
    CETaskPtr task = CETaskCreate(object, finish, execute, param, result, syncTaskWaiter, tag, isBarrier);
    CESourceAppend(queue->source, task);

    if (syncTaskWaiter) {
        CEThreadSyncWaiterWait(syncTaskWaiter);
    }
}


extern CEQueue_s * _Nonnull CEQueueCreate(const char * _Nullable label, uint16_t concurrencyCount, CEQueuePriority_t priority, CEQueueType_t type, uint16_t id);


uint32_t CEQueueSequenceNext(void);


#endif /* CEQueueInternal_h */
