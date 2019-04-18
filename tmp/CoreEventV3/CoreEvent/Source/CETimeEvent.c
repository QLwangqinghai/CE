//
//  CETimeEvent.c
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CETimeEvent.h"
#include "CEMemory.h"

void CETimeEventSourceDeinit(CETimeEvent_s * _Nonnull te, CERunLoop_s * _Nonnull eventLoop);
void CETimeEventSourceStop(CETimeEvent_s * _Nonnull te, CERunLoop_s * _Nonnull eventLoop);

CEResult_t CERunLoopAddRepeatTimer(CERunLoop_s * _Nonnull eventLoop,
                                   CETimeDescription_s * _Nullable tdPtr,
                                   uint64_t delay,
                                   uint64_t leeway,
                                   _Bool repeatModeAfter,
                                   uint64_t interval,
                                   CETimeEventHandler_f _Nonnull execute,
                                   void * _Nullable context,
                                   CETimeEventClearContextHandler_f _Nullable clearContext) {
    return CERunLoopAddTimeEvent(eventLoop, tdPtr, delay, leeway, 1, repeatModeAfter, interval, execute, context, clearContext);
}

CEResult_t CERunLoopAddDelayTimer(CERunLoop_s * _Nonnull eventLoop,
                                  CETimeDescription_s * _Nullable tdPtr,
                                  uint64_t delay,
                                  uint64_t leeway,
                                  CETimeEventHandler_f _Nonnull execute,
                                  void * _Nullable context,
                                  CETimeEventClearContextHandler_f _Nullable clearContext) {
    return CERunLoopAddTimeEvent(eventLoop, tdPtr, delay, leeway, 0, 0, 0, execute, context, clearContext);
}


static uint64_t CEAddMicrosecondsToNow(uint64_t milliseconds) {
    uint64_t time = CEGetMicrosecondsTime();
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

void CETimeEventSourceDeinit(CETimeEvent_s * _Nonnull te, CERunLoop_s * _Nonnull eventLoop) {
    assert(!CETimeEventIsRunning(te));
    CETimeEventSourceStop(te, eventLoop);
//    static inline void CETimeEventTableRemoveItem(CETimeEventTable_s * _Nonnull table, CETimeEventContainer_s * _Nonnull item)
    CELogDebug("dealloc: id = %u, tag = %u",  te->tdId, te->tdTag);
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


static inline void CETimeEventSourceClearContext(CETimeEvent_s * _Nonnull te, CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td) {
    if (te->clearContext) {
        te->clearContext(eventLoop, td, te->context);
        te->clearContext = NULL;
        te->context = NULL;
    }
}


void CETimeEventSourceExecute(CETimeEvent_s * _Nonnull te, CERunLoop_s * _Nonnull eventLoop) {
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


void CETimeEventSourceStop(CETimeEvent_s * _Nonnull te, CERunLoop_s * _Nonnull eventLoop) {
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
    uint32_t flag = 0; //flag用来标记是否需要继续向下调整
    while (i * 2 + 1 <= n && flag == 0) {
        uint32_t leftChildIndex = i * 2 + 1;
        uint32_t rightChildIndex = leftChildIndex + 1;
        
        if (CETimeEventQueueNeedSwap(queue, i, leftChildIndex)) {
            t = leftChildIndex;
        } else {
            t = i;
        }
        
        //如果它有右儿子，再对右儿子进行讨论
        if (rightChildIndex <= n) {
            //如果它的右儿子的值更小，更新较小的结点编号
            if (CETimeEventQueueNeedSwap(queue, t, rightChildIndex)) {
                t = rightChildIndex;
            }
        }
        
        //如果发现最小的编号不是自己，说明子结点中有比父节点更小的
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
    int flag = 0; //用来标记是否需要继续向上调整
    //不在堆顶，并且当前结点i的值比父节点小的时候就继续向上调整
    while (i != 0 && flag == 0) {
        uint32_t parentIndex = (i - 1) / 2;
        //判断是否比父节点的小
        if (CETimeEventQueueNeedSwap(queue, parentIndex, i)) {
        
            printf("");
        }
        if (CETimeEventQueueNeedSwap(queue, parentIndex, i)) {
            CETimeEventQueueSwap(queue, i, parentIndex);
        } else{
            flag = 1;
        }
        i = parentIndex;
    }
}
