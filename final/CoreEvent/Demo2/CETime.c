//
//  CETime.c
//  CoreEvent
//
//  Created by 王青海 on 2018/11/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CETime.h"
#include <assert.h>

#if __APPLE__

#include <stdlib.h>
#include <mach/mach_time.h>
#include <sys/time.h>

static inline struct timespec CEMachTimeToTime(uint64_t t, mach_timebase_info_data_t timeBaseInfo) {
    struct timespec time = {};
    if (1 == timeBaseInfo.numer) {
        uint64_t mt = t;
        uint64_t nsec = mt / timeBaseInfo.denom;
        time.tv_sec = nsec / NSEC_PER_SEC;
        time.tv_nsec = nsec % NSEC_PER_SEC;
    } else {
        uint64_t h = t >> 32;
        uint64_t l = (t << 32) >> 32;
        
        h = h * timeBaseInfo.numer;
        l = l * timeBaseInfo.numer;
        
        uint64_t h1 = h / timeBaseInfo.denom;
        uint64_t rh = h % timeBaseInfo.denom;
        
        if (rh != 0) {
            uint64_t need = (timeBaseInfo.denom - rh) << 32;
            if (l >= need) {
                l -= need;
                h1 += 1;
            } else {
                l += rh << 32;
            }
        }
        
        uint64_t l1 = l / timeBaseInfo.denom;
        
        if (UINT64_MAX - l1 <= (h1 << 32)) {
            uint64_t nsec = (h1 << 32) + l1;
            time.tv_sec = nsec / NSEC_PER_SEC;
            time.tv_nsec = nsec % NSEC_PER_SEC;
        } else {
            uint64_t h2 = h1 / NSEC_PER_SEC;
            uint64_t rh1 = h1 % NSEC_PER_SEC;
            l1 += (rh1 << 32);
            time.tv_sec = (h2 << 32) + l1 / NSEC_PER_SEC;
            time.tv_nsec = l1 % NSEC_PER_SEC;
        }
    }
    return time;
}


struct timespec CESystemBootInterval(void) {
    mach_timebase_info_data_t timeBaseInfo = {};
    assert(0 == mach_timebase_info(&timeBaseInfo));
    uint64_t t = mach_continuous_time();
    return CEMachTimeToTime(t, timeBaseInfo);
}


#else

#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdlib.h>

struct timespec CESystemBootInterval(void) {

#if !defined(CLOCK_BOOTTIME_ALARM)
#error "CLOCK_BOOTTIME_ALARM undefined"
#endif
    
    struct timespec ts;
    clock_gettime(CLOCK_BOOTTIME_ALARM, &ts);
    return ts;
}

#endif




uint64_t CESystemBootMicroseconds(void) {
    struct timespec ts = CESystemBootInterval();
    return ts.tv_nsec / 1000ull + ts.tv_sec * 1000000ull;
}



