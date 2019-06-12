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

static const uint_fast32_t CETaskSchedulerConcurrentGlobalMask = 0x80000000uL;//1bit
static const uint_fast32_t CETaskSchedulerConcurrentGlobalPrivateMask = 0x40000000uL;//1bit
static const uint_fast32_t CETaskSchedulerConcurrentUsingMask = 0x30000000uL;//2bit
static const uint_fast32_t CETaskSchedulerConcurrentUsingMaskOffset = 28;

static const uint_fast32_t CETaskSchedulerConcurrentIndexMask = 0xFFFuL;//12bit
static const uint_fast32_t CETaskSchedulerConcurrentIndexMaskOffset = 0;


typedef struct _CETaskSchedulerConcurrentContextOwnerInfo {
    uint32_t isGlobal: 1;// isGlobal==0 时, 只有index 是有效的
    uint32_t isPrivate: 1;//isGlobal==1 时，isPrivate=0 Global共享的，isPrivate=1 Global独享
    uint32_t using: 2;//isPrivate=0 时有用 谁在使用 0 队列中, 1 high, 2 normal, 3 low
    uint32_t index: 12;//scheduler 在 CESourceConcurrentContext中的位置
} CETaskSchedulerConcurrentContextOwnerInfo_s;

static const uint16_t CETaskSchedulerConcurrentTypeNormal = 1;//普通并发队列
static const uint16_t CETaskSchedulerConcurrentTypeGlobalShared = 2;//共享的
static const uint16_t CETaskSchedulerConcurrentTypeGlobalExclusive = 3;//独享

typedef struct _CETaskSchedulerConcurrentContext {
    _Atomic(uint_fast32_t) status;
    _Atomic(uint_fast32_t) * _Nullable adjust;//全局共享的scheduler 属性有值
    _Atomic(uint_fast32_t) info;
} CETaskSchedulerConcurrentContext_s;


static const uint32_t CESourceConcurrentContextTypeGlobalHigh = 1;
static const uint32_t CESourceConcurrentContextTypeGlobalDefault = 2;
static const uint32_t CESourceConcurrentContextTypeGlobalLow = 3;
static const uint32_t CESourceConcurrentContextTypeCustom = 4;//普通并发队列


//Concurrent

typedef struct _CESourceConcurrentContext {
    CESourceListStore_s tasks;
 
    CETaskSchedulerPtr _Nonnull schedulers[256];
    CETaskSchedulerPtr _Nonnull schedulersBuffer[256];
    
    uint32_t type: 16;//4 普通并发队列, 1 global high, 2 global normal, 3 global low
    uint32_t privateSchedulerCount: 16;
    uint32_t maxConcurrencyCount: 16;
    uint32_t currentConcurrencyCount: 16;
    uint32_t bufferCount: 16;
    uint32_t executingCount: 15;
    uint32_t isBarrier: 1;
    
    _Atomic(uint_fast32_t) executingStatus;


    //    uint32_t concurrencyCount: 10;//队列并发数
    //    uint32_t activeCount: 10;//当前并发数
} CESourceConcurrentContext_s;


//void CEGlobalSourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
//    CESourceCount_t count = 0;
//    CESpinLockLock(source->lock);
//    CESourceConcurrentContext_s * context = source->context;
//    CESourceTaskStoreAppend(&(context->tasks), task);
//    context->count += 1;
//    count = context->count;
//    CESpinLockUnlock(source->lock);
//    if (count == 1) {
//        //weak up
//        CEGlobalTaskSchedulerSignal(context->scheduler);
//    }
//}
//



#endif /* CEConcurrentQueue_h */
