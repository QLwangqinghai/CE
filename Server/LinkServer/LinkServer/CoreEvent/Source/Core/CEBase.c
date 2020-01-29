//
//  CEBase.c
//  CoreEvent
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#include "CEBase.h"


#pragma mark - time

CEMicrosecondTime CEGetCurrentTime(void) {
    return CCBootInterval();
}



#pragma mark - log


#if DEBUG

uint32_t CELogMask = 31;

#else

uint32_t CELogMask = 24;

#endif


uint32_t CELogVerboseLevel = 128;
