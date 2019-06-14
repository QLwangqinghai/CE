//
//  CEMainQueue.h
//  CoreEvent
//
//  Created by vector on 2019/6/11.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEMainQueue_h
#define CEMainQueue_h

#include "CEQueueInternal.h"


CEQueue_s * _Nonnull _CEQueueSharedMainQueue(void);

CESource_s * _Nonnull _CESourceMainCreate(CEQueue_s * _Nonnull queue, CEPtr _Nonnull context);

#endif /* CEMainQueue_h */

