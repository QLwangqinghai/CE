//
//  CEConcurrentQueue.h
//  CoreEvent
//
//  Created by vector on 2019/6/10.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEConcurrentQueue_h
#define CEConcurrentQueue_h

#include "CEQueueInternal.h"

typedef struct _CESourceConcurrentContext {
    CESourceListStore_s tasks;
 
    CETaskSchedulerPtr _Nonnull schedulers[256];
    CETaskSchedulerPtr _Nonnull schedulersBuffer[256];
    
    uint32_t privateSchedulerCount: 16;
    uint32_t maxConcurrencyCount: 16;
    uint32_t currentConcurrencyCount: 16;
    uint32_t bufferCount: 16;
    uint32_t executingCount: 15;
    uint32_t isBarrier: 1;
} CESourceConcurrentContext_s;

#endif /* CEConcurrentQueue_h */