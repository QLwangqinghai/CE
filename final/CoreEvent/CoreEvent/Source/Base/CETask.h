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
#include "CEParam.h"
#include "CESem.h"


struct _CETask;
typedef struct _CETask CETask_s;
typedef CETask_s * CETaskRef;



typedef void (*CETaskExecute_f)(CETaskRef _Nonnull task);
typedef void (*CETaskClearData_f)(uintptr_t closure, const void * _Nullable data, size_t dataSize);


typedef struct _CETaskType {
    CETaskExecute_f _Nonnull execute;
} CETaskType_s;

struct _CETask {
    const CETaskType_s * _Nonnull type;
    CESemRef _Nullable syncSem;//同步信号量
    CEParamRef _Nullable out;
    
    size_t inParamSize;//参数容纳总个数
    CEParamItem_s inParamItems[0];
};




#endif /* CETask_h */
