//
//  CCTime.c
//  SITP
//
//  Created by vector on 2019/12/24.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCBase.h"
#include <time.h>

const CCMicrosecondTime kCCAbsoluteTimeIntervalSince1970 = 978307200000000LL;


#if BUILD_TARGET_OS_WINDOWS
static const CCMicrosecondTime kCCAbsoluteTimeIntervalSince1601 = 12622780800000000LL;

CCMicrosecondTime CCMicrosecondTimeGetCurrent(void) {
    SYSTEMTIME stTime;
    FILETIME ftTime;

    GetSystemTime(&stTime);
    SystemTimeToFileTime(&stTime, &ftTime);

    // 100ns intervals since NT Epoch
    uint64_t result = ((uint64_t)ftTime.dwHighDateTime << 32)
                    | ((uint64_t)ftTime.dwLowDateTime << 0);
    return result / 10 - kCCAbsoluteTimeIntervalSince1601;
}
#else

CCMicrosecondTime CCMicrosecondTimeGetCurrent(void) {
    CCMicrosecondTime ret;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ret = ((CCMicrosecondTime)tv.tv_sec - kCCAbsoluteTimeIntervalSince1970) * 1000000;
    ret += (CCMicrosecondTime)tv.tv_usec;
    return ret;
}
#endif

/*
 Notes：各种系统时钟的区别

  时钟类型    解释    定时器    获取时间
 CLOCK_REALTIME    可设定的系统及实时时钟。
 common_timer_create

 HRTIMER_BASE_REALTIME

 posix_clock_realtime_get
 CLOCK_MONOTONIC    不可设定的恒定态时钟。 Linux上测量时间始于系统启动，系统启动后就不会发生改变，适用于那些无法容忍系统时钟发生跳跃性变化的应用程序。
 common_timer_create

 HRTIMER_BASE_MONOTONIC

 ktime_get

 posix_ktime_get_ts

 CLOCK_PROCESS_CPUTIME_ID     测量调用进程所消耗的用户和系统CPU时间。被调度出去则停止计时。      X    process_cpu_clock_get
 CLOCK_THREAD_CPUTIME_ID    和 CLOCK_PROCESS_CPUTIME_ID类似，不过对象是单条线程。      X    thread_cpu_clock_get
 CLOCK_MONOTONIC_RAW
 和 CLOCK_MONOTONIC类似，单提供了对纯基于硬件时间的访问，不受NTP调整影响。

 只能读取时钟，不支持timer。

   X    posix_get_monotonic_raw
 CLOCK_REALTIME_COARSE
 类似于CLOCK_REALTIME，适用于以最小代价获取较低时间戳的程序，不会引发对硬件时钟的访问，返回值分辨率为jiffy。

 只能读取时钟，不支持timer。

   X    posix_get_realtime_coarse
 CLOCK_MONOTONIC_COARSE
 类似于CLOCK_MONOTONIC，适用于以最小代价获取较低时间戳的程序，不会引发对硬件时钟的访问，返回值分辨率为jiffy。

 只能读取时钟，不支持timer。

  X    posix_get_monotonic_coarse
 CLOCK_BOOTTIME    和CLOCK_MONOTONIC类似，但是累积suspend时间。
 common_timer_create

 HRTIMER_BASE_BOOTTIME

 posix_get_boottime
 CLOCK_REALTIME_ALARM
 alarm_timer_create

 HRTIMER_BASE_REALTIME

 ktime_get_real
 CLOCK_BOOTTIME_ALARM
 alarm_timer_create

 HRTIMER_BASE_BOOTTIME

 ktime_get_boottime
 CLOCK_TAI    原子时钟时间
 */

#if __APPLE__

#include <stdlib.h>
#include <mach/mach_time.h>
#include <sys/time.h>

CCBootMicrosecondTime CCBootInterval(void) {
    mach_timebase_info_data_t timeBaseInfo = {};
    assert(0 == mach_timebase_info(&timeBaseInfo));
    uint64_t t = mach_continuous_time();
    return t / 1000 * timeBaseInfo.numer / timeBaseInfo.denom;
}

#else

#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdlib.h>

CCBootMicrosecondTime CCBootInterval(void) {

#if !defined(CLOCK_BOOTTIME_ALARM)
#error "CLOCK_BOOTTIME_ALARM undefined"
#endif
    
    struct timespec ts;
    assert(0 == clock_gettime(CLOCK_BOOTTIME_ALARM, &ts));
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

#endif



