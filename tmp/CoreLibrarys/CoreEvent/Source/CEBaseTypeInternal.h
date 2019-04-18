//
//  CEBaseTypeInternal.h
//  CoreEvent
//
//  Created by wangqinghai on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#ifndef CEBaseTypeInternal_h
#define CEBaseTypeInternal_h

#include <CoreEvent/CEBaseFileSourceType.h>




#pragma mark - file source

/* File event structure 8B*/
typedef struct __CEFileSourceTimer {
    int prev;
    int next;
} CEFileSourceTimer_s;

/* File event structure 32b-32B 64b-40B */
struct _CEFileEvent {
    uint32_t readable: 1;
    uint32_t writable: 1;
    uint32_t mask: 2;
    uint32_t isReadTimeout: 1;
    uint32_t isWriteTimeout: 1;
    uint32_t readTimeoutInterval: 13;//单位为(1/8)秒
    uint32_t writeTimeoutInterval: 13;//单位为(1/8)秒
    uint32_t fdTag;
    
    CEFileSourceTimer_s readTimer;
    CEFileSourceTimer_s writeTimer;
    
    CEFileEventHandler_f _Nonnull handler;
    void * _Nullable clientData;
};

/* File event structure 32b-32B 64b-40B */
struct _CEFileEventV2 {
    uint32_t fired: 1;

    uint32_t readable: 1;
    uint32_t writable: 1;
    
    uint32_t poolMask: 2;
    uint32_t mask: 2;
    uint32_t isReadTimeout: 1;
    uint32_t isWriteTimeout: 1;
    uint32_t readTimeoutInterval: 13;//单位为(1/8)秒
    uint32_t writeTimeoutInterval: 13;//单位为(1/8)秒
    uint32_t fdTag;
    
    CEFileSourceTimer_s readTimer;
    CEFileSourceTimer_s writeTimer;
    
    CEFileEventHandler_f _Nonnull handler;
    void * _Nullable clientData;
};








#pragma mark - time source

/* Time event structure */
struct _CETimeEvent {
//    uint64_t isStatic: 1;
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
};

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

#pragma mark - time source manager


//最大允许64k
typedef struct _CEBitTable16 {
    uint64_t n2[1024];
    uint64_t n1[16];
    uint16_t n0;
    uint16_t xxx;
    uint32_t usedCount;
} CEBitTable16_s;

typedef struct _CETimeEventTablePage {
    CETimeEvent_s * _Nullable items[1024];
} CETimeEventTablePage_s;

typedef struct _CETimeEventTable {
    CETimeEventTablePage_s * _Nullable pages[64];
    CEBitTable16_s keyTable;
} CETimeEventTable_s;

typedef struct _CETimeEventQueue {
    CETimeEvent_s * _Nullable * _Nonnull buffer;
    uint32_t capacity;
    uint32_t count;
} CETimeEventQueue_s;

typedef struct _CETimeEventManager {
    CETimeEventTable_s table;
    uint64_t size;
    uint64_t unused;
    uint32_t sequence;
    
    CETimeEvent_s * _Nullable executingTimeEvent;
    CETimeEventQueue_s timerQueue;
} CETimeEventManager_s;



#pragma mark - base api

typedef void (*CEApiPoolCallback_f)(void * _Nullable context, void * _Nonnull api);
typedef void (*CEApiPoolFileEventCallback_f)(void * _Nullable context, void * _Nonnull api, int fd, CEFileEventMask_es mask);

typedef struct _CEApiPoolCallback {
    CEApiPoolFileEventCallback_f _Nonnull fileEventCallback;
    CEApiPoolCallback_f _Nonnull pipeCallback;
} CEApiPoolCallback_s;


#pragma mark - pool source

typedef CEFileDescription_s CEFiredEvent_s;

#define CERunLoopFileTimerPageSize 0x4000

/* State of an event based program */
struct _CEPool {
    _Atomic(uintptr_t) runInThread;
    _Atomic(uint_fast32_t) runningStates;// 0 is stopped , 1 running, 2 is will stopped
    CEPoolProgress_t progress;//
    
#if __APPLE__
    pthread_t _Nonnull thread;
#else
    pthread_t thread;
#endif
    
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


static inline uint32_t CERunLoopNextFdTag(CEPool_s * _Nonnull eventLoop) {
    eventLoop->fdTagSequence ++;
    if (0 == eventLoop->fdTagSequence) {
        eventLoop->fdTagSequence ++;
    }
    return eventLoop->fdTagSequence;
}





#endif /* CEBaseType_h */
