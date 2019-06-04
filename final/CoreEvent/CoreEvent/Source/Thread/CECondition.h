//
//  CECondition.h
//  CoreEvent
//
//  Created by vector on 2019/6/3.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CECondition_h
#define CECondition_h

#include "CEThreadBase.h"

CESemPtr _Nonnull CESemInit(unsigned int value);
void CESemDeinit(CESemPtr _Nonnull sem);


void CESemWait(CESemPtr _Nonnull sem);
void CESemSignal(CESemPtr _Nonnull sem);

#endif /* CECondition_h */
