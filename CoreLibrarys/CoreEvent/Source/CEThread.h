//
//  CEThread.h
//  CoreEvent
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEThread_h
#define CEThread_h

#include <CoreEvent/CEBaseTypeInternal.h>


pthread_key_t CERunLoopThreadKeyShared(void);
CEThreadWaiter_s * _Nonnull CEThreadGetWaiter(void);


void CEThreadWaiterWait(CEThreadWaiter_s * _Nonnull waiter);
void CEThreadWaiterWakeUp(CEThreadWaiter_s * _Nonnull waiter);

#endif /* CEThread_h */
