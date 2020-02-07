//
//  LSTimerQueue.c
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "LSTimerQueue.h"

CCRefType CERefTypeTimeEvent = UINTPTR_MAX;

static CEMicrosecondTime CEAddMicrosecondsToNow(CEMicrosecondTime milliseconds) {
    CEMicrosecondTime time = CEGetCurrentTime();
    return time + milliseconds;
}

void __LSTimerQueueInit(LSTimerQueue_s * _Nonnull queue, LSFileHandler_s * _Nonnull owner, LSTimerQueueGetSource_f _Nonnull getSource) {
    assert(queue);
    assert(owner);
    assert(getSource);
    queue->owner = owner;
    queue->getSource = getSource;
    for (uint32_t i=0; i<0x10000; i++) {
        queue->table[i] = i;
    }
}

void LSTimerQueueUpdateItem(LSTimerQueue_s * _Nonnull queue, uint32_t index, CEMicrosecondTime deadline) {
    uint64_t time = deadline >> 16;
    if (time != LSSocketSourceDeadlineForever) {
        if (0 != (deadline & 0xffffULL)) {
            time += 1;
        }
    }
    
    LSSocketSource_s * source = LSTimerQueueGetItem(queue, queue->table[index]);
    uint64_t old = source->deadline;
    source->deadline = time;
    if (time > old) {
        LSTimerQueueShiftDown(queue, index);
    } else {
        LSTimerQueueShiftUp(queue, index);
    }
}



static inline void LSTimerQueueSwap(LSTimerQueue_s * _Nonnull queue, uint32_t index, uint32_t aindex) {
    uint32_t id = queue->table[index];
    queue->table[index] = queue->table[aindex];
    queue->table[aindex] = id;
    LSTimerQueueGetItem(queue, queue->table[index])->index = index;
    LSTimerQueueGetItem(queue, queue->table[aindex])->index = aindex;
    queue->table[aindex] = index;
    queue->table[index] = aindex;
}

static inline CCBool LSTimerQueueNeedSwap(LSTimerQueue_s * _Nonnull queue, uint32_t parentIndex, uint32_t childIndex) {
    
    LSSocketSource_s * parent = LSTimerQueueGetItem(queue, queue->table[parentIndex]);
    LSSocketSource_s * child = LSTimerQueueGetItem(queue, queue->table[childIndex]);
    if (parent->deadline == child->deadline) {
        return false;
    } else {
        return parent->deadline > child->deadline;
    }
}

void LSTimerQueueShiftDown(LSTimerQueue_s * _Nonnull queue, uint32_t index) {
    uint32_t n = 0xffff;
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

void CETimeEventPerform(CETimeEventId _Nonnull ref) {
    assert(ref);
    CETimeEvent_s * content = CCRefGetContentPtr(ref);

    if (CETimeEventStatesWait != content->states) {
        CELogError("CETimeEventPerform error");
        abort();
    }
    content->states = CETimeEventStatesExecuting;
    CCClosurePerform(content->closure, ref, NULL);
    
    if (CETimeEventStatesExecuting == content->states) {
        if (CETimeEventModeRepeatFixedRate == content->mode) {
            content->when = content->when + content->duration;
            content->states = CETimeEventStatesWait;
        } else if (CETimeEventModeRepeatFixedDelay == content->mode) {
            content->when = CEGetCurrentTime() + content->duration;
            content->states = CETimeEventStatesWait;
        } else if (CETimeEventModeDelay == content->mode) {
            content->states = CETimeEventStatesFinished;
        } else {
            CELogError("CETimeEventPerform error");
            abort();
        }
    } else if (CETimeEventStatesWait == content->states) {
        CELogError("CETimeEventPerform error");
        abort();
    } else if (CETimeEventStatesFinished == content->states) {
        CELogError("CETimeEventPerform error");
        abort();
    } else {
        CELogError("CETimeEventPerform error");
        abort();
    }
}
