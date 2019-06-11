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

//scheduler

void CESerialQueueMainFunc(void * _Nullable param);
void CESerialQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific);

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
        CETaskSchedulerSignal(context->scheduler);
    }
}

void CESourceSerialQueueFinishOneTask(CESourceRef _Nonnull source) {
    CESourceCount_t count = 0;
    
    CESourceSerialContext_s * context = source->context;
    CESpinLockLock(source->lock);
    context->count -= 1;
    count = context->count;
    CESpinLockUnlock(source->lock);

    if (count == 0) {
        // wait
        
        CESemWait(context->scheduler->waiter);
    }
}

CETaskPtr _Nonnull CESourceSerialQueueRemove(CESourceRef _Nonnull source) {
    assert(source);
    
    CETaskPtr result = NULL;
    CESpinLockLock(source->lock);
    CESourceSerialContext_s * context = source->context;
    result = CESourceTaskStoreRemove(&(context->tasks));
    CESpinLockUnlock(source->lock);
    assert(result);
    return result;
}
void CESerialTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    //do wake up
    CESemSignal(scheduler->waiter);
}

CETaskSchedulerPtr _Nonnull CESerialTaskSchedulerCreate(CEQueue_s * _Nonnull queue) {
    assert(queue);
    CETaskSchedulerPtr scheduler = CETaskSchedulerCreate(queue, CESerialTaskSchedulerSignal);

    
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
    
    while (1) {
        CETaskPtr task = CESourceSerialQueueRemove(scheduler->source);
        CETaskSchedulerExecuteTask(scheduler, task);
        CESourceSerialQueueFinishOneTask(scheduler->source);
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
