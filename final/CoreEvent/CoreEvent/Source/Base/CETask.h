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


struct _CETaskExecuteContext;
typedef struct _CETaskExecuteContext CETaskExecuteContext_s;
typedef CETaskExecuteContext_s * CETaskExecuteContextRef;

struct _CETaskSync;
typedef struct _CETaskSync CETaskSync_s;
typedef CETaskSync_s * CETaskSyncRef;

struct _CETaskSync {
    CESemRef _Nullable syncSem;//同步信号量
    uint32_t requireResult: 1;
    CEParamRef _Nullable result;
};
struct _CETaskAsync {
    CEParamRef _Nullable result;

    
    CETaskParamItem_s inParamItems[0];
};
struct _CETaskExecuteContext {
    CEParamRef _Nullable * _Nullable resultRef;
    
    uint32_t inParamSize;//参数容纳总个数
    uint8_t * _Nullable inParamItemTypes;
    CETaskParamItem_s * _Nullable inParamItems;
};


struct _CETask;
typedef struct _CETask CETask_s;
typedef CETask_s * CETaskRef;
typedef CETaskRef CEEscapingTaskRef;
typedef CETaskRef CENoescapingTaskRef;



typedef void (*CETaskExecute_f)(CETaskExecuteContextRef _Nonnull context);

typedef struct _CETaskType {
    CETaskExecute_f _Nonnull execute;
} CETaskType_s;

struct _CETask {
//    const CETaskType_s * _Nonnull type;
    CETaskExecute_f _Nonnull execute;
    CESemRef _Nullable sem;//完成后发信号
    
    uint32_t inParamSize;//入参个数
#if CEBuild64Bit
    uint32_t xxxx: 32;
#endif
    
    CETaskParamItem_s * _Nullable inParamItems;
    CEParamRef _Nullable result;
    
    uint8_t content[];
};


CEEscapingTaskRef _Nullable CEEscapingTaskInit(CETaskExecute_f _Nonnull execute,
                                               CESemRef _Nullable sem,
                                               uint32_t itemCount);

CENoescapingTaskRef _Nullable CENoescapingTaskInit(CETaskExecute_f _Nonnull execute,
                                                   CESemRef _Nullable sem,
                                                   CEParamRef _Nullable * _Nullable resultReceiver,
                                                   CETaskParamItem_s * _Nullable items,
                                                   uint32_t count);

void CETaskRelease(CETaskRef _Nonnull task);

_Bool CEEscapingTaskSetParamItem(CEEscapingTaskRef _Nonnull task,
                                uint32_t index,
                                CEParamType_e type,
                                CETaskParamItemRelease_f _Nullable release,
                                void * _Nonnull valuePtr);

_Bool CEEscapingTaskGetParamItem(CEEscapingTaskRef _Nonnull task,
                                 uint32_t index,
                                 CETaskParamItem_s * _Nonnull itemRef);

#endif /* CETask_h */
