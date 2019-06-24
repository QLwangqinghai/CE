//
//  CETimeEvent.c
//  CoreEvent
//
//  Created by vector on 2019/6/24.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CETimeEvent.h"


/* Time event structure */
typedef struct _CETimeEvent {
    uint64_t isStatic: 1;
    uint64_t repeat: 1;
    uint64_t repeatMode: 1;
    uint64_t states: 2;//必须为0
    uint64_t leeway: 17;//偏差 单位 微妙 最大值 0x1FFFF
    uint64_t interval: 42;//间隔时间 单位微妙  最大 1221小时 (50天)
//    CETimeEventHandler_f _Nonnull execute;
//    CETimeEventClearContextHandler_f _Nullable clearContext;
    void * _Nullable context;
    uint64_t when; /* microseconds 微妙 什么时候触发 */
    uint32_t tdTag;
    uint32_t tdId: 16;
    uint32_t qIndex: 16;
} CETimeEvent_s;

/* Time event structure */
typedef struct _CETimeEventContainer {
    CETimeEvent_s te;
    struct _CETimeEventContainer * _Nullable left;
    struct _CETimeEventContainer * _Nullable right;
} CETimeEventContainer_s;


//需要改成avl tree
typedef struct _CETimeEventTable {
    CETimeEventContainer_s * _Nullable buffer[4096];
} CETimeEventTable_s;

typedef struct _CETimeEventQueue {
    CETimeEvent_s * _Nullable * _Nonnull buffer;
    uint32_t capacity;
    uint32_t count;
} CETimeEventQueue_s;

typedef struct _CETimeEventQueue {
    CETimeEvent_s * _Nullable * _Nonnull buffer;
    uint32_t capacity;
    uint32_t count;
} CETimeEventQueue_s;

typedef struct _CETimeEventManager {
    CETimeEvent_s baseTable[4096];
    CETimeEventTable_s table;
    
    uint64_t size;
    uint64_t unused;
    uint32_t sequence;
    
    CETimeEvent_s * _Nullable executingTimeEvent;
    CETimeEventQueue_s timerQueue;
} CETimeEventManager_s;





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
