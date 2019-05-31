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



void CEQueueSerialSync(CEQueuePtr _Nonnull queue, CENoescapingTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {
    
    
    
    
}
void CEQueueConcurrentSync(CEQueuePtr _Nonnull queue, CENoescapingTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {

    
    
    
}

void CEQueueSync(CEQueuePtr _Nonnull queuePtr, CEParamRef _Nonnull param, CEParamRef _Nullable result, CEFunction_f _Nonnull execute) {
    CEQueue_s * queue = CEQueueCheck(queuePtr);
    
    if (param) {
        CERetain(param);
    }
    if (result) {
        CERetain(result);
    }
    
    
    
    CEThreadSpecificRef specific = CEThreadSpecificGetCurrent();

    //获取sync任务的上下文 获取不到， 则创建
    CETaskSyncContextPtr syncContext = specific->syncContext;

    CETaskContext_s context = CETaskContexPush();
    
    if (queue->isSerialQueue == 0) {
        //并发队列
        
        
        
        
        
    } else {
        //串行队列
        
        
        
    }
    
//    if (queue->base.maxConcurrentCount <= 1) {
//        CEQueueSerialSync(queue, task, specific);
//    } else {
//        CEQueueConcurrentSync(queue, task, specific);
//    }
}
