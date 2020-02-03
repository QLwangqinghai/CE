//
//  CELog.c
//  CoreEvent
//
//  Created by vector on 2018/11/28.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CELog.h"

#if DEBUG

uint32_t CELogMask = 31;

#else

uint32_t CELogMask = 24;

#endif


uint32_t CELogVerboseLevel = 128;
