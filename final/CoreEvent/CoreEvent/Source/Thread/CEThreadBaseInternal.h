//
//  CEThreadBaseInternal.h
//  CoreEvent
//
//  Created by vector on 2019/5/8.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEThreadBaseInternal_h
#define CEThreadBaseInternal_h

#include "CEThreadBase.h"
#include "CESem.h"
#include "CEMemory.h"


#pragma mark - CEThread

typedef struct _CEThread {
    CERuntimeAtomicRcBase_s runtime;
#if __APPLE__
    pthread_t _Nullable pthread;
#else
    pthread_t pthread;
#endif
    char name[64];
    CEThreadStatus_t status;
} CEThread_s;


#pragma mark - CEThreadSyncWaiter

typedef struct _CEThreadSyncWaiter {
    CEThread_s * _Nonnull thread;
    CESemPtr _Nonnull sem;
    CEQueueRef _Nullable queue;
} CEThreadSyncWaiter_s;

static inline CEThreadSyncWaiter_s * _Nonnull CEThreadSyncWaiterCreate(CEThread_s * _Nonnull thread) {
    assert(thread);
    CEThreadSyncWaiter_s * result = CEAllocateClear(sizeof(CEThreadSyncWaiter_s));
    result->thread = thread;
    result->sem = CESemInit(0);
    return result;
}
static inline void CEThreadSyncWaiterDestroy(CEThreadSyncWaiter_s * _Nonnull waiter) {
    assert(waiter);
    CESemDeinit(waiter->sem);
}
static inline void CEThreadSyncWaiterWait(CEThreadSyncWaiter_s * _Nonnull waiter) {
    assert(waiter);
    CESemWait(waiter->sem);
}
static inline void CEThreadSyncWaiterSignal(CEThreadSyncWaiter_s * _Nonnull waiter) {
    assert(waiter);
    CESemSignal(waiter->sem);
}



#pragma mark - CETaskScheduler
struct _CETaskScheduler;
typedef struct _CETaskScheduler * CETaskSchedulerPtr;

typedef struct _CETaskScheduler {
    CEThread_s * _Nonnull thread;
    CEQueueRef _Nullable ownerQueue;//当前queue， 如果是个串行队列的线程，ownerQueue 一直有值
    
    CESourceRef _Nonnull source;
    
    CESpinLockPtr _Nonnull lock;
    CESemPtr _Nonnull waiter;
    
    uint32_t type;//main queue
    
    uint8_t context[64];
} CETaskScheduler_s;


#pragma mark - CEThreadSpecific

typedef struct _CEThreadSpecific {
    CEThread_s * _Nonnull thread;
    CEThreadSyncWaiter_s * _Nonnull syncWaiter;
    CETaskSchedulerPtr _Nullable scheduler;
    CETaskContextPtr _Nullable taskContext;
    CEQueueRef _Nullable owner;
} CEThreadSpecific_s;

typedef CEThreadSpecific_s * CEThreadSpecificPtr;



#pragma mark - CETask
struct _CETask;
typedef struct _CETask CETask_s;
typedef CETask_s * CETaskPtr;

struct _CETask {
    CETaskPtr _Nullable next;
    
    CEFunction_f _Nonnull execute;
    CEParamRef _Nullable param;
    CEParamRef _Nullable resultReceiver;
    CEThreadSyncWaiter_s * _Nullable syncTaskWaiter;

    uint32_t isBarrier: 1;
    uint32_t mask: 31;
};

#pragma mark - CEQueue

struct _CEQueue;
typedef struct _CEQueue CEQueue_s;

typedef void (*CEQueueAppendTask_f)(CEQueue_s * _Nonnull queue, CETaskPtr _Nonnull task);
struct _CEQueue {
    CERuntimeAtomicRcBase_s runtime;
    char label[64];
    uint8_t isSerialQueue;
    uint8_t schedPriority;
    CESourceRef _Nonnull source;
    CESpinLock_t lock;
    CEQueueAppendTask_f _Nonnull append;
    CEPtr _Nonnull context;
};

#pragma mark - CETaskScheduler



struct _CERunLoop {
    pthread_t _Nullable thread;
};

struct _CEThreadScheduler;
typedef struct _CEThreadScheduler CEThreadScheduler_s;
typedef CEThreadScheduler_s * CEThreadSchedulerRef;

struct _CEThreadScheduler {
    CERuntimeAtomicRcBase_s runtime;
    CESourceRef _Nonnull owner;
    CEThreadRef _Nonnull thread;
    CEQueueRef _Nullable ownerQueue;

    
    
    CETaskWorkerRef _Nullable worker;

    
    uint32_t type;//main queue
    
};


typedef uint_fast32_t CETaskSchedulerStatus_t;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusNoThread = 0;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusCreatingThread = 1;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusRunning = 2;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusWaiting = 3;

typedef struct _CEGlobalThreadTaskSchedulerContext {
    uint32_t id;
    _Atomic(uint_fast32_t) status;
    
    
    
} CEGlobalThreadTaskSchedulerContext_s;

