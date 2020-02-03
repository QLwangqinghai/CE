//
//  CEBaseType.h
//  CoreEvent
//
//  Created by wangqinghai on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#ifndef CEBaseType_h
#define CEBaseType_h

#include <CoreSystem/CoreSystem.h>

#if __APPLE__
#import <os/lock.h>
#endif


struct _CEPool;
typedef struct _CEPool CEPool_s;
typedef CEPool_s * CEPoolRef;

struct _CETimeEvent;
typedef struct _CETimeEvent CETimeEvent_s;
typedef CETimeEvent_s * CETimeEventRef;



struct _CEFileEvent;
typedef struct _CEFileEvent CEFileEvent_s;
typedef CEFileEvent_s * CEFileEventRef;


static int CEFileDescriptionInvalid = INT_MIN;


/* File event structure */
typedef struct _CEFileEventInfo {
    uint32_t readable: 1;
    uint32_t writable: 1;
    uint32_t isReadTimeout: 1;
    uint32_t isWriteTimeout: 1;
    uint32_t mask: 2;
    uint32_t xxx: 26;
} CEFileEventInfo_s;

typedef struct _CEFileDescription {
    uint32_t tag;
    int fd;
} CEFileDescription_s;


typedef void (*CEFileEventHandler_f)(CEPoolRef _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventInfo_s * _Nonnull clientData, CEFileEventInfo_s eventInfo);
typedef struct _CEFileEventHandler {
    CEFileEventHandler_f _Nonnull func;
} CEFileEventHandler_s;




typedef struct _CEThreadWaiter {
    _Atomic(uintptr_t) whoWakeUp;
#if __APPLE__
    char name[1024];
#endif
    sem_t * _Nonnull lock;
    
    sem_t lockValue;//private
} CEThreadWaiter_s;


static const uint64_t CETimeBetweenFrames = 125000;//每秒钟8帧

typedef int CEResult_t;

typedef uint32_t CEFileEventMask_es;

extern const CEFileEventMask_es CEFileEventMaskNone;
extern const CEFileEventMask_es CEFileEventMaskReadable;
extern const CEFileEventMask_es CEFileEventMaskWritable;
extern const CEFileEventMask_es CEFileEventMaskReadWritable;






typedef struct _CETimeDescription {
    uint32_t tag;
    uint32_t id;
} CETimeDescription_s;

//return true for default， return false for cancel
typedef _Bool (*CETimeEventHandler_f)(CEPoolRef _Nonnull eventPool, CETimeDescription_s td, void * _Nullable context);
typedef void (*CETimeEventClearContextHandler_f)(CEPoolRef _Nonnull eventPool, CETimeDescription_s td, void * _Nullable context);
typedef void (*CEStateChangeHandler_f)(CEPoolRef _Nonnull eventPool);




//当前模式下会在上次timer 执行的时间基础上 添加 间隔时间 作为下次触发的时间， 比如  触发时间为 10000， 间隔时间 为 1000， 当前执行的时间 为 10234， 则下次触发 在 11234 之后
#define CETimeEventRepeatModeAfter 1


//当前模式下会在上次timer 应该触发的时间基础上 添加 间隔时间 作为下次触发的时间， 比如  触发时间为 10000， 间隔时间 为 1000， 当前执行的时间 为 10234， 则下次触发 在 11000 之后
#define CETimeEventRepeatModeNone 0


typedef struct _CEThread {
    pthread_t _Nullable thread;
} CEThread_s;

#define CETimeEventStatesWait 0
#define CETimeEventStatesExecuting 0x1
#define CETimeEventStatesCancel 0x2
#define CETimeEventStatesFinished 0x3

#define CETimeEventLeewayMax 0x1FFFFull
#define CETimeEventIntervalMax 0x3FFFFFFFFFFull



static const uint32_t CETimeEventQueueIndexMax = 0xFFFEul;
static const uint32_t CETimeEventQueueIndexInvalid = 0xFFFFul;

static const uint32_t CETimeEventIdMax = 0xFFFEul;
static const uint32_t CETimeEventIdInvalid = 0xFFFFul;

static const uint64_t CETimeEventIdAllUsed = UINT64_MAX;









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

typedef void (*CEClosureExecute_f)(uintptr_t closure, const void * _Nullable params);
typedef void (*CEClosureClearData_f)(uintptr_t closure, const void * _Nullable data, size_t dataSize);


//typedef struct _CEBlockStorage {
//    //    _Atomic(uintptr_t) execute;
//    //    _Atomic(uintptr_t) executeResult;
//    //    _Atomic(uintptr_t) dealloc;
//
//    CEClosureExecute_f _Nonnull execute;
//    CEClosureExecute_f _Nonnull executeResult;
//    CEClosureClearData_f _Nullable dealloc;
//    _Atomic(uint_fast32_t) contextSize;
//    _Atomic(uint_fast32_t) resultSize;
//    CEAtomicMemoryBlock_t context[0];
//} CEBlockStorage_s;

typedef struct _CEClosureType {
    CEClosureExecute_f _Nonnull execute;
    CEClosureClearData_f _Nullable clearData;
    uint32_t dataSize;
    uint32_t paramsSize;
    uint32_t resultSize;
} CEClosureType_s;

