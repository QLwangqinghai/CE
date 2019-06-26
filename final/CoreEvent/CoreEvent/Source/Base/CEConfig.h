//
//  CEConfig.h
//  CoreEvent
//
//  Created by vector on 2018/11/26.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEConfig_h
#define CEConfig_h

#include <CoreEvent/CConfig.h>

#define CEBuild64Bit CBuild64Bit

#if CEBuild64Bit
#define CECpuWordByteSize 8
#else
#define CECpuWordByteSize 4
#endif


#ifdef EVENT_HAVE_EPOLL
#define CORE_EVENT_USE_EPOLL 1
#define CORE_EVENT_USE_KQUEUE 0
#else
#ifdef EVENT_HAVE_KQUEUE
#define CORE_EVENT_USE_EPOLL 0
#define CORE_EVENT_USE_KQUEUE 1
#else
#error "不支持的平台"
#endif
#endif


//编译选项宏定义

#define CESemUseDispatch 2
#define CESemUseFileSem 1
#define CESemUseDefaultSem 0


#if __APPLE__
#define CESemUseType CESemUseDispatch
#else
#define CESemUseType CESemUseDefaultSem
#endif



#endif /* CEConfig_h */
