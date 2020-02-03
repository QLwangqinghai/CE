//
//  CETimeEvent.c
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CETimeEvent.h"
#include "CEMemory.h"

#include "CETimeEventTable.h"

void CETimeEventSourceDeinit(CETimeEvent_s * _Nonnull te, CEPool_s * _Nonnull eventLoop);
void CETimeEventSourceStop(CETimeEvent_s * _Nonnull te, CEPool_s * _Nonnull eventLoop);


static uint64_t CEAddMicrosecondsToNow(uint64_t milliseconds) {
    uint64_t time = CEGetMicrosecondsTime();
    return time + milliseconds;
}


static inline _Bool CETimeEventManagerAllocateKey(CETimeEventManager_s * _Nonnull manager, CETimeDescription_s * _Nonnull tdPtr) {
    uint32_t id = 0;
    if (!CEBitTable16AllocateKey(&(manager->table.keyTable), &id)) {
        return false;
    }
    if (id > CETimeEventIdMax) {
        return false;
    }
    manager->sequence += 1;
    tdPtr->id = id;
    tdPtr->tag = manager->sequence;
    return true;
}

static inline void CETimeEventManagerDeallocateKey(CETimeEventManager_s * _Nonnull manager, uint32_t key) {
    CEBitTable16DeallocateKey(&(manager->table.keyTable), key);
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
    if (!CETimeEventManagerAllocateKey(manager, &td)) {
        return NULL;
    }
    
    CETimeEvent_s * e = CEAllocate(sizeof(CETimeEvent_s));
    CETimeEventSetTd(e, td);
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
    
    assert(CETimeEventTableAddItem(&(manager->table), td.id, e));
    
    return e;
}

void CETimeEventSourceDeinit(CETimeEvent_s * _Nonnull te, CEPool_s * _Nonnull eventLoop) {
    assert(!CETimeEventIsRunning(te));
    CETimeEventSourceStop(te, eventLoop);
    CETimeEventManager_s * tem = &(eventLoop->timeEventManager);
    
    assert(te == CETimeEventTableRemoveItem(&(tem->table), te->tdId));
    CETimeEventManagerDeallocateKey(&(eventLoop->timeEventManager), te->tdId);
    CEDeallocate(te);
}


static inline void CETimeEventSourceClearContext(CETimeEvent_s * _Nonnull te, CEPool_s * _Nonnull eventLoop, CETimeDescription_s td) {
    if (te->clearContext) {
        te->clearContext(eventLoop, td, te->context);
        te->clearContext = NULL;
        te->context = NULL;
    }
}


void CETimeEventSourceExecute(CETimeEvent_s * _Nonnull te, CEPool_s * _Nonnull eventLoop) {
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


void CETimeEventSourceStop(CETimeEvent_s * _Nonnull te, CEPool_s * _Nonnull eventLoop) {
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
    queue->capacity = 4096;
    queue->buffer = CEAllocateClear(4096 * sizeof(CETimeEvent_s *));
}
void CETimeEventQueueShiftDown(CETimeEventQueue_s * _Nonnull queue, uint32_t index);
void CETimeEventQueueShiftUp(CETimeEventQueue_s * _Nonnull queue, uint32_t index);


void CETimeEventQueueAddSize(CETimeEventQueue_s * _Nonnull queue) {
    uint32_t oldSize = queue->capacity;
    uint32_t size = oldSize * 2;

    queue->buffer = CEReallocate(queue->buffer, size * sizeof(CETimeEvent_s *));
    
    for (uint32_t i=oldSize; i<size; i++) {
        queue->buffer[i] = NULL;
    }
    queue->capacity = size;
}

CETimeEvent_s * _Nullable CETimeEventQueueRemoveIndex(CETimeEventQueue_s * _Nonnull queue, uint32_t index) {
    if (index >= queue->count) {
        return NULL;
    }
    CETimeEvent_s * result = queue->buffer[index];
    result->qIndex = CETimeEventQueueIndexInvalid;
    queue->count -= 1;
    if (queue->count > 0) {
        queue->buffer[index] = queue->buffer[queue->count];
        queue->buffer[index]->qIndex = index;
        queue->buffer[queue->count] = NULL;
        CETimeEventQueueShiftDown(queue, index);
    } else {
        queue->buffer[index] = NULL;
    }
    return result;
}

CETimeEvent_s * _Nullable CETimeEventQueueRemoveFirst(CETimeEventQueue_s * _Nonnull queue) {
    return CETimeEventQueueRemoveIndex(queue, 0);
}

_Bool CETimeEventQueueRemove(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull event) {
    uint32_t index = event->qIndex;
    if (index >= queue->count) {
        return false;
    }
    CETimeEvent_s * t = queue->buffer[index];
    if (t != event) {
        return false;
    }
    CETimeEventQueueRemoveIndex(queue, index);
    return true;
}

void CETimeEventQueueInsert(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull event) {
    if (queue->count == queue->capacity) {
        CETimeEventQueueAddSize(queue);
    }
    uint32_t index = queue->count;
    queue->count += 1;
    queue->buffer[index] = event;
    event->qIndex = index;
    CETimeEventQueueShiftUp(queue, index);
}

static inline void CETimeEventQueueSwap(CETimeEventQueue_s * _Nonnull queue, uint32_t index, uint32_t aindex) {
    CETimeEvent_s * t = queue->buffer[index];
    CETimeEvent_s * at = queue->buffer[aindex];

    queue->buffer[index] = at;
    queue->buffer[aindex] = t;
    queue->buffer[index]->qIndex = index;
    queue->buffer[aindex]->qIndex = aindex;
}

static inline _Bool CETimeEventQueueNeedSwap(CETimeEventQueue_s * _Nonnull queue, uint32_t parentIndex, uint32_t childIndex) {
    CETimeEvent_s * parent = queue->buffer[parentIndex];
    CETimeEvent_s * child = queue->buffer[childIndex];
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
