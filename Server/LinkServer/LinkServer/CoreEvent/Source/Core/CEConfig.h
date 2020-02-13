//
//  CEConfig.h
//  CoreEvent
//
//  Created by vector on 2018/11/26.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEConfig_h
#define CEConfig_h

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef __APPLE__
#include <AvailabilityMacros.h>
#endif

#ifdef __linux__
#include <linux/version.h>
#include <features.h>
#endif

/* Test for proc filesystem */
#ifdef __linux__
#define HAVE_PROC_STAT 1
#define HAVE_PROC_MAPS 1
#define HAVE_PROC_SMAPS 1
#define HAVE_PROC_SOMAXCONN 1
#endif

/* Test for task_info() */
#if defined(__APPLE__)
#define HAVE_TASKINFO 1
#endif

/* Test for backtrace() */
#if defined(__APPLE__) || (defined(__linux__) && defined(__GLIBC__))
#define HAVE_BACKTRACE 1
#endif

/* MSG_NOSIGNAL. */
#ifdef __linux__
#define HAVE_MSG_NOSIGNAL 1
#endif

/* Test for polling API */
#ifdef __linux__
#define CORE_EVENT_HAVE_EPOLL 1
#endif

#if (defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)
#define CORE_EVENT_HAVE_KQUEUE 1
#endif


#ifdef CORE_EVENT_HAVE_EPOLL
#define CORE_EVENT_USE_EPOLL 1
#define CORE_EVENT_USE_KQUEUE 0
#else
#ifdef CORE_EVENT_HAVE_KQUEUE
#define CORE_EVENT_USE_EPOLL 0
#define CORE_EVENT_USE_KQUEUE 1
#else
#error "不支持的平台"
#endif
#endif



/* Byte ordering detection */
#include <sys/types.h> /* This will likely define BYTE_ORDER */


#if defined(__cplusplus)
}
#endif

#endif /* CEConfig_h */
