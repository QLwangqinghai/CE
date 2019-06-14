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
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <errno.h>
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

//custom
#include "CETime.h"
#include "CELog.h"



static const uint64_t CETimeBetweenFrames = 125000;//每秒钟8帧

static inline uint64_t CEGetMicrosecondsTime(void) {
    return CESystemBootInterval();
}

typedef int CEResult_t;

typedef uint32_t CEFileEventMask_es;

extern const CEFileEventMask_es CEFileEventMaskNone;
extern const CEFileEventMask_es CEFileEventMaskReadable;
extern const CEFileEventMask_es CEFileEventMaskWritable;
extern const CEFileEventMask_es CEFileEventMaskReadWritable;


typedef CEFileEventMask_es (*CEApiPoolGetEventMask_f)(void * _Nullable context, void * _Nonnull api, int fd);

typedef void (*CEApiPoolCallback_f)(void * _Nullable context, void * _Nonnull api);
typedef void (*CEApiPoolFileEventCallback_f)(void * _Nullable context, void * _Nonnull api, int fd, CEFileEventMask_es mask);

typedef struct _CEApiPoolCallback {
    CEApiPoolFileEventCallback_f _Nonnull fileEventCallback;
    CEApiPoolCallback_f _Nonnull pipeCallback;
} CEApiPoolCallback_s;

typedef struct _CEFileDescription {
    uint32_t tag;
    int fd;
} CEFileDescription_s;

struct _CERunLoop;
typedef struct _CERunLoop CERunLoop_s;


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


typedef void (*CEFileEventHandler_f)(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventInfo_s * _Nonnull clientData, CEFileEventInfo_s eventInfo);


typedef struct _CETimeDescription {
    uint32_t tag;
    uint32_t id;
} CETimeDescription_s;

//return true for default， return false for cancel
typedef _Bool (*CETimeEventHandler_f)(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td, void * _Nullable context);
typedef void (*CETimeEventClearContextHandler_f)(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td, void * _Nullable context);
typedef void (*CEStateChangeHandler_f)(CERunLoop_s * _Nonnull eventLoop);
typedef struct _CEFileEventHandler {
    CEFileEventHandler_f _Nonnull func;
} CEFileEventHandler_s;


/* File event structure 8B*/
typedef struct _CEFileEventTimer {
    int prev;
    int next;
} CEFileEventTimer_s;

/* File event structure 32b-32B 64b-40B */
typedef struct _CEFileEvent {
    uint32_t readable: 1;
    uint32_t writable: 1;
    uint32_t mask: 2;
    uint32_t isReadTimeout: 1;
    uint32_t isWriteTimeout: 1;
    uint32_t readTimeoutInterval: 13;//单位为(1/8)秒
    uint32_t writeTimeoutInterval: 13;//单位为(1/8)秒
    uint32_t fdTag;
    
    CEFileEventTimer_s readTimer;
    CEFileEventTimer_s writeTimer;

    CEFileEventHandler_f _Nonnull handler;
    void * _Nullable clientData;
} CEFileEvent_s;



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


/* Time event structure */
typedef struct _CETimeEvent {
    uint64_t isStatic: 1;
    uint64_t repeat: 1;
    uint64_t repeatMode: 1;
    uint64_t states: 2;//必须为0
    uint64_t leeway: 17;//偏差 单位 微妙 最大值 0x1FFFF
    uint64_t interval: 42;//间隔时间 单位微妙  最大 1221小时 (50天)
    CETimeEventHandler_f _Nonnull execute;
    CETimeEventClearContextHandler_f _Nullable clearContext;
    void * _Nullable context;
    uint64_t when; /* microseconds 微妙 什么时候触发 */
    uint32_t tdTag;
    uint32_t tdId: 16;
    uint32_t qIndex: 16;
} CETimeEvent_s;

static inline _Bool CETimeEventIsRunning(CETimeEvent_s * _Nonnull event) {
    return event->qIndex <= CETimeEventQueueIndexMax;
}


