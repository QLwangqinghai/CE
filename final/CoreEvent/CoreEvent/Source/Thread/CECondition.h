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

void CEConditionInit(CEConditionPtr _Nonnull lock);
void CEConditionDestroy(CEConditionPtr _Nonnull lock);

void CEConditionWait(CEConditionPtr _Nonnull lock);
void CEConditionSignal(CEConditionPtr _Nonnull lock);

#endif /* CECondition_h */
