//
//  CESerialQueue.c
//  CoreEvent
//
//  Created by vector on 2019/6/10.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CESerialQueue.h"
#include "CETaskScheduler.h"
#include "CESource.h"
#include "CEQueueInternal.h"

typedef struct _CESourceSerialContext {
    CESourceCount_t count;
    CESourceListStore_s tasks;
    CETaskSchedulerPtr _Nonnull scheduler;
} CESourceSerialContext_s;

//scheduler

void CESerialQueueMainFunc(void * _Nullable param);
void CESerialQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific);

void CESerialTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    //do wake up
    CESemSignal(scheduler->waiter);
}

void CESerialSourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    CESourceCount_t count = 0;
    CESpinLockLock(source->lock);
    CESourceSerialContext_s * context = source->context;
    CESourceTaskStoreAppend(&(context->tasks), task);
    context->count += 1;
    count = context->count;
    CESpinLockUnlock(source->lock);
    if (count == 1) {
        //weak up
        CESerialTaskSchedulerSignal(context->scheduler);
    }
}

CETaskPtr _Nonnull CESourceSerialQueueFinishOneTaskAndRemove(CESourceRef _Nonnull source) {
    CESourceCount_t count = 0;
    CETaskPtr result = NULL;
    CESourceSerialContext_s * context = source->context;
    CESpinLockLock(source->lock);
    context->count -= 1;
    count = context->count;

    if (count == 0) {
        // wait
        CESpinLockUnlock(source->lock);
        CESemWait(context->scheduler->waiter);
        CESpinLockLock(source->lock);
    }
    
    result = CESourceTaskStoreRemove(&(context->tasks));
    CESpinLockUnlock(source->lock);
    assert(result);
    return result;
}

CETaskPtr _Nonnull CESourceSerialQueueRemove(CESourceRef _Nonnull source) {
    assert(source);
    CESourceSerialContext_s * context = source->context;
    CESemWait(context->scheduler->waiter);
    
    CETaskPtr result = NULL;
    CESpinLockLock(source->lock);
    result = CESourceTaskStoreRemove(&(context->tasks));
    CESpinLockUnlock(source->lock);
    assert(result);
    return result;
}


CETaskSchedulerPtr _Nonnull CESerialTaskSchedulerCreate(CEQueue_s * _Nonnull queue) {
    assert(queue);
    CETaskSchedulerPtr scheduler = CETaskSchedulerCreate(queue);

    CEThreadConfig_s config = {};
    float schedPriority = 0;
    memcpy(config.name, queue->label, 64);
    schedPriority = CEQueuePriorityToThreadPriority(queue->priority);
    config.schedPriority = schedPriority;
    _CEThreadCreate(config, scheduler, CESerialQueueBeforeMainFunc, CESerialQueueMainFunc, NULL, NULL);
    
    
    return scheduler;
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

    CETaskPtr task = CESourceSerialQueueRemove(scheduler->source);
    while (1) {
        CETaskSchedulerExecuteTask(scheduler, task);
        task = CESourceSerialQueueFinishOneTaskAndRemove(scheduler->source);
    }
}


//source
CESource_s * _Nonnull CESerialSourceCreate(CEQueue_s * _Nonnull queue) {
    assert(queue);
    CESourceSerialContext_s * context = CEAllocateClear(sizeof(CESourceSerialContext_s));
    context->scheduler = CESerialTaskSchedulerCreate(queue);
    CESource_s * source = CESourceCreate(queue, context, CESerialSourceAppend);
    return source;
}

//queue
CEQueue_s * _Nonnull CESerialQueueCreate(char * _Nullable label, CEQueuePriority_t priority) {
    CEQueue_s * queue = CEQueueCreate(label, 1, priority, CEQueueTypeSerial);
    queue->source = CESerialSourceCreate(queue);
    return queue;
}