//
//  LSTimerQueue.h
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef LSTimerQueue_h
#define LSTimerQueue_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "LSTypes.h"

#define LSTimerQueue


void LSTimerQueueShiftDown(LSTimerQueue_s * _Nonnull queue, uint32_t index);
void LSTimerQueueShiftUp(LSTimerQueue_s * _Nonnull queue, uint32_t index);

static inline LSConnectionTimeSource_s * _Nonnull LSTimerQueueGetItem(LSTimerQueue_s * _Nonnull queue, uint32_t index) {
    return queue->sources + index;
}

static inline LSConnectionTimeSource_s * _Nonnull LSTimerQueueGetFirstItem(LSTimerQueue_s * _Nonnull queue) {
    return queue->sources;
}

//index 必须是timer的index
void LSTimerQueueUpdateItem(LSTimerQueue_s * _Nonnull queue, uint32_t index, uint64_t deadline);


#if defined(__cplusplus)
}
#endif

#endif /* LSTimerQueue_h */
