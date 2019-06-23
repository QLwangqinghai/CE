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
#include "CETaskScheduler.h"
#include "CETask.h"


static _Atomic(uint_fast32_t) _CEQueueSequence;
void _CEQueueSequencerInitialize(void) {
    uint_fast32_t v = 1;
    atomic_init(&_CEQueueSequence, v);
}

uint32_t CEQueueSequenceNext(void) {
    uint_fast32_t a = 0;
    while (a <= 1) {
        a = atomic_fetch_add(&_CEQueueSequence, 1);
        if (a > UINT16_MAX) {
            abort();
        }
    }
    return (uint32_t)a;
}


const CETypeSpecific_s CETypeSpecificQueue = {
    .name = "CEQueue",
    .descript = CETypeDefaultDescript,
    .deinit = CERuntimeDefaultDeinit,
};

const CEType_s __CETypeQueue = CEType(CETypeBitHasRc | CETypeBitStatic, 0, (uintptr_t)(&__CETypeQueue), &CETypeSpecificQueue);


CETypeRef _Nonnull CETypeQueue = &__CETypeQueue;


void CEQueueSync(CEQueueRef _Nonnull queuePtr,
                 CEPtr _Nonnull object,
                 CETaskFinish_f _Nullable finish,
                 CEFunction_f _Nonnull execute,
                 CETaskParamRef _Nullable param,
                 CETaskParamRef _Nullable result,
                 uint32_t tag) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    _CEQueueJoin(queue, object, finish, execute, param, result, specific->syncWaiter, tag, false);
}

void CEQueueAsync(CEQueueRef _Nonnull queuePtr,
                  CEPtr _Nonnull object,
                  CETaskFinish_f _Nullable finish,
                  CEFunction_f _Nonnull execute,
                  CETaskParamRef _Nullable param,
                  uint32_t tag) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    _CEQueueJoin(queue, object, finish, execute, param, NULL, NULL, tag, false);
}


void CEConcurrentQueueBarrierSync(CEQueueRef _Nonnull queuePtr,
                                  CEPtr _Nonnull object,
                                  CETaskFinish_f _Nullable finish,
                                  CEFunction_f _Nonnull execute,
                                  CETaskParamRef _Nullable param,
                                  CETaskParamRef _Nullable result,
                                  uint32_t tag) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    assert(CEQueueTypeConcurrent == queue->type);
    
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    _CEQueueJoin(queue, object, finish, execute, param, result, specific->syncWaiter, tag, true);
}
void CEConcurrentQueueBarrierAsync(CEQueueRef _Nonnull queuePtr,
                                   CEPtr _Nonnull object,
                                   CETaskFinish_f _Nullable finish,
                                   CEFunction_f _Nonnull execute,
                                   CETaskParamRef _Nullable param,
                                   uint32_t tag) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    assert(CEQueueTypeConcurrent == queue->type);
    _CEQueueJoin(queue, object, finish, execute, param, NULL, NULL, tag, true);
}



CEQueue_s * _Nonnull CEQueueCreate(const char * _Nullable label, uint16_t concurrencyCount, CEQueuePriority_t priority, CEQueueType_t type, uint16_t id) {
    CEQueue_s * queue = CETypeQueue->alloctor->allocate(CETypeQueue, sizeof(CEQueue_s));;
    if (label) {
        snprintf(queue->label, 64, "%s", label);
    }
    queue->concurrencyCount = concurrencyCount;
    queue->priority = priority;
    queue->type = type;
    queue->id = id;
    return queue;
}

CEQueueRef _Nonnull CESerialQueueCreate(const char * _Nullable label, CEQueuePriority_t priority) {
    return _CESerialQueueCreate(label, priority);
}


