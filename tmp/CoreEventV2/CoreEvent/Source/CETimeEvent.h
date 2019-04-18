//
//  CETimeEvent.h
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CETimeEvent_h
#define CETimeEvent_h

#include "CEBaseType.h"

uintptr_t CETimeEventInit(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, _Bool repeat, _Bool repeatModeAfter, uint64_t interval, CETimeEventHandler_f _Nonnull execute, void * _Nullable context, CETimeEventClearContextHandler_f _Nullable clearContext, CEResult_t * _Nonnull result);

uintptr_t CETimeEventCreateRepeatTimer(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, _Bool repeatModeAfter, uint64_t interval, CETimeEventHandler_f _Nonnull execute, void * _Nullable context, CETimeEventClearContextHandler_f _Nullable clearContext, CEResult_t * _Nonnull result);

uintptr_t CETimeEventCreateAfterTimer(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, CETimeEventHandler_f _Nonnull execute, void * _Nullable context, CETimeEventClearContextHandler_f _Nullable clearContext, CEResult_t * _Nonnull result);


CEResult_t CETimeEventDeinit(uintptr_t event);

_Bool CETimeEventExecute(uintptr_t event);


#endif /* CETimeEvent_h */
