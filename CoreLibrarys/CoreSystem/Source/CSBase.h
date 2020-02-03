//
//  CSBase.h
//  CoreSystem
//
//  Created by vector on 2018/12/10.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CSBase_h
#define CSBase_h


#include <CoreSystem/CSTarget.h>

#include <CoreSystem/CSAnsiCHeader.h>
#include <CoreSystem/CSPosixHeader.h>

//#include <sys/time.h>
//#include <sys/times.h>

#if CS_TARGET_OS_DARWIN
#include <AvailabilityMacros.h>
#endif

#if CS_TARGET_OS_LINUX
#include <linux/version.h>
#include <features.h>
#endif

#if CS_TARGET_OS_LINUX
#include <linux/version.h>
#include <features.h>
#endif

#ifdef __unix__ /* __unix__ is usually defined by compilers targeting Unix systems */
# include <unistd.h>
#endif


#if CS_TARGET_OS_DARWIN
#include <malloc/malloc.h>

#elif CS_TARGET_OS_LINUX
#include <malloc.h>
#elif CS_TARGET_OS_WINDOWS
# include <windows.h>
#define __builtin_expect(P1,P2) P1
#else
#error "不支持的平台"
#endif


CS_EXTERN_C_BEGIN


#if __GNUC__
#define CSCompilerBarrier()  __asm__ __volatile__("" ::: "memory")
#else
#define CSCompilerBarrier()  do { } while (0)
#endif


#define CSMutexLockInit ((pthread_mutex_t)PTHREAD_ERRORCHECK_MUTEX_INITIALIZER)


#if CS_TARGET_OS_DARWIN
#include <mach/mach.h>
static inline size_t CSMemoryPageSize() { return (size_t)vm_page_size; }
#elif CS_TARGET_OS_WINDOWS
static inline size_t CSMemoryPageSize() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return (size_t)(sysInfo.dwPageSize);
}
#elif CS_TARGET_OS_LINUX
static inline size_t CSMemoryPageSize() {
    return (size_t)getpagesize();
}
#else

#error "不支持的平台"

#endif








CS_EXTERN_C_END

#endif /* CSBase_h */
