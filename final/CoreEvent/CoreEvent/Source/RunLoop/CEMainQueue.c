//
//  CEMainQueue.c
//  CoreEvent
//
//  Created by vector on 2019/6/11.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEMainQueue.h"
#include "CEThread.h"
#include "CETaskScheduler.h"
#include "CESource.h"
#include "CEQueueInternal.h"

typedef struct _CESourceMainContext {
    CESourceCount_t count;
    CESourceListStore_s tasks;
    CESourceListStore_s highLevelTasks;
    CETaskSchedulerPtr _Nonnull scheduler;
} CESourceMainContext_s;


void CEMainTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    //do wake up
    CESemSignal(scheduler->waiter);
}

void CEMainSourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    CESourceCount_t count = 0;
    CESpinLockLock(source->lock);
    CESourceMainContext_s * context = source->context;
    CESourceTaskStoreAppend(&(context->tasks), task);
    context->count += 1;
    count = context->count;
    CESpinLockUnlock(source->lock);
    if (count == 1) {
        //weak up
        CEMainTaskSchedulerSignal(context->scheduler);
    }
}
CETaskPtr _Nonnull CESourceMainQueueRemove(CESourceRef _Nonnull source) {
    assert(source);
    CESourceMainContext_s * context = source->context;
    CETaskPtr result = NULL;
    
    while (NULL == result) {
        CESpinLockLock(source->lock);
        result = CESourceTaskStoreRemove(&(context->highLevelTasks));
        if (NULL == result) {
            result = CESourceTaskStoreRemove(&(context->tasks));
        }
        CESpinLockUnlock(source->lock);
        if (NULL == result) {
            CESemWait(context->scheduler->waiter);
        }
    }
    return result;
}
CETaskPtr _Nonnull CESourceMainQueueFinishOneTaskAndRemove(CESourceRef _Nonnull source) {
    CESourceCount_t count = 0;
    CETaskPtr result = NULL;
    CESourceMainContext_s * context = source->context;
    CESpinLockLock(source->lock);
    context->count -= 1;
    count = context->count;
    
    if (count == 0) {
        // wait
        CESpinLockUnlock(source->lock);
        CESemWait(context->scheduler->waiter);
        CESpinLockLock(source->lock);
    }
    
    result = CESourceTaskStoreRemove(&(context->highLevelTasks));
    if (NULL == result) {
        result = CESourceTaskStoreRemove(&(context->tasks));
    }
    CESpinLockUnlock(source->lock);
    assert(result);
    return result;
}

CETaskSchedulerPtr _Nonnull CEMainTaskSchedulerCreate(CEQueue_s * _Nonnull queue) {
    assert(queue);
    CETaskSchedulerPtr scheduler = CETaskSchedulerCreate(queue);
    scheduler->thread = (CEThread_s *)CEMainThreadShared();
    return scheduler;
}

void CEQueueMain(void) {
    assert(CEIsMainThread());
    
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    
    CETaskPtr task = CESourceMainQueueRemove(scheduler->source);
    while (1) {
        CETaskSchedulerExecuteTask(scheduler, task);
        task = CESourceMainQueueFinishOneTaskAndRemove(scheduler->source);
    }
}


//source
CESource_s * _Nonnull CEMainSourceCreate(CEQueue_s * _Nonnull queue) {
    assert(queue);
    CESourceMainContext_s * context = CEAllocateClear(sizeof(CESourceMainContext_s));
    context->scheduler = CEMainTaskSchedulerCreate(queue);
    CESource_s * source = CESourceCreate(queue, context, CEMainSourceAppend);
    context->scheduler->source = source;
    return source;
}

//queue
CEQueue_s * _Nonnull CEMainQueueCreate(void) {
    CEQueue_s * queue = CEQueueCreate("main", 1, UINT16_MAX, CEQueueTypeSerial);
    queue->source = CEMainSourceCreate(queue);
    return queue;
}


static CEQueue_s * __CEMainQueueShared = NULL;
void __CEMainQueueSharedOnceBlockFunc(void) {
    __CEMainQueueShared = CEMainQueueCreate();
}
CEQueue_s * _CEMainQueueShared(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CEMainQueueSharedOnceBlockFunc);
    return __CEMainQueueShared;
}
