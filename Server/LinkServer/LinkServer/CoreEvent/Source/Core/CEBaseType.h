//
//  CEBaseType.h
//  CoreEvent
//
//  Created by wangqinghai on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#ifndef CEBaseType_h
#define CEBaseType_h

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

//#pragma pack(push, 2)
//typedef struct {
//    uint64_t fd: 23;
//    uint64_t sequence: 41;
//} CEFileId;
//#pragma pack(pop)

typedef uint64_t CEFileId;

static const CEMicrosecondTime CEFrameIntervalPer32 = 61 * 40;//每32个链接 时间间隔减少 (1000000 - 0xF4200)
static const CEMicrosecondTime CEFrameIntervalDefault = 125000 * 40;

typedef int CEResult_t;

typedef uint32_t CEFileEventMask_es;

extern const CEFileEventMask_es CEFileEventMaskNone;
extern const CEFileEventMask_es CEFileEventMaskReadable;
extern const CEFileEventMask_es CEFileEventMaskWritable;
extern const CEFileEventMask_es CEFileEventMaskReadWritable;



//return true for default， return false for cancel
typedef void (*CEStateChangeHandler_f)(CEPollPtr _Nonnull poll);


typedef struct _CEThread {
    pthread_t _Nullable thread;
} CEThread_s;

#define CETimeEventStatesWait 0
#define CETimeEventStatesExecuting 0x1
#define CETimeEventStatesFinished 0x2

#define CETimeEventLeewayMax 0x1FFFFull
#define CETimeEventIntervalMax 0x3FFFFFFFFFFull



static const uint32_t CETimeEventQueueIndexMax = 0xFFFEul;
static const uint32_t CETimeEventQueueIndexInvalid = 0xFFFFul;


//延迟执行一次
#define CETimeEventModeDelay 0

//任务两次执行时间间隔是任务的开始点
#define CETimeEventModeRepeatFixedRate 1

//间隔是前次任务的结束时间 + duration
#define CETimeEventModeRepeatFixedDelay 2

/* Time event structure */
typedef struct _CETimeEvent {
    uint64_t mode: 2;
    uint64_t isCanceled: 1;
    uint64_t states: 2;//必须为0
    uint64_t qIndex: 16;
    uint64_t duration: 43;//间隔时间 最大 101 天
    CEMicrosecondTime when; //什么时候触发
    CCClosureRef _Nonnull closure;
} CETimeEvent_s;


typedef CCRef CETimeEventRef;

typedef struct _CETimeEventPage {
    CETimeEventRef _Nullable buffer[4096];
} CETimeEventPage_s;

#define CETimeEventQueueSizeMax 0x10000
#define CETimeEventQueuePagesSize 16
typedef struct _CETimeEventQueue {
    uint32_t capacity;
    uint32_t count;
    CETimeEventPage_s * _Nullable pages[CETimeEventQueuePagesSize];

    
//    CETimeEvent_s * _Nullable * _Nonnull buffer;
} CETimeEventQueue_s;


#if __LLP64__ || __LP64__
#define CEAtomicMemoryBlockSizeUse64 1
#else
#define CEAtomicMemoryBlockSizeUse64 0
#endif

#if CEAtomicMemoryBlockSizeUse64

typedef uint64_t CEMemoryBlock_t;
typedef _Atomic(uint_fast64_t) CEAtomicMemoryBlock_t;
typedef uint64_t CEAtomicMemoryBlockBridgeType_t;
#define CEMemoryBlockSize (8)
#define CEAtomicMemoryBlockSize (8)
#define CEAtomicMemoryBlockSizeShift (3)

#else

typedef uint32_t CEMemoryBlock_t;
typedef _Atomic(uint_fast32_t) CEAtomicMemoryBlock_t;
typedef uint32_t CEAtomicMemoryBlockBridgeType_t;
#define CEMemoryBlockSize (4)
#define CEAtomicMemoryBlockSize (4)
#define CEAtomicMemoryBlockSizeShift 2

