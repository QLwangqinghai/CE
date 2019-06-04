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


void CETaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler);

void CETaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler);



#endif /* CETaskScheduler_h */
