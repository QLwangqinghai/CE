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
#include "CEThreadBaseInternal.h"

#include "CEParam.h"


CETaskPtr _Nonnull CETaskCreate(CEFunction_f _Nonnull execute, CEParamRef _Nullable param, CEParamRef _Nullable resultReceiver, CEThreadSyncWaiter_s * _Nullable syncTaskWaiter, _Bool isBarrier);
void CETaskDestroy(CETaskPtr _Nonnull task);

#endif /* CETask_h */
