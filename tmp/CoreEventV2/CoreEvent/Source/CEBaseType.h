//
//  CEBaseType.h
//  CoreEvent
//
//  Created by CaiLianfeng on 2018/10/23.
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
    CEApiPoolCallback_f _Nonnull timerCallback;
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

//return true for default， return false for cancel
typedef _Bool (*CETimeEventHandler_f)(CERunLoop_s * _Nonnull eventLoop, uintptr_t id, void * _Nullable context);
typedef void (*CETimeEventClearContextHandler_f)(CERunLoop_s * _Nonnull eventLoop, uintptr_t id, void * _Nullable context);
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

//typedef struct _CETimeEventInfo {
//    uint64_t id; /* time event identifier. */
//    uint64_t repeat: 1;
//    uint64_t repeatMode: 1;
//    uint64_t states: 2;//必须为0
//    uint64_t leeway: 17;//偏差 单位 微妙 最大值 0x1FFFF
//    uint64_t interval: 43;//间隔时间 单位微妙  最大 2443小时 (101天)
//    CETimeEventHandler_f _Nonnull execute;
//    CETimeEventClearContextHandler_f _Nullable dealloc;
//    void * _Nullable clientData;
//} CETimeEventInfo_s;

/* Time event structure */
typedef struct _CETimeEvent {
    uint64_t repeat: 1;
    uint64_t repeatMode: 1;
    uint64_t states: 2;//必须为0
    uint64_t leeway: 17;//偏差 单位 微妙 最大值 0x1FFFF
    uint64_t running: 1;
    uint64_t interval: 42;//间隔时间 单位微妙  最大 1221小时 (50天)
    CETimeEventHandler_f _Nonnull execute;
    CETimeEventClearContextHandler_f _Nullable clearContext;
    void * _Nullable context;
    uint64_t when; /* microseconds 微妙 什么时候触发 */
    uintptr_t owner;
    struct _CETimeEvent * _Nullable prev;
    struct _CETimeEvent * _Nullable next;
} CETimeEvent_s;

typedef CEFileDescription_s CEFiredEvent_s;

typedef void (*CEBlockExecute_f)(CERunLoop_s * _Nonnull eventLoop, void * _Nullable context);
typedef void (*CEBlockDealloc_f)(CERunLoop_s * _Nonnull eventLoop, void * _Nullable context);

struct _CEBlock {
    struct _CEBlock * _Nullable next;
    
    CEBlockExecute_f _Nonnull execute;
    CEBlockDealloc_f _Nullable dealloc;
    void * _Nullable context;
};

typedef struct _CEBlock CEBlock_s;


struct _CEBlockQueue {
    CEBlock_s * _Nullable begin;
    CEBlock_s * _Nullable end;
};

typedef struct _CEBlockQueue CEBlockQueue_s;


typedef enum _CETimeEventQueueProgress {
    CETimeEventQueueProgressNone = 0,
    CETimeEventQueueProgressExecute = 1,
    CETimeEventQueueProgressDeinitInvalid = 2,
} CETimeEventQueueProgress_e;

typedef struct _CETimeEventQueue {
    size_t progress;
    CETimeEvent_s * _Nullable first;
    CETimeEvent_s * _Nullable last;
    CETimeEvent_s * _Nullable iter;
} CETimeEventQueue_s;


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


/* State of an event based program */
typedef struct _CERunLoopObserver {
    
} CERunLoopObserver_s;


/* State of an event based program */
struct _CERunLoop {
    _Atomic(uintptr_t) runInThread;
    _Atomic(uint_fast32_t) runningStates;// 0 is stopped , 1 running, 2 is will stopped
    CERunLoopProgress_t progress;//
    CEThread_s thread;

    int maxfd;   /* highest file descriptor currently registered */
    int maxIndex;
    int setsize; /* max number of file descriptors tracked */
    int firedCount;

#if __APPLE__
    os_unfair_lock blockQueueLock;
#else
    pthread_spinlock_t blockQueueLock;
#endif
    uint16_t blockEvent;
    uint16_t timerEvent;
    uint32_t timerFiredIndex;

    CEBlockQueue_s blockQueue;
    CETimeEventQueue_s timeEventQueue;

