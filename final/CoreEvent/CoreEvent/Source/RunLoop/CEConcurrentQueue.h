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
    
    uint32_t maxConcurrencyCount: 16;
    uint32_t bufferCount: 16;
    uint32_t executingBarrier: 1;
    uint32_t headIsBarrier: 1;

    uint32_t xxx: 30;
    uint32_t barrierTaskTag;
} CESourceConcurrentContext_s;

static inline _Bool CESourceConcurrentContextIsBarrier(CESourceConcurrentContext_s * _Nonnull context) {
    return (0 != context->executingBarrier) || (0 != context->headIsBarrier);
}



#endif /* CEConcurrentQueue_h */
