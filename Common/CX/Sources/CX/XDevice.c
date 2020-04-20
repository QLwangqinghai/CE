//
//  XDevice.c
//  
//
//  Created by vector on 2020/4/3.
//

#include "XDevice.h"




#if BUILD_TARGET_OS_DARWIN

#include <stdlib.h>
#include <mach/mach_time.h>
#include <sys/time.h>

XTimeInterval XDeviceUptime(void) {
    mach_timebase_info_data_t timeBaseInfo = {};
    assert(0 == mach_timebase_info(&timeBaseInfo));
    uint64_t t = mach_continuous_time();
    return t / 1000 * timeBaseInfo.numer / timeBaseInfo.denom;
}

#elif BUILD_TARGET_OS_LINUX || BUILD_TARGET_OS_BSD || BUILD_TARGET_OS_ANDROID

#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdlib.h>

XTimeInterval XDeviceUptime(void) {
    struct timespec ts;
    assert(0 == clock_gettime(CLOCK_MONOTONIC, &ts));
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

#else
#error Unable XDeviceUptime for this platform
#endif


XUInt XDeviceProcessorCount(void) {
    int32_t pcnt;
#if BUILD_TARGET_OS_WINDOWS
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD_PTR activeProcessorMask = sysInfo.dwActiveProcessorMask;
    // assumes sizeof(DWORD_PTR) is 64 bits or less
    uint64_t v = activeProcessorMask;
    v = v - ((v >> 1) & 0x5555555555555555ULL);
    v = (v & 0x3333333333333333ULL) + ((v >> 2) & 0x3333333333333333ULL);
    v = (v + (v >> 4)) & 0xf0f0f0f0f0f0f0fULL;
    pcnt = (v * 0x0101010101010101ULL) >> ((sizeof(v) - 1) * 8);
#elif BUILD_TARGET_OS_DARWIN
    int32_t mib[] = {CTL_HW, HW_AVAILCPU};
    size_t len = sizeof(pcnt);
    int32_t result = sysctl(mib, sizeof(mib) / sizeof(int32_t), &pcnt, &len, NULL, 0);
    if (result != 0) {
        pcnt = 0;
    }
#elif BUILD_TARGET_OS_LINUX
    pcnt = sysconf(_SC_NPROCESSORS_ONLN);
#else
    // Assume the worst
    pcnt = 1;
#endif
    return pcnt;
}


#if BUILD_TARGET_OS_DARWIN
#include <sys/sysctl.h>
XSize XDeviceCachelineSize(void) {
    
    int name[] = {CTL_HW,HW_CACHELINE};
    size_t lineSize = 0;
    size_t sizeOfLineSize = sizeof(lineSize);

    int code = sysctl(name, 2, &lineSize, &sizeOfLineSize, NULL, 0); // getting size of answer
    if (0 == code) {
        return (XSize)lineSize;
    } else {
        return 64;
    }
}
#elif BUILD_TARGET_OS_WINDOWS

#include <stdlib.h>
#include <windows.h>
XSize XDeviceCachelineSize() {
    size_t line_size = 0;
    DWORD buffer_size = 0;
    DWORD i = 0;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

    GetLogicalProcessorInformation(0, &buffer_size);
    buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(buffer_size);
    GetLogicalProcessorInformation(&buffer[0], &buffer_size);

    for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
            line_size = buffer[i].Cache.LineSize;
            break;
        }
    }

    free(buffer);
    return line_size;
}

#elif BUILD_TARGET_OS_LINUX || BUILD_TARGET_OS_ANDROID

#include <stdio.h>
XSize XDeviceCachelineSize() {
    FILE * p = 0;
    p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
    unsigned int i = 0;
    if (p) {
        fscanf(p, "%d", &i);
        fclose(p);
    }
    return i;
}

#else
    #error Unrecognized platform
#endif