typedef struct _CESerialThreadTaskSchedulerContext {
    uint32_t id;
    _Atomic(uint_fast32_t) status;
    
    
    
} CESerialThreadTaskSchedulerContext_s;



typedef void (*CESourceCallback_f)(CESourceRef _Nonnull source);
typedef void (*CESourceAppendTask_f)(CESourceRef _Nonnull source, CETaskPtr _Nonnull task);
typedef CETaskPtr _Nullable (*CESourceRemoveTask_f)(CESourceRef _Nonnull source);

#if CEBuild64Bit
typedef uint64_t CESourceCount_t;
#else
typedef uint32_t CESourceCount_t;
#endif


typedef struct _CESourceCounter {
#if CEBuild64Bit
    CESourceCount_t executingCount: 9;/*concurrencyCount*/
    CESourceCount_t count: 55;/*总数 未完成的总数*/
#else
    CESourceCount_t executingCount: 7;/*concurrencyCount*/
    CESourceCount_t count: 25;/*总数 未完成的总数*/
#endif
} CESourceCounter_s;

typedef struct _CESourceConcurrentContext {
    CETaskPtr _Nullable head;
    CETaskPtr _Nullable last;
    CESourceCount_t count;
    uint16_t executingCount;
    uint16_t maxExecutingCount;
    uint32_t isBarrier: 1;

//    uint32_t concurrencyCount: 10;//队列并发数
//    uint32_t activeCount: 10;//当前并发数
} CESourceConcurrentContext_s;

struct _CESource {
    CEThreadSchedulerRef _Nonnull scheduler;
    CESpinLockPtr _Nonnull lock;
    CESourceCallback_f _Nonnull weakUp;
    CESourceAppendTask_f _Nonnull append;
    CESourceRemoveTask_f _Nonnull remove;
    uint16_t type;
    
    uint8_t context[];
};


typedef struct _CESourceStatus {
    
#if CEBuild64Bit
    CESourceCount_t executingCount: 9;/*concurrencyCount*/
    CESourceCount_t count: 55;/*总数 未完成的总数*/
#else
    CESourceCount_t executingCount: 7;/*concurrencyCount*/
    CESourceCount_t count: 25;/*总数 未完成的总数*/
#endif
} CESourceStatus_s;

/*
 queue 状态
 
 
 挂起
 
 执行
 
 执行屏障等待
 
 解除屏障

 */

/*
 worker 状态
 
 普通挂起
 
 挂起 等待屏障解除

 普通执行
 
 执行屏障
 */



//#if CEBuild64Bit
//typedef struct _CETaskWorkerManager {
//    _Atomic(uint_fast64_t) workerStatus[8];
//
//    uint32_t concurrency: 7;//[1-127]
//    uint32_t concurrencyBitCount: 3;//[1-255]
//
//    uint32_t unfinishedCount;
//    uint32_t count;
//    uint32_t executingCount;
//
//
//    CESpinLock_t lock;
//
//    CETaskPtr _Nullable head;
//    CETaskPtr _Nullable last;
//
//
//} CETaskWorkerManager_s;
//#else
//typedef struct _CETaskWorkerManager {
//    _Atomic(uint_fast32_t) workerStatus[8];
//    uint32_t concurrency: 7;//[1-127]
//    uint32_t concurrencyBitCount: 3;//[1-255]
//
//    uint32_t unfinishedCount;
//    uint32_t count;
//    uint32_t executingCount;
//
//
//    CESpinLock_t lock;
//
//    CETaskPtr _Nullable head;
//    CETaskPtr _Nullable last;
//
//
//} CETaskWorkerManager_s;
//#endif

typedef struct _CEGlobalThreadManager {
#if CEBuild64Bit
    _Atomic(uint_fast64_t) workerStatus[8];
    CETaskWorkerRef _Nonnull works[512];
#else
    _Atomic(uint_fast32_t) workerStatus[8];
    CETaskWorkerRef _Nonnull works[256];
#endif
    uint32_t activecpu;
    uint32_t capacity;
    CESourceRef _Nonnull highLevelSource;
    CESourceRef _Nonnull defaultLevelSource;
    CESourceRef _Nonnull lowLevelSource;

    _Atomic(uint_fast32_t) activeThreadCountInfo;//[6-8-8][1:2]
    _Atomic(uint_fast32_t) activeRc;

    
} CEGlobalThreadManager_s;

CETaskSchedulerPtr _Nullable CEGlobalThreadManagerDeQueue(void) {
    //    static const CEQueuePriority_t CEQueuePriorityHigh = 192;
    //    static const CEQueuePriority_t CEQueuePriorityDefault = 128;
    //    static const CEQueuePriority_t CEQueuePriorityLow = 64;
    
    return NULL;
}



 void CEGlobalThreadManagerEnQueue(CETaskSchedulerPtr _Nonnull schedule) {
//    static const CEQueuePriority_t CEQueuePriorityHigh = 192;
//    static const CEQueuePriority_t CEQueuePriorityDefault = 128;
//    static const CEQueuePriority_t CEQueuePriorityLow = 64;
    
     
     
}


