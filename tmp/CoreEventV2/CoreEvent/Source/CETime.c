//
//  CETime.c
//  CoreEvent
//
//  Created by 王青海 on 2018/11/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CETime.h"

#if __APPLE__

#include <stdlib.h>
#include <mach/mach_time.h>
#include <sys/time.h>

uint64_t CESystemBootInterval(void) {
    mach_timebase_info_data_t timeBaseInfo = {};
    mach_timebase_info(&timeBaseInfo);
    uint64_t t = mach_continuous_time() / NSEC_PER_USEC;
    uint64_t microseconds = t * ((double)timeBaseInfo.numer / (double)timeBaseInfo.denom);
    return microseconds;
}

#else

#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdlib.h>

uint64_t CESystemBootInterval(void) {
    
#if !defined(CLOCK_BOOTTIME_ALARM)
#error "CLOCK_BOOTTIME_ALARM undefined"
#endif
    
    struct timespec ts;
    clock_gettime(CLOCK_BOOTTIME_ALARM, &ts);
    return ts.tv_nsec / 1000ull + ts.tv_sec * 1000000ull;
}


#endif




