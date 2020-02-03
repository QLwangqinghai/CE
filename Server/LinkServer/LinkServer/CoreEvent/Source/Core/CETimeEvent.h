//
//  CETimeEvent.h
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CETimeEvent_h
#define CETimeEvent_h

#include "CEBase.h"

extern CCRefType CERefTypeTimeEvent;

void CETimeEventQueueInit(CETimeEventQueue_s * _Nonnull queue);
void CETimeEventQueueShiftDown(CETimeEventQueue_s * _Nonnull queue, uint32_t index);


static inline CETimeEventRef _Nonnull CETimeEventQueueGetItem(CETimeEventQueue_s * _Nonnull queue, uint32_t index) {
    assert(index < queue->count);
    return queue->pages[(index >> 12)]->buffer[(index & 0xfff)];
}

static inline CETimeEventRef _Nullable CETimeEventQueueGetFirstItem(CETimeEventQueue_s * _Nonnull queue) {
    if (queue->count <= 0) {
        return NULL;
    }
    return CETimeEventQueueGetItem(queue, 0);
}

CETimeEvent_s * _Nullable CETimeEventQueueRemoveFirst(CETimeEventQueue_s * _Nonnull queue);
CCBool CETimeEventQueueRemove(CETimeEventQueue_s * _Nonnull queue, CETimeEventRef _Nonnull event);
CCBool CETimeEventQueueInsert(CETimeEventQueue_s * _Nonnull queue, CETimeEventRef _Nonnull event);



void _CETimeEventDeinit(CETimeEventRef _Nonnull ref);




#endif /* CETimeEvent_h */