static inline CETimeDescription_s CETimeEventGetTd(CETimeEvent_s * _Nonnull event) {
    CETimeDescription_s td = {.id = event->tdId, .tag = event->tdTag};
    return td;
}
static inline void CETimeEventSetTd(CETimeEvent_s * _Nonnull event, CETimeDescription_s td) {
    event->tdTag = td.tag;
    event->tdId = td.id;
}


/* Time event structure */
typedef struct _CETimeEventContainer {
    CETimeEvent_s te;
    struct _CETimeEventContainer * _Nullable left;
    struct _CETimeEventContainer * _Nullable right;
} CETimeEventContainer_s;




static inline uint64_t CETimeEventGetLeeway(CETimeEvent_s * _Nonnull event) {
    return event->leeway;
}

static inline void CETimeEventSetLeeway(CETimeEvent_s * _Nonnull event, uint64_t leeway) {
    if (leeway > CETimeEventLeewayMax) {
        leeway = CETimeEventLeewayMax;
    } else if (leeway <= 1) {
        leeway = 1;
    } else {
        leeway = leeway;
    }
    event->leeway = leeway;
}

//最大允许64k
typedef struct _CETimeDescriptionSet {
    uint64_t n2[1024];
    uint64_t n1[16];
    uint16_t n0;
    uint16_t xxx;
    uint32_t usedCount;
} CETimeDescriptionSet_s;

//需要改成avl tree
typedef struct _CETimeEventTable {
    CETimeEventContainer_s * _Nullable buffer[4096];
} CETimeEventTable_s;

static inline CETimeEventContainer_s * _Nullable CETimeEventTableGetItem(CETimeEventTable_s * _Nonnull table, uint32_t key) {
    uint32_t index = key & 0xFFF;
    CETimeEventContainer_s * iter = table->buffer[index];
    while (iter) {
        if (iter->te.tdId == key) {
            return iter;
        }
        iter = iter->right;
    }
    return NULL;
}

static inline void CETimeEventTableRemoveItem(CETimeEventTable_s * _Nonnull table, CETimeEventContainer_s * _Nonnull item) {
    uint32_t index = item->te.tdId & 0xFFF;
    CETimeEventContainer_s * iter = item;
    if (iter->left) {
        iter->left->right = iter->right;
    } else {
        table->buffer[index] = iter->right;
    }
    if (iter->right) {
        iter->right->left = iter->left;
    }
    iter->left = NULL;
    iter->right = NULL;
}
static inline void CETimeEventTableAddItem(CETimeEventTable_s * _Nonnull table, CETimeEventContainer_s * _Nonnull item) {
    uint32_t index = item->te.tdId & 0xFFF;
    
    CETimeEventContainer_s * old = table->buffer[index];
    if (old) {
        old->left = item;
    }
    item->right = old;
    table->buffer[index] = item;
}

typedef struct _CETimeEventQueue {
    CETimeEvent_s * _Nullable * _Nonnull buffer;
    uint32_t capacity;
    uint32_t count;
} CETimeEventQueue_s;

typedef struct _CETimeEventManager {
    CETimeDescriptionSet_s tdSet;
    CETimeEvent_s baseTable[4096];
    CETimeEventTable_s table;
    
    uint64_t size;
    uint64_t unused;
    uint32_t sequence;
    
    CETimeEvent_s * _Nullable executingTimeEvent;
    CETimeEventQueue_s timerQueue;
} CETimeEventManager_s;

