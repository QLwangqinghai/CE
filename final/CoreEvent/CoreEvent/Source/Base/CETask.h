//
//  CETask.h
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CETask_h
#define CETask_h

#include "CEBase.h"
#include "CESem.h"

#include "CEParam.h"
#include "CETaskParam.h"
#include "CETaskExecuteObserver.h"

struct _CETaskExecuteContext;
typedef struct _CETaskExecuteContext CETaskExecuteContext_s;
typedef CETaskExecuteContext_s * CETaskExecuteContextRef;

struct _CETaskExecuteContext {
    CEParamRef _Nullable * _Nullable resultReceiver;
    CEParamType_e * _Nullable paramTypes;
    CEParamItemValue_u * _Nullable paramItems;
    uint32_t paramCount;
};

struct _CETask;
typedef struct _CETask CETask_s;
typedef CETask_s * CETaskRef;
typedef CETaskRef CEEscapingTaskRef;

struct _CENoescapingTask;
typedef struct _CENoescapingTask CENoescapingTask_s;
typedef CENoescapingTask_s * CENoescapingTaskRef;

typedef void (*CETaskExecute_f)(CETaskExecuteContextRef _Nonnull context);
typedef void (*CETaskRelease_f)(CETaskRef _Nonnull task);


typedef struct _CETaskBase {
    uint64_t tid: 56;
    uint64_t type: 3;
    uint64_t paramItemCount: 5;//入参个数
    
    CETaskRelease_f _Nonnull release;
    CETaskExecute_f _Nonnull execute;
    CETaskExecuteObserverRef _Nullable observer;
} CETaskBase_t;


struct _CENoescapingTask {
    CETaskBase_t base;
    CEParamType_e paramTypes[CETaskParamItemBufferSize];
    CEParamItemValue_u paramItems[CETaskParamItemBufferSize];
};




_Bool CENoescapingTaskInit(CENoescapingTaskRef _Nonnull task,
                           CETaskExecute_f _Nonnull execute,
                           CETaskExecuteObserverRef _Nullable observer,
                           uint32_t itemCount);

CEEscapingTaskRef _Nullable CEEscapingTaskInit(CETaskExecute_f _Nonnull execute,
                                               CETaskExecuteObserverRef _Nullable observer,
                                               uint32_t itemCount);

void CEEscapingTaskRelease(CEEscapingTaskRef _Nonnull task);

_Bool CEEscapingTaskSetParamItem(CEEscapingTaskRef _Nonnull task,
                                uint32_t index,
                                CEParamType_e type,
                                CETaskParamItemRelease_f _Nullable release,
                                void * _Nonnull valuePtr);

_Bool CEEscapingTaskGetParamItem(CEEscapingTaskRef _Nonnull task,
                                 uint32_t index,
                                 CEParamItemValue_u * _Nullable itemRef,
                                 CEParamType_e * _Nullable itemTypeRef);

#endif /* CETask_h */
