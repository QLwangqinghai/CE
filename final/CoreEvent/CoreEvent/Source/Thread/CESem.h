//
//  CESem.h
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CESem_h
#define CESem_h

#include "CERunLoopBase.h"

CESemPtr _Nonnull CESemInit(unsigned int value);
void CESemDeinit(CESemPtr _Nonnull sem);


void CESemWait(CESemPtr _Nonnull sem);
void CESemWakeUp(CESemPtr _Nonnull sem);



#endif /* CESem_h */
