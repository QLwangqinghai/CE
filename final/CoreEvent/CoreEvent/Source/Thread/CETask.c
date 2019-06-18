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
#include "CEQueueLog.h"

static _Atomic(uint_fast32_t) _CETaskTag;
void _CETaskTagInitialize(void) {
    uint_fast32_t v = 0;
    atomic_init(&_CETaskTag, v);
}

uint32_t CETaskTagNext(void) {
    uint_fast32_t a = 0;
    while (0 == a) {
        a = atomic_fetch_add(&_CETaskTag, 1);
    }
    return (uint32_t)a;
}

CETaskPtr _Nonnull CETaskCreate(CEPtr _Nonnull obj,
                                CETaskFinish_f _Nullable finish,
                                CEFunction_f _Nonnull execute,
                                CETaskParamRef _Nullable param,
                                CETaskParamRef _Nullable resultReceiver,
                                CEThreadSyncWaiter_s * _Nullable syncTaskWaiter,
                                uint32_t tag,
                                _Bool isBarrier) {
    assert(execute);
    assert(obj);

    if (param) {
        param = CERetain(param);
        assert(param);
    }
    if (resultReceiver) {
        resultReceiver = CERetain(resultReceiver);
        assert(resultReceiver);
    }
    
    CETaskPtr result = CEAllocateClear(sizeof(CETask_s));
    result->obj = obj;
    result->finish = finish;
    result->execute = execute;
    result->param = param;
    result->resultReceiver = resultReceiver;
    result->syncTaskWaiter = syncTaskWaiter;
    result->isBarrier = isBarrier ? 1 : 0;
    result->tag = tag;
    CEQueueLog("ce.task.create(%x)", tag);
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
    CEQueueLog("ce.task.destroy(%x)", task->tag);
    CEDeallocate(task);
}

