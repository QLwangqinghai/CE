//
//  CTWaiter.h
//  CoreThread
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//
#ifndef CTWaiter_h
#define CTWaiter_h

#include "CTBaseType.h"


CTSyncWaiter_s * _Nonnull CTSyncWaiterGetCurrent(void);
void CTSyncWaiterWait(CTSyncWaiter_s * _Nonnull waiter);
void CTSyncWaiterWakeUp(CTSyncWaiter_s * _Nonnull waiter);

void CTShare(void);

#endif /* CEThread_h */