static inline uint32_t CETimeEventManagerGetIndex64(uint64_t n) {
    if (UINT64_MAX == n) {
        return UINT32_MAX;
    }
    
    uint32_t i = 0;
    uint32_t j = 63;
    uint32_t n1 = (uint32_t)(n >> 32);
    if (n1 == UINT32_MAX) {
        i += 32;
        n1 = (uint32_t)(n & UINT32_MAX);
    } else {
        j -= 32;
    }
    uint16_t n2 = (uint16_t)(n1 >> 16);
    if (n2 == UINT16_MAX) {
        i += 16;
        n2 = (uint16_t)(n1 & UINT16_MAX);
    } else {
        j -= 16;
    }
    uint8_t n3 = (uint8_t)(n2 >> 8);
    if (n3 == UINT8_MAX) {
        i += 8;
        n3 = (uint8_t)(n2 & UINT8_MAX);
    } else {
        j -= 8;
    }
    uint8_t n4 = (n3 >> 4);
    if (n4 == 0xf) {
        i += 4;
        n4 = (n3 & 0xf);
    } else {
        j -= 4;
    }
    uint8_t n5 = (n4 >> 2);
    if (n5 == 0x3) {
        i += 2;
        n5 = (n4 & 0x3);
    } else {
        j -= 2;
    }
    uint8_t n6 = (n5 >> 1);
    if (n6 == 0x1) {
        return j;
    } else {
        return i;
    }
}
static inline uint32_t CETimeEventManagerGetIndex32(uint32_t n) {
    if (UINT32_MAX == n) {
        return UINT32_MAX;
    }
    uint32_t i = 0;
    uint32_t j = 31;
    uint32_t n1 = n;
    uint16_t n2 = (uint16_t)(n1 >> 16);
    if (n2 == UINT16_MAX) {
        i += 16;
        n2 = (uint16_t)(n1 & UINT16_MAX);
    } else {
        j -= 16;
    }
    uint8_t n3 = (uint8_t)(n2 >> 8);
    if (n3 == UINT8_MAX) {
        i += 8;
        n3 = (uint8_t)(n2 & UINT8_MAX);
    } else {
        j -= 8;
    }
    uint8_t n4 = (n3 >> 4);
    if (n4 == 0xf) {
        i += 4;
        n4 = (n3 & 0xf);
    } else {
        j -= 4;
    }
    uint8_t n5 = (n4 >> 2);
    if (n5 == 0x3) {
        i += 2;
        n5 = (n4 & 0x3);
    } else {
        j -= 2;
    }
    uint8_t n6 = (n5 >> 1);
    if (n6 == 0x1) {
        return j;
    } else {
        return i;
    }
}
static inline uint32_t CETimeEventManagerGetIndex16(uint16_t n) {
    if (UINT16_MAX == n) {
        return UINT32_MAX;
    }
    uint32_t i = 0;
    uint32_t j = 15;
    uint16_t n2 = n;
    uint8_t n3 = (uint8_t)(n2 >> 8);
    if (n3 == UINT8_MAX) {
        i += 8;
        n3 = (uint8_t)(n2 & UINT8_MAX);
    } else {
        j -= 8;
    }
    uint8_t n4 = (n3 >> 4);
    if (n4 == 0xf) {
        i += 4;
        n4 = (n3 & 0xf);
    } else {
        j -= 4;
    }
    uint8_t n5 = (n4 >> 2);
    if (n5 == 0x3) {
        i += 2;
        n5 = (n4 & 0x3);
    } else {
        j -= 2;
    }
    uint8_t n6 = (n5 >> 1);
    if (n6 == 0x1) {
        return j;
    } else {
        return i;
    }
}

static inline uint32_t CETimeEventManagerMakeId(CETimeEventManager_s * _Nonnull manager) {
    uint64_t mask = 0x8000000000000000ull;
    
    uint32_t id = CETimeEventIdInvalid;
    uint16_t n0 = manager->tdSet.n0;
    
    uint32_t offset0 = CETimeEventManagerGetIndex16(n0);
    if (offset0 > 15) {
        return id;
    }
    
    uint32_t index1 = offset0;
    uint64_t n1 = manager->tdSet.n1[index1];
    uint32_t offset1 = CETimeEventManagerGetIndex64(n1);
    if (offset1 > 63) {
        abort();
    }

    uint32_t index2 = offset1 + (index1 << 6);
    uint64_t n2 = manager->tdSet.n2[index2];
    uint32_t offset2 = CETimeEventManagerGetIndex64(n2);
    if (offset2 > 63) {
        abort();
    }
    n2 = n2 | (mask >> offset2);
    manager->tdSet.n2[index2] = n2;
    if (CETimeEventIdAllUsed == n2) {
        n1 = n1 | (mask >> offset1);
        manager->tdSet.n1[index1] = n1;
        
        if (CETimeEventIdAllUsed == n1) {
            uint16_t m16 = 0x8000;
            n0 = n0 | (m16 >> offset0);
            manager->tdSet.n0 = n0;
        }
    }
    id = (uint32_t)((index2 << 6) + offset2);

    manager->tdSet.usedCount += 1;
    return id;
}

