//
//  CETimeEvent.c
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEBase.h"
#include "CEMemory.h"


CCRefType CERefTypeTimeEvent = UINTPTR_MAX;

void CETimeEventSourceDeinit(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll);
void CETimeEventSourceStop(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll);


static CEMicrosecondTime CEAddMicrosecondsToNow(CEMicrosecondTime milliseconds) {
    CEMicrosecondTime time = CEGetCurrentTime();
    return time + milliseconds;
}


CETimerQueue_s * _Nonnull CETimeEventQueueCreate(void) {
    CETimerQueue_s * result = CCAlignedAllocate(sizeof(CETimerQueue_s), 128);
    result->count = 0;
    return result;
}
void CETimeEventQueueShiftDown(CETimerQueue_s * _Nonnull queue, uint32_t index);
void CETimeEventQueueShiftUp(CETimerQueue_s * _Nonnull queue, uint32_t index);


void _CETimerQueueUpdate(CETimerQueue_s * _Nonnull queue, CETimeEventRef _Nonnull ref, CCMicrosecondTime time) {
    
    
}


CETimeEventRef _Nonnull _CETimerQueueRemoveIndex(CETimerQueue_s * _Nonnull queue, uint32_t index) {
    assert(index < queue->count);
    uint32_t lastIndex = (uint32_t)(queue->count - 1);
    CETimeEventRef result = queue->buffer[index];
    CETimeEvent_s * event = (CETimeEvent_s *)CCRefGetContentPtr(result);
    event->qIndex = CETimeEventQueueIndexInvalid;
    if (lastIndex == index) {
        queue->buffer[index] = NULL;
        queue->count -= 1;
    } else {
        CETimeEventRef last = queue->buffer[lastIndex];
        queue->buffer[index] = last;
        CETimeEvent_s * lastEvent = (CETimeEvent_s *)CCRefGetContentPtr(last);
        lastEvent->qIndex = index;
        queue->buffer[lastIndex] = NULL;
        if (lastEvent->when > event->when) {
            CETimeEventQueueShiftDown(queue, index);
        } else if (lastEvent->when < event->when) {
            if (index > 0) {
                CETimeEventQueueShiftUp(queue, index);
            }
        }
        queue->count -= 1;
    }
    return result;
}

CETimeEventRef _Nullable _CETimerQueueRemoveFirst(CETimerQueue_s * _Nonnull queue) {
    if (queue->count <= 0) {
        return NULL;
    }
    return _CETimerQueueRemoveIndex(queue, 0);
}


CCBool _CETimerQueueRemove(CETimerQueue_s * _Nonnull queue, CETimeEventRef _Nonnull event) {
    assert(queue);
    assert(event);
    uint32_t index = ((CETimeEvent_s *)CCRefGetContentPtr(event))->qIndex;
    if (index >= queue->count) {
        return false;
    }
    CETimeEventRef t = queue->buffer[index];
    if (t != event) {
        return false;
    }
    CETimeEventRef removed = _CETimerQueueRemoveIndex(queue, index);
    assert(event == removed);
    return true;
}

CCBool _CETimerQueueInsert(CETimerQueue_s * _Nonnull queue, CETimeEventRef _Nonnull event) {
    if (queue->count >= CETimerQueueBufferSize) {
        return false;
    }
    CETimeEvent_s * e = (CETimeEvent_s *)CCRefGetContentPtr(event);
    uint32_t currentIndex = e->qIndex;
    if (currentIndex != CETimeEventQueueIndexInvalid) {
        return false;
    }
    uint32_t index = (uint32_t)(queue->count);
    queue->count += 1;
    queue->buffer[index] = event;
    e->qIndex = index;
    CETimeEventQueueShiftUp(queue, index);
    return true;
}

static inline void CETimeEventQueueSwap(CETimerQueue_s * _Nonnull queue, uint32_t index, uint32_t aindex) {
    CETimeEventRef t = queue->buffer[index];
    CETimeEventRef at = queue->buffer[aindex];
    queue->buffer[index] = at;
    queue->buffer[aindex] = t;
    ((CETimeEvent_s *)CCRefGetContentPtr(t))->qIndex = aindex;
    ((CETimeEvent_s *)CCRefGetContentPtr(at))->qIndex = index;
}

static inline _Bool CETimeEventQueueNeedSwap(CETimerQueue_s * _Nonnull queue, uint32_t parentIndex, uint32_t childIndex) {
    CETimeEvent_s * parent = CCRefGetContentPtr(queue->buffer[parentIndex]);
    CETimeEvent_s * child = CCRefGetContentPtr(queue->buffer[childIndex]);
    if (parent->when == child->when) {
        return false;
    } else {
        return parent->when > child->when;
    }
}

void CETimeEventQueueShiftDown(CETimerQueue_s * _Nonnull queue, uint32_t index) {
    uint32_t n = (uint32_t)(queue->count - 1);
    uint32_t i = index;
    uint32_t t = 0;
    uint32_t flag = 0;
    while (i * 2 + 1 <= n && flag == 0) {
        uint32_t leftChildIndex = i * 2 + 1;
        uint32_t rightChildIndex = leftChildIndex + 1;
        
        if (CETimeEventQueueNeedSwap(queue, i, leftChildIndex)) {
            t = leftChildIndex;
        } else {
            t = i;
        }
        
        if (rightChildIndex <= n) {
            if (CETimeEventQueueNeedSwap(queue, t, rightChildIndex)) {
                t = rightChildIndex;
            }
        }
        if (t != i) {
            CETimeEventQueueSwap(queue, t, i);
            i = t;
        } else {
            flag = 1;
        }
    }
}

void CETimeEventQueueShiftUp(CETimerQueue_s * _Nonnull queue, uint32_t index) {
    uint32_t i = index;
    int flag = 0;
    while (i != 0 && flag == 0) {
        uint32_t parentIndex = (i - 1) / 2;
        if (CETimeEventQueueNeedSwap(queue, parentIndex, i)) {
            CETimeEventQueueSwap(queue, i, parentIndex);
        } else{
            flag = 1;
        }
        i = parentIndex;
    }
}

void _CETimeEventPerform(CETimeEventRef _Nonnull ref) {
    assert(ref);
    CETimeEvent_s * content = CCRefGetContentPtr(ref);
    CCClosurePerform(content->closure, ref, NULL);
}


void _CETimeEventDeinit(CETimeEventRef _Nonnull ref) {
    assert(ref);
    CETimeEvent_s * content = CCRefGetContentPtr(ref);
    assert(content->qIndex = CETimeEventQueueIndexInvalid);
    assert(content->closure);
    CCRefRelease(content->closure);
    content->closure = NULL;
}