    uint64_t microsecondsTime;//单位 微秒
    uint64_t fileTimerSeconds8;//单位为(1/8)秒
    
    CEFileEvent_s * _Nullable fileEventsPages[32768]; /* Registered events, 0x10000 count peer page */
    CEFiredEvent_s * _Nullable firedPages[32768]; /* Fired events, 0x10000 count peer page  */

    int readTimerPages[CERunLoopFileTimerPageSize];
    int writeTimerPages[CERunLoopFileTimerPageSize];
    
    void * _Nullable api; /* This is used for polling API specific data */
    CEStateChangeHandler_f _Nullable beforesleep;
    CEStateChangeHandler_f _Nullable aftersleep;
};




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



static inline void CETimeEventLink(CETimeEvent_s * _Nonnull prev, CETimeEvent_s * _Nonnull item) {
    item->prev = prev;
    prev->next = item;
}
static inline void CETimeEventUnlink(CETimeEvent_s * _Nonnull prev, CETimeEvent_s * _Nonnull item) {
    item->prev = NULL;
    prev->next = NULL;
}
static inline void CETimeEventQueueAppend(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull item) {
    if (NULL == queue->last) {
        queue->first = item;
        queue->last = item;
    } else {
        CETimeEventLink(queue->last, item);
        queue->last = item;
    }
}
static inline void CETimeEventQueueInsert(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull item) {
    if (NULL == queue->last) {
        queue->first = item;
        queue->last = item;
    } else {
        CETimeEventLink(item, queue->first);
        queue->first = item;
    }
}
static inline void CETimeEventQueueAppendAt(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull at, CETimeEvent_s * _Nonnull item) {
    if (queue->last == at) {
        CETimeEventLink(queue->last, item);
        queue->last = item;
    } else {
        CETimeEvent_s * next = at->next;
        CETimeEventUnlink(at, next);
        CETimeEventLink(at, item);
        CETimeEventLink(item, next);
    }
}
static inline void CETimeEventQueueInsertAt(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull at, CETimeEvent_s * _Nonnull item) {
    if (queue->first == at) {
        CETimeEventLink(item, queue->first);
        queue->first = item;
    } else {
        CETimeEvent_s * prev = at->prev;
        CETimeEventUnlink(prev, at);
        CETimeEventLink(prev, item);
        CETimeEventLink(item, at);
    }
}
static inline CETimeEvent_s * _Nullable CETimeEventQueueRemoveFirst(CETimeEventQueue_s * _Nonnull queue) {
    CETimeEvent_s * _Nonnull item = queue->first;
    if (NULL != item) {
        if (queue->first == queue->last) {
            queue->first = NULL;
            queue->last = NULL;
        } else {
            queue->first = item->next;
            CETimeEventUnlink(item, item->next);
        }
    }
    return item;
}
static inline CETimeEvent_s * _Nullable CETimeEventQueueRemoveLast(CETimeEventQueue_s * _Nonnull queue) {
    CETimeEvent_s * _Nonnull item = queue->last;
    if (NULL != item) {
        if (queue->first == queue->last) {
            queue->first = NULL;
            queue->last = NULL;
        } else {
            queue->last = item->prev;
            CETimeEventUnlink(item->prev, item);
        }
    }
    return item;
}

static inline void CETimeEventQueueRemove(CETimeEventQueue_s * _Nonnull queue, CETimeEvent_s * _Nonnull item) {
    if (NULL != item->prev) {
        item->prev->next = item->next;
    } else {
        queue->first = item->next;
    }
    
    if (NULL != item->next) {
        item->next->prev = item->prev;
    } else {
        queue->last = item->prev;
    }
    item->prev = NULL;
    item->next = NULL;
}

static const CEResult_t CEResultSuccess = 0;
static const CEResult_t CEResultErrorParams = 1;
static const CEResult_t CEResultErrorSystemCall = 2;
static const CEResult_t CEResultErrorFileDescription = 3;
static const CEResult_t CEResultErrorSourceReinit = 4;
static const CEResult_t CEResultErrorShouldRemoveSource = 5;
static const CEResult_t CEResultFailureResize = 6;


#endif /* CEBaseType_h */
