//
//  CCTime.c
//  SITP
//
//  Created by vector on 2019/12/24.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCTime.h"

const CCMicrosecondTime kCCAbsoluteTimeIntervalSince1970 = 978307200000000LL;

#if TARGET_OS_WIN32
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