//typedef struct _CEClosure {
//    CEThreadWaiter_s * _Nullable waiter;
//    CEClosureExecute_f _Nonnull execute;
//    CEClosureClearData_f _Nullable clearData;
//    uint32_t dataSize;
//    uint32_t paramsSize;
//    uint32_t resultSize;
//} CEClosure_s;

typedef struct _CEClosure {
    const CEClosureType_s * _Nonnull type;
    const CEThreadWaiter_s * _Nullable waiter;
    
} CEClosure_s;

typedef struct _CEClosureInfo {
    CEClosure_s closure;
    
} CEClosureInfo_s;

#define CEBlockQueuePageSize 2046
typedef struct _CEBlockQueuePage {
    struct _CEBlockQueuePage * _Nullable next;
    size_t used;
    uintptr_t buffer[CEBlockQueuePageSize];
} CEBlockQueuePage_s;



struct _CEBlockQueue {
    CEBlockQueuePage_s * _Nullable begin;
    CEBlockQueuePage_s * _Nullable end;
};

typedef struct _CEBlockQueue CEBlockQueue_s;


#define CEFileEventTimeoutMillisecondMax 2047875


typedef uint32_t CEPoolProgress_t;

static const CEPoolProgress_t CEPoolProgressDoBlock0 = 1;
static const CEPoolProgress_t CEPoolProgressDoTimer0 = 2;
static const CEPoolProgress_t CEPoolProgressDoCheckSource = 3;
static const CEPoolProgress_t CEPoolProgressDoBlock1 = 4;
static const CEPoolProgress_t CEPoolProgressDoTimer1 = 5;
static const CEPoolProgress_t CEPoolProgressDoSource = 6;
static const CEPoolProgress_t CEPoolProgressDoCheckSourceTimeout = 7;
static const CEPoolProgress_t CEPoolProgressDoSourceTimeout = 8;


typedef uint32_t CEPoolObserverMask_t;

static const CEPoolObserverMask_t ERunLoopObserverMaskFrameBegin = 0x80000000;
static const CEPoolObserverMask_t ERunLoopObserverMaskFrameFinish = 0x40000000;

static const CEPoolObserverMask_t ERunLoopObserverMaskBeforeDoBlock0 = 0x1;
static const CEPoolObserverMask_t ERunLoopObserverMaskAfterDoBlock0 = 0x2;
static const CEPoolObserverMask_t ERunLoopObserverMaskBeforeDoTimer0 = 0x4;
static const CEPoolObserverMask_t ERunLoopObserverMaskAfterDoTimer0 = 0x8;
static const CEPoolObserverMask_t ERunLoopObserverMaskBeforeDoCheckSource = 0x10;
static const CEPoolObserverMask_t ERunLoopObserverMaskAfterDoCheckSource = 0x20;
static const CEPoolObserverMask_t ERunLoopObserverMaskBeforeDoBlock1 = 0x40;
static const CEPoolObserverMask_t ERunLoopObserverMaskAfterDoBlock1 = 0x80;
static const CEPoolObserverMask_t ERunLoopObserverMaskBeforeDoTimer1 = 0x100;
static const CEPoolObserverMask_t ERunLoopObserverMaskAfterDoTimer1 = 0x200;
static const CEPoolObserverMask_t ERunLoopObserverMaskBeforeDoSource = 0x400;
static const CEPoolObserverMask_t ERunLoopObserverMaskAfterDoSource = 0x800;
static const CEPoolObserverMask_t ERunLoopObserverMaskBeforeCheckSourceTimeout = 0x1000;
static const CEPoolObserverMask_t ERunLoopObserverMaskAfterCheckSourceTimeout = 0x2000;
static const CEPoolObserverMask_t ERunLoopObserverMaskBeforeDoSourceTimeout = 0x4000;
static const CEPoolObserverMask_t ERunLoopObserverMaskAfterDoSourceTimeout = 0x8000;


typedef void (*CERunLoopObseverHandler_f)(CEPool_s * _Nonnull eventLoop, CEPoolObserverMask_t mask, void * _Nullable context);

/* State of an event based program */
typedef struct _CERunLoopObserver {
    CEPoolObserverMask_t mask;
    CERunLoopObseverHandler_f _Nonnull handler;
    void * _Nullable context;
} CERunLoopObserver_s;


static const CEResult_t CEResultSuccess = 0;
static const CEResult_t CEResultErrorParams = 1;
static const CEResult_t CEResultErrorSystemCall = 2;
static const CEResult_t CEResultErrorFileDescription = 3;
static const CEResult_t CEResultErrorSourceReinit = 4;
static const CEResult_t CEResultErrorShouldRemoveSource = 5;
static const CEResult_t CEResultFailureResize = 6;
static const CEResult_t CEResultErrorFileDescriptionInvalid = 7;
static const CEResult_t CEResultErrorTimeDescriptionInvalid = 8;
static const CEResult_t CEResultErrorTimeDescriptionNotEnough = 9;

#endif /* CEBaseType_h */
