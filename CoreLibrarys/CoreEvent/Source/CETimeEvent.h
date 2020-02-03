//
//  CETimeEvent.h
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CETimeEvent_h
#define CETimeEvent_h

#include <CoreEvent/CEBaseInternal.h>


void CETimeEventQueueInit(CETimeEventQueue_s * _Nonnull queue);
void CETimeEventQueueShiftDown(CETimeEventQueue_s * _Nonnull queue, uint32_t index);


static inline CETimeEventRef _Nullable CETimeEventQueueGetFirst(CETimeEventQueue_s * _Nonnull queue) {
    if (queue->count == 0) {
        return NULL;
    }
    return queue->buffer[0];
}

CETimeEventRef _Nullable CETimeEventQueueRemoveFirst(CETimeEventQueue_s * _Nonnull queue);
_Bool CETimeEventQueueRemove(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull event);
void CETimeEventQueueInsert(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull event);



CETimeEventRef _Nullable CETimeEventSourceInit(CETimeEventManager_s * _Nonnull manager,
                                               uint64_t delay,
                                               uint64_t leeway,
                                               _Bool repeat,
                                               _Bool repeatModeAfter,
                                               uint64_t interval,
                                               CETimeEventHandler_f _Nonnull execute,
                                               void * _Nullable context,
                                               CETimeEventClearContextHandler_f _Nullable clearContext);
void CETimeEventSourceExecute(CETimeEventRef _Nonnull te, CEPoolRef _Nonnull pool);
void CETimeEventSourceDeinit(CETimeEventRef _Nonnull te, CEPoolRef _Nonnull pool);
void CETimeEventSourceStop(CETimeEventRef _Nonnull te, CEPoolRef _Nonnull pool);


#endif /* CETimeEvent_h */
