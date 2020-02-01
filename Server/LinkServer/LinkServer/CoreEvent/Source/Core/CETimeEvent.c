//
//  CETimeEvent.c
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CETimeEvent.h"
#include "CEMemory.h"

void CETimeEventSourceDeinit(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll);
void CETimeEventSourceStop(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll);


static CEMicrosecondTime CEAddMicrosecondsToNow(CEMicrosecondTime milliseconds) {
    CEMicrosecondTime time = CEGetCurrentTime();
    return time + milliseconds;
}

CETimeEvent_s * _Nullable CETimeEventSourceInit(CETimeEventManager_s * _Nonnull manager,
                                                uint64_t delay,
                                                uint64_t leeway,
                                                _Bool repeat,
                                                _Bool repeatModeAfter,
                                                uint64_t interval,
                                                CETimeEventHandler_f _Nonnull execute,
                                                void * _Nullable context,
                                                CETimeEventClearContextHandler_f _Nullable clearContext) {
    CETimeDescription_s td = {};
    if (!CETimeEventManagerMakeTd(manager, &td)) {
        return NULL;
    }
    
    CETimeEvent_s * e = NULL;
    e = CETimeEventManagerGetEventById(manager, td.id);
    
    if (td.id < 4096) {
#if DEBUG
        assert(NULL != e);
#endif
        e->isStatic = 1;
        CETimeEventSetTd(e, td);
    } else {
        CETimeEventContainer_s * c = CEAllocate(sizeof(CETimeEventContainer_s));
        c->left = NULL;
        c->right = NULL;
        e = &(c->te);
        e->isStatic = 0;
        CETimeEventSetTd(e, td);
        CETimeEventTableAddItem(&(manager->table), c);
    }
    e->repeat = repeat ? 1 : 0;
    e->repeatMode = repeatModeAfter ? 1 : 0;
    e->states = CETimeEventStatesWait;
    e->interval = interval;
    e->execute = execute;
    e->clearContext = clearContext;
    e->context = context;
    e->qIndex = CETimeEventQueueIndexInvalid;
    e->when = CEAddMicrosecondsToNow(delay);
    CETimeEventSetLeeway(e, leeway);
    
    return e;
}

void CETimeEventSourceDeinit(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll) {
    assert(!CETimeEventIsRunning(te));
    CETimeEventSourceStop(te, poll);
    CETimeEventManager_s * tem = &(eventLoop->timeEventManager);
    if (0 == te->isStatic) {
        assert(te->tdId >= 4096);
        CETimeEventContainer_s * c = CETimeEventTableGetItem(&(tem->table), te->tdId);
        assert(&(c->te) == te);
        CETimeEventTableRemoveItem(&(tem->table), c);
        CETimeEventManagerFreeId(&(eventLoop->timeEventManager), te->tdId);
        CEDeallocate(c);
    } else {
        assert(te->tdId < 4096);
        CETimeEventManagerFreeId(&(eventLoop->timeEventManager), te->tdId);
    }
}


static inline void CETimeEventSourceClearContext(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll, CETimeDescription_s td) {
    if (te->clearContext) {
        te->clearContext(eventLoop, td, te->context);
        te->clearContext = NULL;
        te->context = NULL;
    }
}


void CETimeEventSourceExecute(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll) {
    if (CETimeEventStatesWait != te->states) {
        return;
    }
    te->states = CETimeEventStatesExecuting;
    
    CETimeDescription_s td = CETimeEventGetTd(te);
    _Bool result = te->execute(eventLoop, td, te->context);
    if (CETimeEventStatesExecuting == te->states) {
        if (1 == te->repeat) {
            if (result) {
                if (te->repeatMode == CETimeEventRepeatModeAfter) {
                    te->when = eventLoop->microsecondsTime + te->interval;
                } else {
                    te->when += te->interval;
                }
                te->states = CETimeEventStatesWait;
            } else {
                te->states = CETimeEventStatesCancel;
                CETimeEventSourceClearContext(te, eventLoop, td);
            }
        } else {
            te->states = CETimeEventStatesFinished;
            CETimeEventSourceClearContext(te, eventLoop, td);
        }
    }
}


void CETimeEventSourceStop(CETimeEvent_s * _Nonnull te, CEPollPtr _Nonnull poll) {
    CETimeDescription_s td = CETimeEventGetTd(te);
    if (CETimeEventStatesExecuting == te->states) {
        if (1 == te->repeat) {
            te->states = CETimeEventStatesCancel;
        } else {
            te->states = CETimeEventStatesFinished;
        }
    } else {
        te->states = CETimeEventStatesCancel;
    }
    CETimeEventSourceClearContext(te, eventLoop, td);
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

CETimeEventRef _Nonnull CETimeEventQueueRemoveIndex(CETimeEventQueue_s * _Nonnull queue, uint32_t index) {
    assert(index < queue->count);
    uint32_t lastIndex = queue->count - 1;
    CETimeEventRef result = CETimeEventQueueGetItem(queue, index);
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

_Bool CETimeEventQueueRemove(CETimeEventQueue_s * _Nonnull queue, CETimeEventRef _Nonnull event) {
    assert(queue);
    assert(event);
    uint32_t index = ((CETimeEvent_s *)CCRefGetContentPtr(event))->qIndex;
    if (index >= queue->count) {
        return false;
    }
    CETimeEventRef t = CETimeEventQueueGetItem(queue, index);
    if (t != event) {
        return false;
    }
    CETimeEventQueueRemoveIndex(queue, index);
    return true;
}

_Bool CETimeEventQueueInsert(CETimeEventQueue_s * _Nonnull queue, CETimeEventRef _Nonnull event) {
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
}

static inline void CETimeEventQueueSwap(CETimeEventQueue_s * _Nonnull queue, uint32_t index, uint32_t aindex) {
    CETimeEventRef t = queue->pages[(index >> 12)]->buffer[(index & 0xfff)];
    CETimeEventRef at = queue->pages[(aindex >> 12)]->buffer[(aindex & 0xfff)];
    queue->pages[(index >> 12)]->buffer[(index & 0xfff)] = at;
    queue->pages[(aindex >> 12)]->buffer[(aindex & 0xfff)] = t;
    
    ((CETimeEvent_s *)CCRefGetContentPtr(t))->qIndex = aindex;
    ((CETimeEvent_s *)CCRefGetContentPtr(at))->qIndex = index;
}

static inline _Bool CETimeEventQueueNeedSwap(CETimeEventQueue_s * _Nonnull queue, uint32_t parentIndex, uint32_t childIndex) {
    
    CETimeEvent_s * parent = CCRefGetContentPtr(CETimeEventQueueGetItem(queue, parentIndex));
    CETimeEvent_s * child = CCRefGetContentPtr(CETimeEventQueueGetItem(queue, childIndex));
    if (parent->when == child->when) {
        return parent->leeway > child->leeway;
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
