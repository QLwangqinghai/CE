//
//  CETimeEvent.c
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CETimeEvent.h"
#include "CEMemory.h"


CCRefType CERefTypeTimeEvent = UINTPTR_MAX;

void CETimeEventSourceDeinit(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll);
void CETimeEventSourceStop(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll);


static CEMicrosecondTime CEAddMicrosecondsToNow(CEMicrosecondTime milliseconds) {
    CEMicrosecondTime time = CEGetCurrentTime();
    return time + milliseconds;
}


void CETimeEventQueueInit(CETimeEventQueue_s * _Nonnull queue) {
    queue->count = 0;
    queue->capacity = 0;
    for (int i=0; i<CETimeEventQueuePagesSize; i++) {
        queue->pages[i] = 0;
    }
}
void CETimeEventQueueShiftDown(CETimeEventQueue_s * _Nonnull queue, uint32_t index);
void CETimeEventQueueShiftUp(CETimeEventQueue_s * _Nonnull queue, uint32_t index);


void CETimeEventQueueAddSize(CETimeEventQueue_s * _Nonnull queue) {
    queue->capacity += 0x1000;
    queue->pages[(queue->capacity >> 12)] = CEAllocateClear(sizeof(CETimeEventPage_s));
}

CETimeEventId _Nonnull CETimeEventQueueRemoveIndex(CETimeEventQueue_s * _Nonnull queue, uint32_t index) {
    assert(index < queue->count);
    uint32_t lastIndex = queue->count - 1;
    CETimeEventId result = CETimeEventQueueGetItem(queue, index);
    ((CETimeEvent_s *)CCRefGetContentPtr(result))->qIndex = CETimeEventQueueIndexInvalid;
    if (lastIndex == index) {
        queue->pages[(index >> 12)]->buffer[(index & 0xfff)] = NULL;
        queue->count -= 1;
    } else {
        queue->pages[(index >> 12)]->buffer[(index & 0xfff)] = queue->pages[(lastIndex >> 12)]->buffer[(lastIndex & 0xfff)];
        ((CETimeEvent_s *)CCRefGetContentPtr(CETimeEventQueueGetItem(queue, index)))->qIndex = index;
        queue->pages[(lastIndex >> 12)]->buffer[(lastIndex & 0xfff)] = NULL;
        CETimeEventQueueShiftDown(queue, index);
        queue->count -= 1;
    }
    return result;
}

CETimeEvent_s * _Nullable CETimeEventQueueRemoveFirst(CETimeEventQueue_s * _Nonnull queue) {
    return CETimeEventQueueRemoveIndex(queue, 0);
}

CCBool CETimeEventQueueRemove(CETimeEventQueue_s * _Nonnull queue, CETimeEventId _Nonnull event) {
    assert(queue);
    assert(event);
    uint32_t index = ((CETimeEvent_s *)CCRefGetContentPtr(event))->qIndex;
    if (index >= queue->count) {
        return false;
    }
    CETimeEventId t = CETimeEventQueueGetItem(queue, index);
    if (t != event) {
        return false;
    }
    CETimeEventQueueRemoveIndex(queue, index);
    return true;
}

CCBool CETimeEventQueueInsert(CETimeEventQueue_s * _Nonnull queue, CETimeEventId _Nonnull event) {
    if (queue->count == queue->capacity) {
        CETimeEventQueueAddSize(queue);
    }
    CETimeEvent_s * e = (CETimeEvent_s *)CCRefGetContentPtr(event);
    uint32_t currentIndex = e->qIndex;
    if (currentIndex != CETimeEventQueueIndexInvalid) {
        return false;
    }
    uint32_t index = queue->count;
    queue->count += 1;
    queue->pages[(index >> 12)]->buffer[(index & 0xfff)] = event;
    e->qIndex = index;
    CETimeEventQueueShiftUp(queue, index);
    return true;
}

static inline void CETimeEventQueueSwap(CETimeEventQueue_s * _Nonnull queue, uint32_t index, uint32_t aindex) {
    CETimeEventId t = queue->pages[(index >> 12)]->buffer[(index & 0xfff)];
    CETimeEventId at = queue->pages[(aindex >> 12)]->buffer[(aindex & 0xfff)];
    queue->pages[(index >> 12)]->buffer[(index & 0xfff)] = at;
    queue->pages[(aindex >> 12)]->buffer[(aindex & 0xfff)] = t;
    
    ((CETimeEvent_s *)CCRefGetContentPtr(t))->qIndex = aindex;
    ((CETimeEvent_s *)CCRefGetContentPtr(at))->qIndex = index;
}

static inline _Bool CETimeEventQueueNeedSwap(CETimeEventQueue_s * _Nonnull queue, uint32_t parentIndex, uint32_t childIndex) {
    
    CETimeEvent_s * parent = CCRefGetContentPtr(CETimeEventQueueGetItem(queue, parentIndex));
    CETimeEvent_s * child = CCRefGetContentPtr(CETimeEventQueueGetItem(queue, childIndex));
    if (parent->when == child->when) {
        return false;
    } else {
        return parent->when > child->when;
    }
}

void CETimeEventQueueShiftDown(CETimeEventQueue_s * _Nonnull queue, uint32_t index) {
    uint32_t n = queue->count - 1;
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

void CETimeEventQueueShiftUp(CETimeEventQueue_s * _Nonnull queue, uint32_t index) {
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


void _CETimeEventDeinit(CETimeEventId _Nonnull ref) {
    assert(ref);
    CETimeEvent_s * content = CCRefGetContentPtr(ref);
    assert(content->qIndex = CETimeEventQueueIndexInvalid);
    assert(content->closure);
    CCRefRelease(content->closure);
    content->closure = NULL;
}
