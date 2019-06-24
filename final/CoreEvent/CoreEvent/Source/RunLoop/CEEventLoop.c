//
//  CEEventLoop.c
//  CoreEvent
//
//  Created by vector on 2019/6/23.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEEventLoop.h"
#include <poll.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <fcntl.h>




struct _CERunLoop;
typedef struct _CERunLoop CERunLoop_s;

/* State of an event based program */
struct _CERunLoop {
    int fdLimit;
    int maxfd;   /* highest file descriptor currently registered */
    int setsize; /* max number of file descriptors tracked */
    long long timeEventNextId;
    time_t lastTime;     /* Used to detect system clock skew */
//    CEFileEvent_s * _Nonnull events; /* Registered events */
//    CEFiredEvent_s * _Nonnull fired; /* Fired events */
//    CETimeEvent_s * _Nullable timeEventHead;
//    int stop;
//    void * _Nullable api; /* This is used for polling API specific data */
//    CEStateChangeHandler_f _Nullable beforesleep;
//    CEStateChangeHandler_f _Nullable aftersleep;
};



static CERunLoop_s * _Nonnull __CERunLoopShared = NULL;
void CEEventLoopSharedCreate(void) {
    struct rlimit limit;
    if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
        printf("getrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
        abort();
    }
    //    limit.rlim_cur = limit.rlim_cur > 1024 * 1024 ? limit.rlim_cur : 1024 * 1024;
    //    limit.rlim_cur = OPEN_MAX < limit.rlim_max ? OPEN_MAX : limit.rlim_max;
    
    //https://www.jb51.net/article/132035.htm
    //https://blog.csdn.net/pao98pao/article/details/53322868
    //https://www.baidu.com/s?wd=sysctl+%E8%8E%B7%E5%8F%96%E5%86%85%E6%A0%B8%E5%8F%82%E6%95%B0&tn=84053098_3_dg&ie=utf-8
    //https://blog.csdn.net/whereismatrix/article/details/50582919
    //https://blog.csdn.net/z1134145881/article/details/52573441
    //https://blog.csdn.net/ying0216/article/details/4024143
    //
    
    //参数调优 https://www.cnblogs.com/zengkefu/p/5602473.html
    
    int mib[2] = {CTL_KERN, KERN_MAXFILESPERPROC};
    int maxFilesPerProc = 0;
    size_t size = sizeof(maxFilesPerProc);
    
    if (sysctl(mib, 2, &maxFilesPerProc, &size, NULL, 0) != 0) {
        printf("sysctl KERN_MAXFILESPERPROC error %s; \n", strerror(errno));
        abort();
    }
    if (limit.rlim_cur < maxFilesPerProc) {
        limit.rlim_cur = maxFilesPerProc;
        printf("setrlimit rlim_cur %lld; \n", limit.rlim_cur);
        if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
            printf("setrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
            abort();
        }
    }
    
    int value = (int)(limit.rlim_cur);
    
//    __CERunLoopShared = CERunLoopCreate(value);
    __CERunLoopShared->fdLimit = value;
}
