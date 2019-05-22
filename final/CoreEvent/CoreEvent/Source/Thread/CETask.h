//
//  CETask.h
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CETask_h
#define CETask_h

#include "CERunLoopBase.h"
#include "CESem.h"

#include "CEParam.h"
#include "CETaskExecuteObserver.h"


#define CEStackParamSize(capacity, structSize) (CECpuWordByteSize + 8 + 12 * capacity + structSize)



_Bool CENoescapingTaskInit(CENoescapingTaskRef _Nonnull task,
                           CETaskExecute_f _Nonnull execute,
                           CETaskExecuteObserverRef _Nullable observer,
                           uint32_t itemCount);

CEEscapingTaskRef _Nullable CEEscapingTaskInit(CETaskExecute_f _Nonnull execute,
                                               CETaskExecuteObserverRef _Nullable observer,
                                               uint32_t itemCount);

void CEEscapingTaskRelease(CEEscapingTaskRef _Nonnull task);

//_Bool CEEscapingTaskSetParamItem(CEEscapingTaskRef _Nonnull task,
//                                uint32_t index,
//                                CEParamType_e type,
//                                CETaskParamItemRelease_f _Nullable release,
//                                void * _Nonnull valuePtr);

_Bool CEEscapingTaskGetParamItem(CEEscapingTaskRef _Nonnull task,
                                 uint32_t index,
                                 CEParamItemValue_u * _Nullable itemRef,
                                 CEParamType_e * _Nullable itemTypeRef);

#endif /* CETask_h */
