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


void CEGlobalTaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler);
void CEGlobalTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler);


void CESerialTaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler);
void CESerialTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler);

#endif /* CETaskScheduler_h */
