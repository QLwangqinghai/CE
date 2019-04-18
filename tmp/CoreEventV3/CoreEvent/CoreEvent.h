//
//  CoreEvent.h
//  CoreEvent
//
//  Created by 王青海 on 2018/10/23.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#import <Cocoa/Cocoa.h>

//! Project version number for CoreEvent.
FOUNDATION_EXPORT double CoreEventVersionNumber;

//! Project version string for CoreEvent.
FOUNDATION_EXPORT const unsigned char CoreEventVersionString[];


#import <CoreEvent/CETime.h>
#import <CoreEvent/CELog.h>
#import <CoreEvent/CEBaseType.h>
#import <CoreEvent/CEFileEvent.h>
#import <CoreEvent/CETimeEvent.h>
#import <CoreEvent/CERunLoop.h>

// In this header, you should import all the public headers of your framework using statements like #import <CoreEvent/PublicHeader.h>

//static CERunLoop_s * _Nonnull __CERunLoopShared = NULL;
//void CERunLoopSharedCreate(void) {
//    struct rlimit limit;
//    if(getrlimit(RLIMIT_NOFILE,&limit) != 0) {
//        fprintf(stderr, "getrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
//        fflush(stderr);
//        abort();
//    }
//    //    limit.rlim_cur = limit.rlim_cur > 1024 * 1024 ? limit.rlim_cur : 1024 * 1024;
//    //    limit.rlim_cur = OPEN_MAX < limit.rlim_max ? OPEN_MAX : limit.rlim_max;
//
//    //https://www.jb51.net/article/132035.htm
//    //https://blog.csdn.net/pao98pao/article/details/53322868
//    //https://www.baidu.com/s?wd=sysctl+%E8%8E%B7%E5%8F%96%E5%86%85%E6%A0%B8%E5%8F%82%E6%95%B0&tn=84053098_3_dg&ie=utf-8
//    //https://blog.csdn.net/whereismatrix/article/details/50582919
//    //https://blog.csdn.net/z1134145881/article/details/52573441
//    //https://blog.csdn.net/ying0216/article/details/4024143
//    //
//
//    //参数调优 https://www.cnblogs.com/zengkefu/p/5602473.html
//
//    int mib[2] = {CTL_KERN, KERN_MAXFILESPERPROC};
//    int maxFilesPerProc = 0;
//    size_t size = sizeof(maxFilesPerProc);
//
//    if (sysctl(mib, 2, &maxFilesPerProc, &size, NULL, 0) != 0) {
//        fprintf(stderr, "sysctl KERN_MAXFILESPERPROC error %s; \n", strerror(errno));
//        fflush(stderr);
//        abort();
//    }
//    if (limit.rlim_cur < maxFilesPerProc) {
//        limit.rlim_cur = maxFilesPerProc;
//        printf("setrlimit rlim_cur %lld; \n", limit.rlim_cur);
//        if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
//            fprintf(stderr, "setrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
//            fflush(stderr);
//            abort();
//        }
//    }
//
//    int value = (int)(limit.rlim_cur);
//
//    __CERunLoopShared = CERunLoopCreate(value);
//}



#import <CoreEvent/CERunLoop.h>
