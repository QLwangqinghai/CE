//
//  CETimeEvent.c
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CETimeEvent.h"
#include "CEMemory.h"


void CETimeEventStop(CETimeEvent_s * _Nonnull te);



uintptr_t CETimeEventInit(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, _Bool repeat, _Bool repeatModeAfter, uint64_t interval, CETimeEventHandler_f _Nonnull execute, void * _Nullable context, CETimeEventClearContextHandler_f _Nullable dealloc, CEResult_t * _Nonnull result) {
    
    assert(result);
    if (interval > CETimeEventIntervalMax || NULL == execute || NULL == eventLoop) {
        *result = CEResultErrorParams;
        return (uintptr_t)(NULL);
    }
    
    if (leeway > CETimeEventLeewayMax) {
        leeway = CETimeEventLeewayMax;
    } else if (leeway < 1) {
        leeway = 1;
    }
    
    CETimeEvent_s * e = (CETimeEvent_s *)CEAllocate(sizeof(CETimeEvent_s));
    e->repeat = repeat ? 1 : 0;
    e->repeatMode = repeatModeAfter ? 1 : 0;
    e->states = CETimeEventStatesWait;
    e->leeway = leeway;
    e->running = 0;
    e->interval = interval;
    e->execute = execute;
    e->clearContext = dealloc;
    e->context = context;
    e->when = 0;
    e->owner = (uintptr_t)eventLoop;
    e->prev = NULL;
    e->next = NULL;

    *result = CEResultSuccess;
    return (uintptr_t)(e);
}

uintptr_t CETimeEventCreateRepeatTimer(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, _Bool repeatModeAfter, uint64_t interval, CETimeEventHandler_f _Nonnull execute, void * _Nullable context, CETimeEventClearContextHandler_f _Nullable dealloc, CEResult_t * _Nonnull result) {
    return CETimeEventInit(eventLoop, leeway, 1, repeatModeAfter, interval, execute, context, dealloc, result);

}

uintptr_t CETimeEventCreateAfterTimer(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, CETimeEventHandler_f _Nonnull execute, void * _Nullable context, CETimeEventClearContextHandler_f _Nullable dealloc, CEResult_t * _Nonnull result) {
    return CETimeEventInit(eventLoop, leeway, 0, 0, 0, execute, context, dealloc, result);
}

CEResult_t CETimeEventDeinit(uintptr_t event) {
    CETimeEvent_s * te = (CETimeEvent_s *)event;
    if (NULL == te) {
        return CEResultErrorParams;
    }
    if (1 == te->running) {
        return CEResultErrorShouldRemoveSource;
    }
    CETimeEventStop(te);
    CEDeallocate(te);

    return CEResultSuccess;
}


static inline void CETimeEventClearContext(CETimeEvent_s * _Nonnull te) {
    if (te->clearContext) {
        te->clearContext((CERunLoop_s *)te->owner, (uintptr_t)te, te->context);
        te->clearContext = NULL;
        te->context = NULL;
    }
}


_Bool CETimeEventExecute(uintptr_t event) {
    CETimeEvent_s * te = (CETimeEvent_s *)event;
    if (NULL == te) {
        return 0;
    }
    if (CETimeEventStatesWait != te->states) {
        return 0;
    }
    
    te->states = CETimeEventStatesExecuting;
    _Bool result = te->execute((CERunLoop_s *)te->owner, (uintptr_t)te, te->context);
    if (CETimeEventStatesExecuting == te->states) {
        if (1 == te->repeat) {
            if (result) {
                te->states = CETimeEventStatesWait;
            } else {
                te->states = CETimeEventStatesCancel;
                CETimeEventClearContext(te);
            }
        } else {
            te->states = CETimeEventStatesFinished;
            CETimeEventClearContext(te);
        }
    }
    
    return 1;
}


void CETimeEventStop(CETimeEvent_s * _Nonnull te) {
    if (CETimeEventStatesExecuting == te->states) {
        if (1 == te->repeat) {
            te->states = CETimeEventStatesCancel;
        } else {
            te->states = CETimeEventStatesFinished;
        }
    } else {
        te->states = CETimeEventStatesCancel;
    }
    CETimeEventClearContext(te);
}









