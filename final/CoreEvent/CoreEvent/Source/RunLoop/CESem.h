//
//  CESem.h
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CESem_h
#define CESem_h

#include <stdio.h>


struct _CESem;
typedef struct _CESem CESem_s;
typedef CESem_s * CESemRef;

CESemRef _Nonnull CESemInit(unsigned int value);
void CESemDeinit(CESemRef _Nonnull sem);


void CESemWait(CESemRef _Nonnull sem);
void CESemWakeUp(CESemRef _Nonnull sem);



#endif /* CESem_h */
