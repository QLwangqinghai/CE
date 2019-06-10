//
//  CETask.c
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CETask.h"
#include "CEMemory.h"
#include "CEThreadBaseInternal.h"


CETaskPtr _Nonnull CETaskCreate(CEFunction_f _Nonnull execute, CEParamRef _Nullable param, CEParamRef _Nullable resultReceiver, CEThreadSyncWaiter_s * _Nullable syncTaskWaiter, _Bool isBarrier) {
    assert(execute);
    
    if (param) {
        CERetain(param);
    }
    if (resultReceiver) {
        CERetain(resultReceiver);
    }
    
    CETaskPtr result = CEAllocateClear(sizeof(CETask_s));
    result->execute = execute;
    result->param = param;
    result->resultReceiver = resultReceiver;
    result->syncTaskWaiter = syncTaskWaiter;
    result->isBarrier = isBarrier ? 1 : 0;
    
    return result;
}
void CETaskDestroy(CETaskPtr _Nonnull task) {
    assert(task);
    if (task->param) {
        CERelease(task->param);
    }
    if (task->resultReceiver) {
        CERelease(task->resultReceiver);
    }
    CEDeallocate(task);
}

void CETaskWaitFinish(CETaskPtr _Nonnull task) {
    
}
void CETaskFinish(CETaskPtr _Nonnull task) {
    
}
