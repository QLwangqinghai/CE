//
//  CEQueue.c
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEQueue.h"
#include "CEThread.h"
#include "CEThreadBaseInternal.h"
#include "CETaskContext.h"
#include "CERuntime.h"
#include "CEConditionLock.h"
#include "CESem.h"
#include "CETaskScheduler.h"

const CETypeSpecific_s CETypeSpecificQueue = {
    .name = "CEQueue",
    .descript = CETypeDefaultDescript,
    .deinit = CERuntimeDefaultDeinit,
};

const CEType_s __CETypeQueue = CEType(CETypeBitHasRc | CETypeBitStatic, 0, (uintptr_t)(&__CETypeQueue), &CETypeSpecificQueue);


CETypeRef _Nonnull CETypeQueue = &__CETypeQueue;

static inline CEQueue_s * _Nonnull CEQueueCheck(CEQueuePtr _Nonnull queuePtr) {
    assert(queuePtr);
    CETypeRef type = CERefGetType(queuePtr);
    assert(CETypeIsEqual(type, CETypeQueue));
    return (CEQueue_s *)queuePtr;
}

//void CEQueueSerialSync(CEQueuePtr _Nonnull queue, CESyncTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {
//}
//void CEQueueConcurrentSync(CEQueuePtr _Nonnull queue, CESyncTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {
//}

void CEQueueSync(CEQueuePtr _Nonnull queuePtr, CEParamRef _Nonnull param, CEParamRef _Nullable result, CEFunction_f _Nonnull execute) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    
    if (param) {
        CERetain(param);
    }
    if (result) {
        CERetain(result);
    }
    
    CEThreadSpecificRef specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    //获取sync任务的上下文 获取不到， 则创建
//    CETaskSyncContextPtr syncContext = specific->syncContext;
    
    CETaskContext_s context = CETaskContexPush();

    CETaskPtr task = NULL;
    task->isSyncTask = 1;
    task->syncTaskWaiter = specific->syncWaiter;
    CESourceAppend(queue->source, task);//转移
    CESemWait(specific->syncWaiter);
}




void CEGlobalThreadManagerDispatch(CESourceRef _Nonnull source) {
    CETaskSchedulerPtr scheduler = CEGlobalThreadManagerDeQueue();
    if (scheduler) {
        assert(CETaskSchedulerBindSource(scheduler, source));
        CETaskSchedulerSignal(scheduler);
    }
}