static inline void CETimeEventManagerFreeId(CETimeEventManager_s * _Nonnull manager, uint32_t id) {
    assert(id <= CETimeEventIdMax);
    uint64_t mask = 0x8000000000000000ull;
    uint16_t m16 = 0x8000;

    uint16_t n0 = manager->tdSet.n0;
    uint32_t offset0 = id >> 12;
    
    uint32_t index1 = offset0;
    uint32_t offset1 = (id >> 6) & 0x3F;
    uint64_t n1 = manager->tdSet.n1[index1];

    uint32_t index2 = offset1 + (index1 << 6);
    uint32_t offset2 = id & 0x3F;;
    uint64_t n2 = manager->tdSet.n2[index2];
    
    assert( n2 & (mask >> offset2));
    n2 = n2 & (~(mask >> offset2));
    manager->tdSet.n2[index2] = n2;
    
    n1 = n1 & (~(mask >> offset1));
    manager->tdSet.n1[index1] = n1;

    n0 = n0 & (~(m16 >> offset0));
    manager->tdSet.n0 = n0;
    manager->tdSet.usedCount -= 1;
}

static inline _Bool CETimeEventManagerMakeTd(CETimeEventManager_s * _Nonnull manager, CETimeDescription_s * _Nonnull tdPtr) {
    uint32_t id = CETimeEventManagerMakeId(manager);
    if (id > CETimeEventIdMax) {
        return 0;
    }
    manager->sequence += 1;
    tdPtr->id = id;
    tdPtr->tag = manager->sequence;
    return 1;
}


typedef struct _CETimeEventQueueIterator {
    CETimeEventManager_s * _Nonnull manager;
    CETimeEventQueue_s * _Nonnull queue;
    CETimeEvent_s * _Nullable current;
    CETimeEvent_s * _Nullable prev;
    CETimeEvent_s * _Nullable next;

    uint32_t currentId;
    uint32_t prevId;
    uint32_t nextId;
} CETimeEventQueueIterator_s;

static inline CETimeEvent_s * _Nullable CETimeEventManagerGetEventById(CETimeEventManager_s * _Nonnull manager, uint32_t id) {
    if (id > CETimeEventIdMax) {
        return NULL;
    }
    
    if (id < 4096) {
        uint64_t t = manager->tdSet.n2[id >> 6];
        uint64_t mask = 0x8000000000000000ull;
        uint64_t offset = id & 0x3F;
        mask = mask >> offset;
        
        if (t & mask) {
            return &(manager->baseTable[id]);
        } else {
            return NULL;
        }
    } else {
        return &(CETimeEventTableGetItem(&(manager->table), id)->te);
    }
}

static inline CETimeEvent_s * _Nullable CETimeEventManagerGetEvent(CETimeEventManager_s * _Nonnull manager, CETimeDescription_s td) {
    CETimeEvent_s * event = CETimeEventManagerGetEventById(manager, td.id);
    if (event->tdTag == td.tag) {
        return event;
    }
    return NULL;
}


typedef CEFileDescription_s CEFiredEvent_s;



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


//typedef struct _CEClosure {
//    CEThreadWaiter_s * _Nullable waiter;
//    CEClosureExecute_f _Nonnull execute;
//    CEClosureClearData_f _Nullable clearData;
//    uint32_t dataSize;
//    uint32_t paramsSize;
//    uint32_t resultSize;
//} CEClosure_s;



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


#define CERunLoopFileTimerPageSize 0x4000
#define CEFileEventTimeoutMillisecondMax 2047875


typedef uint32_t CERunLoopProgress_t;

