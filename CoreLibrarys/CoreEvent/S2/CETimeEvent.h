//
//  CETimeEvent.h
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CETimeEvent_h
#define CETimeEvent_h

#include <CoreEvent/CEBase.h>


void CETimeEventQueueInit(CETimeEventQueue_s * _Nonnull queue);
void CETimeEventQueueShiftDown(CETimeEventQueue_s * _Nonnull queue, uint32_t index);


static inline CETimeEvent_s * _Nullable CETimeEventQueueGetFirst(CETimeEventQueue_s * _Nonnull queue) {
    if (queue->count == 0) {
        return NULL;
    }
    return queue->buffer[0];
}

CETimeEvent_s * _Nullable CETimeEventQueueRemoveFirst(CETimeEventQueue_s * _Nonnull queue);
_Bool CETimeEventQueueRemove(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull event);
void CETimeEventQueueInsert(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull event);


#endif /* CETimeEvent_h */