typedef CEGlobalThreadManager_s * CEGlobalThreadManagerPtr;

static CEGlobalThreadManager_s __CEGlobalThreadManager = {};


#if defined(__i386__) || defined(__x86_64__)
#define CENTHREADS 16
#else
#define CENTHREADS 4
#endif

void __CEGlobalThreadManagerDefaultInitialize(void) {
    
    uint32_t activecpu;
    uint32_t wq_max_threads;
#ifdef __linux__
    activecpu = (uint32_t)sysconf(_SC_NPROCESSORS_ONLN);
    wq_max_threads = activecpu * CENTHREADS + 2;
#else
    size_t s = sizeof(uint32_t);
    sysctlbyname("hw.activecpu", &activecpu, &s, NULL, 0);
    s = sizeof(uint32_t);
    sysctlbyname("kern.wq_max_threads", &wq_max_threads, &s, NULL, 0);
#endif
    
    size_t n = MIN(activecpu * CENTHREADS, wq_max_threads - 2);
    if (n < 8) {
        n = 8;
    }
    
#if CEBuild64Bit
    if (n > 512) {
        n = 512;
    }
#else
    if (n > 256) {
        n = 256;
    }
#endif

#if CEBuild64Bit
    for (int i=0; i<8; i++) {
        _Atomic(uint_fast64_t) * status = &(__CEGlobalThreadManager.workerStatus[i]);
        uint_fast64_t s = 0;
        atomic_store(status, s);
    }
#else
    for (int i=0; i<8; i++) {
        _Atomic(uint_fast32_t) * status = &(__CEGlobalThreadManager.workerStatus[i]);
        uint_fast32_t s = 0;
        atomic_store(status, s);
    }
#endif
    __CEGlobalThreadManager.activecpu = activecpu;
    __CEGlobalThreadManager.capacity = (uint32_t)n;
    
}

CEGlobalThreadManagerPtr _Nonnull CETaskWorkerManagerGetDefault(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CEGlobalThreadManagerDefaultInitialize);
    return &__CEGlobalThreadManager;
}




////append
//void CESourceLock(CESourceRef _Nonnull source) {
//    CESpinLockLock(&(source->lock));
//
//
//}
//
//void CESourceUnlock(CESourceRef _Nonnull source) {
//    CESpinLockUnlock(&(source->lock));
//
//
//
//}
void CESourceLock(CESourceRef _Nonnull source) {
    CESpinLockLock(&(source->lock));


}

void CESourceUnlock(CESourceRef _Nonnull source) {
    CESpinLockUnlock(&(source->lock));



}






void CESourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    assert(source);
    assert(task);
    assert(NULL == task->next);
    
    source->append(source, task);
}

//执行中任务的百分比 4 : 2 : 1

CETaskPtr _Nullable CESourceRemove(CESourceRef _Nonnull source) {
    assert(source);
    return source->remove(source);
}

//void CESourceSerialQueueAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
//    assert(source);
//    assert(task);
//    assert(NULL == task->next);
//    
//    _Bool weakup = false;
//    
//    CESourceLock(source);
//    
//    if (source->head == NULL) {
//        assert(source->last == NULL);
//        assert(source->counter.count == 0);
//        source->head = task;
//        source->last = task;
//        source->counter.count = 1;
//        weakup = true;
//    } else {
//        assert(source->last != NULL);
//        assert(source->counter.count > 0);
//        source->last->next = task;
//        source->last = task;
//        source->counter.count += 1;
//    }
//    CESourceUnlock(source);
//    if (weakup) {
//        source->weakUp(source);
//    }
//}
//
////执行中任务的百分比 4 : 2 : 1
//
//CETaskPtr _Nullable CESourceSerialQueueRemove(CESourceRef _Nonnull source) {
//    assert(source);
//    
//    CETaskPtr result = NULL;
//    CESourceCount_t count = 0;
//    CESourceLock(source);
//    if (source->head == NULL) {
//        assert(source->last == NULL);
//    } else {
//        assert(source->last != NULL);
//        if (source->head != source->last) {
//            result = source->head;
//            source->head = result->next;
//            result->next = NULL;
//            assert(source->counter.count > 1);
//            source->counter.count -= 1;
//            count = source->counter.count;
//        } else {
//            result = source->head;
//            source->head = NULL;
//            source->last = NULL;
//            result->next = NULL;
//            assert(source->counter.count == 1);
//            source->counter.count = 0;
//        }
//    }
//    CESourceUnlock(source);
//    
//    return result;
//}

//

//removeFirst

//status

//信号量

//count

extern CEThreadSpecificPtr _Nonnull CEThreadSpecificGetCurrent(void);

extern CEThread_s * _Nullable _CEThreadCreate(CEThreadConfig_s config,
                                             CETaskSchedulerPtr _Nullable scheduler,
                                             void (* _Nullable beforeMain)(CEThreadSpecificPtr _Nonnull specific),
                                             void (* _Nonnull main)(void * _Nullable),
                                             void * _Nullable params,
                                             void (* _Nullable paramsDealloc)(void * _Nonnull));



#endif /* CEThreadBaseInternal_h */
