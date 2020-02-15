//
//  LSTimerQueue.c
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "LSTimerQueue.h"


static inline void __LSTimerQueueSetItemSourceIndex(LSTimerQueue_s * _Nonnull queue, uint32_t connectionIndex, uint32_t timerIndex) {
    queue->setSourceIndex(queue->group, connectionIndex, timerIndex);
}


//void __LSTimerQueueInit(LSTimerQueue_s * _Nonnull queue, LSConnectionGroup_s * _Nonnull group, LSTimerQueueSetSourceIndex_f _Nonnull setSourceIndex) {
//    assert(queue);
//    assert(group);
//    assert(setSourceIndex);
//    queue->group = group;
//    queue->setSourceIndex = setSourceIndex;
//    for (uint32_t i=0; i<0x10000; i++) {
//        queue->table[i] = i;
//    }
//}

void LSTimerQueueUpdateItem(LSTimerQueue_s * _Nonnull queue, uint32_t index, uint64_t deadline) {
    uint64_t time = deadline >> 13;
    if (time != LSSocketSourceDeadlineForever) {
        if (0 != (deadline & 0x1fffULL)) {
            time += 1;
        }
    }
    
    LSConnectionTimeSource_s * source = LSTimerQueueGetItem(queue, index);
    uint64_t old = source->deadline;
    source->deadline = time;
    if (time > old) {
        LSTimerQueueShiftDown(queue, index);
    } else {
        LSTimerQueueShiftUp(queue, index);
    }
}



static inline void LSTimerQueueSwap(LSTimerQueue_s * _Nonnull queue, uint32_t aIndex, uint32_t bIndex) {
    LSConnectionTimeSource_s * a = LSTimerQueueGetItem(queue, aIndex);
    LSConnectionTimeSource_s * b = LSTimerQueueGetItem(queue, bIndex);
    LSConnectionTimeSource_s tmp = *a;
    *a = *b;
    *b = tmp;
    
    __LSTimerQueueSetItemSourceIndex(queue, a->connectionIndex, aIndex);
    __LSTimerQueueSetItemSourceIndex(queue, b->connectionIndex, bIndex);
}

static inline CCBool LSTimerQueueNeedSwap(LSTimerQueue_s * _Nonnull queue, uint32_t parentIndex, uint32_t childIndex) {
    
    LSConnectionTimeSource_s * parent = LSTimerQueueGetItem(queue, parentIndex);
    LSConnectionTimeSource_s * child = LSTimerQueueGetItem(queue, childIndex);
    if (parent->deadline == child->deadline) {
        return false;
    } else {
        return parent->deadline > child->deadline;
    }
}

void LSTimerQueueShiftDown(LSTimerQueue_s * _Nonnull queue, uint32_t index) {
    uint32_t n = 0x1fff;
    uint32_t i = index;
    uint32_t t = 0;
    uint32_t flag = 0;
    while (i * 2 + 1 <= n && flag == 0) {
        uint32_t leftChildIndex = i * 2 + 1;
        uint32_t rightChildIndex = leftChildIndex + 1;
        
        if (LSTimerQueueNeedSwap(queue, i, leftChildIndex)) {
            t = leftChildIndex;
        } else {
            t = i;
        }
        
        if (rightChildIndex <= n) {
            if (LSTimerQueueNeedSwap(queue, t, rightChildIndex)) {
                t = rightChildIndex;
            }
        }
        if (t != i) {
            LSTimerQueueSwap(queue, t, i);
            i = t;
        } else {
            flag = 1;
        }
    }
}

void LSTimerQueueShiftUp(LSTimerQueue_s * _Nonnull queue, uint32_t index) {
    uint32_t i = index;
    int flag = 0;
    while (i != 0 && flag == 0) {
        uint32_t parentIndex = (i - 1) / 2;
        if (LSTimerQueueNeedSwap(queue, parentIndex, i)) {
            LSTimerQueueSwap(queue, i, parentIndex);
        } else{
            flag = 1;
        }
        i = parentIndex;
    }
}

