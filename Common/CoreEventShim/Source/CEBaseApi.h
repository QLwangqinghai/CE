//
//  CEBaseApi.h
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEBaseApi_h
#define CEBaseApi_h

#if defined(__cplusplus)
extern "C" {
#endif
#include <stdio.h>

#ifdef __APPLE__
#include <AvailabilityMacros.h>
#endif

#ifdef __linux__
#include <linux/version.h>
#include <features.h>
#endif

#if defined(__GNUC__)
#include <stdbool.h>
#endif

/* Test for backtrace() */
#if defined(__APPLE__) || (defined(__linux__) && defined(__GLIBC__))
#define CORE_EVENT_HAVE_BACKTRACE 1
#endif

/* MSG_NOSIGNAL. */
#ifdef __linux__
#define CORE_EVENT_HAVE_MSG_NOSIGNAL 1
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

#define CORE_EVENT_READ_WRITE_SEPARATION CORE_EVENT_USE_KQUEUE


typedef uint32_t CEFileEventMask_es;

extern const CEFileEventMask_es CEFileEventMaskNone;
extern const CEFileEventMask_es CEFileEventMaskReadable;
extern const CEFileEventMask_es CEFileEventMaskWritable;
extern const CEFileEventMask_es CEFileEventMaskReadWritable;

typedef struct {
    int fd;
    uint32_t xxx: 29;
    uint32_t error: 1;
    uint32_t status: 2;//CEFileEventMask_es
} CEEvent_s;

static inline CEEvent_s CEEventMake(int fd, uint32_t error, CEFileEventMask_es status) {
    CEEvent_s e = {};
    e.fd = fd;
    e.error = error != 0 ? 1 : 0;
    e.status = status;
    return e;
}

typedef void (*CEBaseApiPollCallback_f)(void * _Nonnull api, void * _Nullable context, CEEvent_s event);

typedef int CEApiResult_t;

static const CEApiResult_t CEApiResultSuccess = 0;
static const CEApiResult_t CEApiResultErrorSystemCall = 2;


size_t CEApiGetEventItemSize(void);

void * _Nullable CEApiCreate(int size);

void CEApiFree(void * _Nonnull api);

//正确的调用顺序 是先添加， 再更新读写状态监听  再removeEvent

void CEApiAddEvent(void * _Nonnull api, int fd);

#if CORE_EVENT_READ_WRITE_SEPARATION
void CEApiUpdateReadEvent(void * _Nonnull api, int fd, _Bool enable);
void CEApiUpdateWriteEvent(void * _Nonnull api, int fd, _Bool enable);
#else
void CEApiUpdateEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask);
#endif

void CEApiRemoveEvent(void * _Nonnull api, int fd);



//timeout 单位是微妙， callback 回调中的status 可能多于需要监听的， 应用需要自己做filter
void CEApiPoll(void * _Nonnull api, uint64_t timeout, void * _Nonnull context, CEBaseApiPollCallback_f _Nonnull callback);

CEFileEventMask_es CEApiWait(int fd, CEFileEventMask_es mask, int milliseconds);

void CEApiWakeUp(void * _Nonnull api);

char * _Nonnull CEApiName(void);


#if defined(__cplusplus)
}
#endif

#endif /* CEBaseApi_h */
