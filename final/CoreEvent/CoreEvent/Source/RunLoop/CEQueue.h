//
//  CEQueue.h
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEQueue_h
#define CEQueue_h

#include "CERunLoopBase.h"


void CEQueueSync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param, CEParamRef _Nullable result);

void CEQueueAsync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param);

void CEQueueBarrierSync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param, CEParamRef _Nullable result);

void CEQueueBarrierAsync(CEQueueRef _Nonnull queuePtr, CEFunction_f _Nonnull execute, CEParamRef _Nonnull param);

#endif /* CEQueue_h */