static const CERunLoopProgress_t CERunLoopProgressDoBlock0 = 1;
static const CERunLoopProgress_t CERunLoopProgressDoTimer0 = 2;
static const CERunLoopProgress_t CERunLoopProgressDoCheckSource = 3;
static const CERunLoopProgress_t CERunLoopProgressDoBlock1 = 4;
static const CERunLoopProgress_t CERunLoopProgressDoTimer1 = 5;
static const CERunLoopProgress_t CERunLoopProgressDoSource = 6;
static const CERunLoopProgress_t CERunLoopProgressDoCheckSourceTimeout = 7;
static const CERunLoopProgress_t CERunLoopProgressDoSourceTimeout = 8;


typedef uint32_t CERunLoopObserverMask_t;

static const CERunLoopObserverMask_t ERunLoopObserverMaskFrameBegin = 0x80000000;
static const CERunLoopObserverMask_t ERunLoopObserverMaskFrameFinish = 0x40000000;

static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoBlock0 = 0x1;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoBlock0 = 0x2;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoTimer0 = 0x4;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoTimer0 = 0x8;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoCheckSource = 0x10;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoCheckSource = 0x20;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoBlock1 = 0x40;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoBlock1 = 0x80;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoTimer1 = 0x100;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoTimer1 = 0x200;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoSource = 0x400;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoSource = 0x800;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeCheckSourceTimeout = 0x1000;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterCheckSourceTimeout = 0x2000;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoSourceTimeout = 0x4000;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoSourceTimeout = 0x8000;


typedef void (*CERunLoopObseverHandler_f)(CERunLoop_s * _Nonnull eventLoop, CERunLoopObserverMask_t mask, void * _Nullable context);

/* State of an event based program */
typedef struct _CERunLoopObserver {
    CERunLoopObserverMask_t mask;
    CERunLoopObseverHandler_f _Nonnull handler;
    void * _Nullable context;
} CERunLoopObserver_s;


/* State of an event based program */
struct _CERunLoop {
    _Atomic(uintptr_t) runInThread;
    _Atomic(uint_fast32_t) runningStates;// 0 is stopped , 1 running, 2 is will stopped
    CERunLoopProgress_t progress;//
    CEThread_s thread;

    int maxfd;   /* highest file descriptor currently registered */
    int maxIndex;
    unsigned int setsize; /* max number of file descriptors tracked */
    int firedCount;

#if __APPLE__
    os_unfair_lock blockQueueLock;
#else
    pthread_spinlock_t blockQueueLock;
#endif
    uint32_t blockEvent;
    uint32_t timerFiredIndex;//source timer 的 游标
    uint32_t fdTagSequence;
    uint32_t xxxx;
    CEBlockQueue_s blockQueue;
    
    CETimeEventManager_s timeEventManager;
    
    uint64_t microsecondsTime;//单位 微秒
    uint64_t fileTimerSeconds8;//单位为(1/8)秒
    
    CEFileEvent_s * _Nullable fileEventsPages[32768]; /* Registered events, 0x10000 count peer page */
    CEFiredEvent_s * _Nullable firedPages[32768]; /* Fired events, 0x10000 count peer page  */

    int readTimerPages[CERunLoopFileTimerPageSize];
    int writeTimerPages[CERunLoopFileTimerPageSize];
    
    void * _Nullable api; /* This is used for polling API specific data */
    uint32_t observerBufferSize;
    uint32_t observerBufferCount;

    CERunLoopObserver_s * _Nullable * _Nonnull observers;
};

static inline uint32_t CERunLoopNextFdTag(CERunLoop_s * _Nonnull eventLoop) {
    eventLoop->fdTagSequence ++;
    if (0 == eventLoop->fdTagSequence) {
        eventLoop->fdTagSequence ++;
    }
    return eventLoop->fdTagSequence;
}



static inline int CERunLoopSize(int size) {
    if (size <= 0x10000) {
        return 0x10000;
    } else {
        uint32_t tmp = size;
        int count = 0x10000;
        int pageCount = (int)(tmp >> 16);
        
        if ((tmp & 0xffff) != 0) {
            pageCount += 1;
        }
        return count * pageCount;
    }
}


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
