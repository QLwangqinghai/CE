//
//  CEBaseType.h
//  CoreEvent
//
//  Created by wangqinghai on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#ifndef CEBaseType_h
#define CEBaseType_h

#if defined(__cplusplus)
extern "C" {
#endif

//c
#include <CCFoundation/CCBase.h>
#include <CCFoundation/CCClosure.h>

#include <pthread.h>
#include <semaphore.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <fcntl.h>

#if __APPLE__
#import <os/lock.h>
#endif


typedef uint64_t CEMicrosecondTime;
typedef CCPtr CEPollPtr;

static const CEMicrosecondTime CEFrameIntervalPer32 = 61 * 40;//每32个链接 时间间隔减少 (1000000 - 0xF4200)
static const CEMicrosecondTime CEFrameIntervalDefault = 125000 * 40;

typedef int CEResult_t;

typedef uint32_t CEFileEventMask_es;

extern const CEFileEventMask_es CEFileEventMaskNone;
extern const CEFileEventMask_es CEFileEventMaskReadable;
extern const CEFileEventMask_es CEFileEventMaskWritable;
extern const CEFileEventMask_es CEFileEventMaskReadWritable;

typedef struct {
    int fd;
    uint32_t status: 2;
    uint32_t context: 30;
} CEFileFiredInfo;

typedef void (*CEPollFileEventCallback_f)(CCPtr _Nullable context, CEFileFiredInfo * _Nonnull infos, uint32_t count);


//return true for default， return false for cancel
typedef void (*CEStateChangeHandler_f)(CEPollPtr _Nonnull poll);


typedef struct _CEThread {
    pthread_t _Nullable thread;
} CEThread_s;

#define CETimeEventRepeatDurationMax 0xFFFFFFFFFFFull
#define CEFileHandlerInvalid 0x3FFF


static const uint32_t CETimeEventQueueIndexMax = 0xFFFEul;
static const uint32_t CETimeEventQueueIndexInvalid = 0xFFFFul;


//延迟执行一次
#define CETimeEventModeDelay 0

//任务两次执行时间间隔是任务的开始点
#define CETimeEventModeRepeatFixedRate 1

//间隔是前次任务的结束时间 + duration
#define CETimeEventModeRepeatFixedDelay 2


/* Time event structure */
typedef struct {
    CEMicrosecondTime when; //什么时候触发
    uint64_t mode: 2;
    uint64_t isCanceled: 1;
    uint64_t isFinished: 1;
    uint64_t qIndex: 16;
    uint64_t duration: 44;//间隔时间 最大 101 天
    CCClosureRef _Nonnull closure;
} CETimeEvent_s;


typedef CCRef CETimeEventRef;

typedef struct _CETimeEventPage {
    CETimeEventRef _Nullable buffer[4096];
} CETimeEventPage_s;

#define CETimeEventQueueSizeMax 0x10000
#define CETimerQueueBufferSize 0xffff
#define CETimeEventQueuePagesSize 16
typedef struct _CETimeEventQueue {
    size_t count;
    CETimeEventRef _Nonnull buffer[CETimerQueueBufferSize];
} CETimerQueue_s;


#define CEBlockQueuePageSize 2046
typedef struct _CEBlockQueuePage {
    struct _CEBlockQueuePage * _Nullable next;
    size_t used;
    CCClosureRef _Nonnull buffer[CEBlockQueuePageSize];
} CEBlockQueuePage_s;



struct _CEBlockQueue {
#if __APPLE__
    os_unfair_lock blockQueueLock;
#else
    pthread_spinlock_t blockQueueLock;
#endif
    
    CEBlockQueuePage_s * _Nullable begin;
    CEBlockQueuePage_s * _Nullable end;
};

typedef struct _CEBlockQueue CEBlockQueue_s;


#define CEFileEventTimeoutMax 2047875

typedef uint32_t CEPollProgress_t;

static const CEPollProgress_t CEPollProgressDoSource = 0;
static const CEPollProgress_t CEPollProgressDoBlock = 1;
static const CEPollProgress_t CEPollProgressDoTimer = 2;

typedef uint32_t CEPollObserverMask_t;

static const CEPollObserverMask_t CEPollObserverMaskBeforeDoSource = 0x1;
static const CEPollObserverMask_t CEPollObserverMaskAfterDoBlock = 0x2;
static const CEPollObserverMask_t CEPollObserverMaskBeforeDoTimer = 0x4;


typedef void (*CEPollObseverHandler_f)(CEPollPtr _Nonnull poll, CEPollObserverMask_t mask, void * _Nullable context);

/* State of an event based program */
typedef struct _CERunLoopObserver {
    CEPollObserverMask_t mask;
    CEPollObseverHandler_f _Nonnull handler;
    CCPtr _Nullable context;
} CEPollObserver_s;


static const CEResult_t CEResultSuccess = 0;
static const CEResult_t CEResultErrorParams = 1;
static const CEResult_t CEResultErrorSystemCall = 2;
static const CEResult_t CEResultErrorFileId = 3;
static const CEResult_t CEResultErrorSourceReinit = 4;
static const CEResult_t CEResultErrorShouldRemoveSource = 5;
static const CEResult_t CEResultFailureResize = 6;
static const CEResult_t CEResultErrorFileIdInvalid = 7;
static const CEResult_t CEResultErrorTimeDescriptionInvalid = 8;
static const CEResult_t CEResultErrorTimeDescriptionNotEnough = 9;

#if defined(__cplusplus)
}
#endif

#endif /* CEBaseType_h */
