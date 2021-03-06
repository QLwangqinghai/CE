//
//  CESerialQueue.h
//  CoreEvent
//
//  Created by vector on 2019/6/10.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CESerialQueue_h
#define CESerialQueue_h

#include "CEQueueInternal.h"

typedef struct _CESourceSerialContext {
    CESourceCount_t count;
    CESourceListStore_s tasks;
    CESourceListStore_s highLevelTasks;
    CETaskSchedulerPtr _Nonnull scheduler;
} CESourceSerialContext_s;

static inline CETaskPtr _Nullable _CESourceSerialContextRemove(CESourceSerialContext_s * _Nonnull context) {
    CETaskPtr result = CESourceTaskStoreRemove(&(context->highLevelTasks));
    if (NULL == result) {
        result = CESourceTaskStoreRemove(&(context->tasks));
    }
    return result;
}

CEQueue_s * _Nonnull _CESerialQueueCreate(const char * _Nullable label, CEQueuePriority_t priority);

#endif /* CESerialQueue_h */
