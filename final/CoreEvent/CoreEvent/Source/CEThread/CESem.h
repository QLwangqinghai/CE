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

CESemRef _Nonnull CESemInit(unsigned int value);
void CESemDeinit(CESemRef _Nonnull sem);


void CESemWait(CESemRef _Nonnull sem);
void CESemWakeUp(CESemRef _Nonnull sem);



#endif /* CESem_h */
