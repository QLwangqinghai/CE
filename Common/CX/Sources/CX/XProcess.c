//
//  XProcess.c
//  
//
//  Created by vector on 2020/4/3.
//

#include "XProcess.h"
#include <sys/time.h>


#if BUILD_TARGET_OS_DARWIN

#include <mach-o/dyld.h>
int XProcessExecutablePath(char * _Nonnull buf, uint32_t * _Nonnull bufsize) {
    return _NSGetExecutablePath(buf, bufsize);
}

#elif BUILD_TARGET_OS_LINUX

#include <unistd.h>
int XProcessExecutablePath(char* buf, uint32_t * bufsize) {
    if (NULL == buf || NULL == bufsize) {
        return -1;
    }
    ssize_t ret = readlink("/proc/self/exe", path, (size_t)(*bufsize));
    if (ret <= 0) {
        return -1;
    } else {
        *bufsize = (uint32_t)ret;
    }
}
#else
#error Unable XProcessExecutablePath for this platform
#endif
