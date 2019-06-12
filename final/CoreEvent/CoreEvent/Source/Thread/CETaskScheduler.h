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

CETaskSchedulerPtr _Nonnull CETaskSchedulerCreate(CEQueue_s * _Nullable ownerQueue);

void CETaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler);


#endif /* CETaskScheduler_h */
