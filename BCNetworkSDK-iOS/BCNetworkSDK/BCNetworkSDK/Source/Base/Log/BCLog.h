//
//  BCLog.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/8/9.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CocoaLumberjack/DDLog.h>


#if DEBUG
static const int BCLogLevel = LOG_LEVEL_VERBOSE;
#else
static const int BCLogLevel = LOG_LEVEL_WARN;
#endif

#define BCLogError(frmt, ...)   LOG_OBJC_MAYBE(LOG_ASYNC_ERROR,   BCLogLevel, LOG_FLAG_ERROR,   0, frmt, ##__VA_ARGS__)
#define BCLogWarn(frmt, ...)    LOG_OBJC_MAYBE(LOG_ASYNC_WARN,    BCLogLevel, LOG_FLAG_WARN,    0, frmt, ##__VA_ARGS__)
#define BCLogInfo(frmt, ...)    LOG_OBJC_MAYBE(LOG_ASYNC_INFO,    BCLogLevel, LOG_FLAG_INFO,    0, frmt, ##__VA_ARGS__)
#define BCLogDebug(frmt, ...)   LOG_OBJC_MAYBE(LOG_ASYNC_DEBUG,   BCLogLevel, LOG_FLAG_DEBUG,   0, frmt, ##__VA_ARGS__)
#define BCLogVerbose(frmt, ...) LOG_OBJC_MAYBE(LOG_ASYNC_VERBOSE, BCLogLevel, LOG_FLAG_VERBOSE, 0, frmt, ##__VA_ARGS__)


@interface BCLog : NSObject



@end


