//
//  CSTime.c
//  CoreSystem
//
//  Created by 王青海 on 2018/11/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CSTime.h"

#if __APPLE__
#include <mach/mach_time.h>

#if __GNUC__
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (!!(x))
#define unlikely(x) (!!(x))
#endif // __GNUC__

typedef struct __CSMachTimeBaseInfo {
    long double frac;
    bool ratio_1_to_1;
} CSMachTimeBaseInfo_s;

static CSMachTimeBaseInfo_s __CSMachTimeBaseInfo = {};

static void CSMachTimeBaseInfoInit(mach_timebase_info_data_t * _Nonnull tbi) {
    __CSMachTimeBaseInfo.frac = tbi->numer;
    __CSMachTimeBaseInfo.frac /= tbi->denom;
    __CSMachTimeBaseInfo.ratio_1_to_1 = (tbi->numer == tbi->denom);
}

void __CSMachTimeBaseInfoShareOnceBlockFunc(void) {
    mach_timebase_info_data_t tbi;
    assert(0 == mach_timebase_info(&tbi));
    CSMachTimeBaseInfoInit(&tbi);
}
void __CSMachTimeBaseInfoOnceInit(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CSMachTimeBaseInfoShareOnceBlockFunc);
}

const CSMachTimeBaseInfo_s * _Nonnull CSMachTimeBaseInfo(void) {
    __CSMachTimeBaseInfoOnceInit();
    return &__CSMachTimeBaseInfo;
}

static inline struct timespec CSMachTimeToTime(uint64_t machtime) {
    struct timespec time;
    const CSMachTimeBaseInfo_s * data = CSMachTimeBaseInfo();
    if (unlikely(!machtime || data->ratio_1_to_1)) {
        time.tv_sec = machtime / NSEC_PER_SEC;
        time.tv_nsec = machtime % NSEC_PER_SEC;
        return time;
    }
    long double big_tmp = ((long double)machtime * data->frac) + .5L;
    long double big_tmp_sec = big_tmp / 1000000000.0L;
    
    if (unlikely(big_tmp_sec >= INT64_MAX)) {
        time.tv_sec = INT64_MAX;
        return time;
    }
    long sec = (long)big_tmp_sec;
    big_tmp_sec -= sec;
    time.tv_sec = sec;
    time.tv_nsec = (long)(big_tmp_sec * 1000000000.0L);
    
    return time;
}

struct timespec CSBootInterval(void) {
    uint64_t t = mach_continuous_time();
    return CSMachTimeToTime(t);
}

#else

struct timespec CSBootInterval(void) {

#if !defined(CLOCK_BOOTTIME_ALARM)
#error "CLOCK_BOOTTIME_ALARM undefined"
#endif
    
    struct timespec ts;
    clock_gettime(CLOCK_BOOTTIME_ALARM, &ts);
    return ts;
}

#endif