#endif


static inline size_t CEMemoryBlockCountFromSize(size_t size) {
    return (size <= CEMemoryBlockSize) ? CEMemoryBlockSize
    : (((size + CEMemoryBlockSize - 1) >> CEAtomicMemoryBlockSizeShift) << CEAtomicMemoryBlockSizeShift);
}
static inline size_t CEAtomicMemoryBlockCountFromSize(size_t size) {
    size_t result = size / CEAtomicMemoryBlockSize;
    if (size % CEAtomicMemoryBlockSize != 0) {
        result += 1;
    }
    return result;
}
static inline void CEAtomicMemoryStore(CEAtomicMemoryBlock_t * _Nonnull dst, const void * _Nonnull src, size_t srcSize) {

    size_t count = srcSize / CEAtomicMemoryBlockSize;
    const CEAtomicMemoryBlockBridgeType_t * tmp = (const CEAtomicMemoryBlockBridgeType_t *)src;
    for (size_t i=0; i<count; i++) {
        atomic_store(dst, *tmp);
        dst ++;
        tmp ++;
    }
    
    size_t remain = srcSize % CEAtomicMemoryBlockSize;
    if (remain != 0) {
        CEAtomicMemoryBlockBridgeType_t t = 0;
        memcpy(&t, tmp, remain);
        atomic_store(dst, t);
    }
}
static inline void CEAtomicMemoryLoad(CEAtomicMemoryBlock_t * _Nonnull src, void * _Nonnull dst, size_t dstSize) {
    
    size_t count = dstSize / CEAtomicMemoryBlockSize;
    CEAtomicMemoryBlockBridgeType_t * tmp = (CEAtomicMemoryBlockBridgeType_t *)dst;
    for (size_t i=0; i<count; i++) {
        *tmp = atomic_load(src);
        src ++;
        tmp ++;
    }
    
    size_t remain = dstSize % CEAtomicMemoryBlockSize;
    if (remain != 0) {
        CEAtomicMemoryBlockBridgeType_t t = atomic_load(src);
        memcpy(tmp, &t, remain);
    }
}
static inline void CEAtomicMemorySet(CEAtomicMemoryBlock_t * _Nonnull dst, CEAtomicMemoryBlockBridgeType_t src, size_t size) {
    size_t count = size / CEAtomicMemoryBlockSize;
    for (size_t i=0; i<count; i++) {
        atomic_store(dst, src);
        dst ++;
    }
    size_t remain = size % CEAtomicMemoryBlockSize;
    if (remain != 0) {
        atomic_store(dst, src);
    }
}

#define CEBlockQueuePageSize 2046
typedef struct _CEBlockQueuePage {
    struct _CEBlockQueuePage * _Nullable next;
    size_t used;
    CCClosureRef _Nonnull buffer[CEBlockQueuePageSize];
} CEBlockQueuePage_s;



struct _CEBlockQueue {
    CEBlockQueuePage_s * _Nullable begin;
    CEBlockQueuePage_s * _Nullable end;
};

typedef struct _CEBlockQueue CEBlockQueue_s;


#define CEFileEventTimeoutMax 2047875

typedef uint32_t CEPollProgress_t;

static const CEPollProgress_t CEPollProgressDoSource = 0;
static const CEPollProgress_t CEPollProgressDoSourceTimeout = 1;
static const CEPollProgress_t CEPollProgressDoBlock = 2;
static const CEPollProgress_t CEPollProgressDoTimer = 3;

typedef uint32_t CEPollObserverMask_t;

static const CEPollObserverMask_t CEPollObserverMaskBeforeDoSource = 0x1;
static const CEPollObserverMask_t CEPollObserverMaskBeforeSourceTimeout = 0x2;
static const CEPollObserverMask_t CEPollObserverMaskAfterDoBlock = 0x4;
static const CEPollObserverMask_t CEPollObserverMaskBeforeDoTimer = 0x8;


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

#endif /* CEBaseType_h */
