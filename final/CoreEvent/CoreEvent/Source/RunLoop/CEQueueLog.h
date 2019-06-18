//
//  CEQueueLog.h
//  CoreEvent
//
//  Created by vector on 2019/6/18.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEQueueLog_h
#define CEQueueLog_h


#include <CoreEvent/CELog.h>

#define CEQueueLogAble 1

#if CEQueueLogAble

#define CEQueueLog(format, ...) CELogVerbose(format, ##__VA_ARGS__)

#else

#define CEQueueLog(format, ...) do {\
} while(0);


#endif

#endif /* CEQueueLog_h */
