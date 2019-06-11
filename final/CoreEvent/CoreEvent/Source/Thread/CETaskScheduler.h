//
//  CETaskScheduler.h
//  CoreEvent
//
//  Created by vector on 2019/6/1.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CETaskScheduler_h
#define CETaskScheduler_h

#include "CEThreadBaseInternal.h"


typedef uint_fast32_t CETaskSchedulerStatus_t;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusNoThread = 0;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusCreatingThread = 1;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusRunning = 2;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusWaiting = 3;


/*
 typedef struct _CETaskScheduler {
 CESpinLockPtr _Nonnull lock;
 CESemPtr _Nonnull waiter;
 CETaskSchedulerSignal_f _Nonnull signal;
 
 CEThread_s * _Nonnull thread;
 CEQueueRef _Nullable ownerQueue;//当前queue， 如果是个串行队列的线程，ownerQueue 一直有值
 CESourceRef _Nonnull source;
  
 uint8_t context[64];
 } CETaskScheduler_s;

 */

void CETaskSchedulerExecuteTask(CETaskSchedulerPtr _Nonnull scheduler, CETaskPtr _Nonnull task);

CETaskSchedulerPtr _Nonnull CETaskSchedulerCreate(CEQueue_s * _Nullable ownerQueue, CETaskSchedulerSignal_f _Nonnull signal);

void CETaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler);


#endif /* CETaskScheduler_h */
