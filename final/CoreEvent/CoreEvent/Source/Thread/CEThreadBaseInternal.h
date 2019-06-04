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

struct _CETaskSyncContext;
typedef struct _CETaskSyncContext CETaskSyncContext_t;
typedef CETaskSyncContext_t * CETaskSyncContextPtr;


struct _CETaskStack;
typedef struct _CETaskStack CETaskStack_s;
typedef CETaskStack_s * CETaskStackPtr;

struct _CEQueue;
typedef struct _CEQueue CEQueue_s;

struct _CEThread {
    CERuntimeAtomicRcBase_s runtime;
#if __APPLE__
    pthread_t _Nullable pthread;
#else
    pthread_t pthread;
#endif
    char name[64];
    CEThreadStatus_t status;
};

struct _CERunLoop {
    pthread_t _Nullable thread;
};

struct _CEThreadSpecificDelegate {
    CEThreadRef _Nonnull thread;
    CEThreadStatus_t status;
    CERunLoopRef _Nonnull (* _Nullable loadRunLoop)(CEThreadRef _Nonnull);
    CERunLoopRef _Nonnull runLoop;
};

struct _CETaskWorker {
    CEThreadRef _Nonnull thread;
    CEQueuePtr _Nonnull queue;
    CESemPtr _Nonnull sem;
    
    _Atomic(uint_fast32_t) actionCode;

    CESourceRef _Nullable source;
};

struct _CEThreadScheduler;
typedef struct _CEThreadScheduler CEThreadScheduler_s;
typedef CEThreadScheduler_s * CEThreadSchedulerRef;

struct _CEThreadScheduler {
    CERuntimeAtomicRcBase_s runtime;
    CESourceRef _Nonnull owner;
    CEThreadRef _Nonnull thread;
    CEQueuePtr _Nullable ownerQueue;

    
    
    CEThreadSpecificDelegatePtr _Nullable loader;
    CETaskWorkerRef _Nullable worker;

    
    uint32_t type;//main queue
    
};

struct _CETaskScheduler;
typedef struct _CETaskScheduler CETaskScheduler_s;
typedef CETaskScheduler_s * CETaskSchedulerRef;

typedef uint_fast32_t CETaskSchedulerStatus_t;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusNoThread = 0;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusCreatingThread = 1;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusRunning = 2;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusWaiting = 3;


struct _CETaskScheduler {
    CERuntimeAtomicRcBase_s runtime;
    CEThreadRef _Nonnull thread;
    CEQueuePtr _Nullable ownerQueue;
    
    CESourceRef _Nonnull source;

    CESpinLock_t lock;
    
    uint32_t id;

    _Atomic(uint_fast32_t) status;

    
    CEThreadSpecificDelegatePtr _Nullable loader;
    CETaskWorkerRef _Nullable worker;
    
    CETaskSyncContextPtr _Nullable syncContext;
    CETaskStackPtr _Nonnull taskStack;
    
    
    CESemPtr _Nonnull waiter;
    
    uint32_t type;//main queue
};

struct _CEThreadSpecific {
    CEThreadRef _Nonnull thread;
    CETaskSchedulerRef _Nullable scheduler;
    CETaskContextPtr _Nullable taskContext;
};

typedef struct _CEThreadContext {
    CEThreadRef _Nonnull thread;
    CEQueuePtr _Nonnull queue;
} CEThreadContext_t;



typedef struct _CETaskSyncContextItem {
    void * _Nonnull task;
    CEQueuePtr _Nonnull queue;
} CETaskSyncContextItem_t;

typedef struct _CETaskSyncContext {
    void * _Nonnull owner;
    size_t blockingQueueCount;
    CETaskSyncContextItem_t blocking[16];
    CETaskSyncContextItem_t * _Nullable moreBlocking;
    CETaskSyncContextItem_t current;
} CETaskSyncContext_t;

typedef CETaskSyncContext_t * CETaskSyncContextPtr;



struct _CETaskContext {
    CETaskContextPtr _Nullable prev;
//thread sem
    CETaskPtr _Nonnull task;
    
    //当前队列
    CEQueuePtr _Nullable queue;
    
    //当前线程
    CEThreadRef _Nonnull thread;


};

typedef void (*CESourceCallback_f)(CESourceRef _Nonnull source);
typedef void (*CESourceAppendTask_f)(CESourceRef _Nonnull source, CETaskPtr _Nonnull task);
typedef CETaskPtr _Nullable (*CESourceRemoveTask_f)(CESourceRef _Nonnull source);



struct _CESource {
#if CEBuild64Bit
    //高14 位 表示执行中的count， 其余为任务个数
    _Atomic(uint_fast64_t) countInfos;// 0->1 do wake; 1->0 do wait
#else
    //高8 位 表示执行中的count， 其余为任务个数
    _Atomic(uint_fast32_t) countInfos;// 0->1 do wake; 1->0 do wait
#endif
    uint32_t concurrency: 7;//[1-127]
    uint32_t concurrencyBitCount: 3;//[1-255]
    
    uint32_t unfinishedCount;
    uint32_t count;
    uint32_t executingCount;

    _Atomic(uint_fast64_t) limitWorkerCount;// 0->1 do wake; 1->0 do wait

    CEThreadSchedulerRef _Nullable scheduler;
    
    void * _Nonnull consumer;
    
    
//    CESpinLock_t lock;
    
    CETaskPtr _Nullable head;
    CETaskPtr _Nullable last;

    CESourceCallback_f _Nonnull weakUp;
    
    CESourceAppendTask_f _Nonnull append;
    CESourceRemoveTask_f _Nonnull remove;
};


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
    
    uint32_t capacity;
    CESourceRef _Nonnull highLevelSource;
    CESourceRef _Nonnull defaultLevelSource;
    CESourceRef _Nonnull lowLevelSource;

    _Atomic(uint_fast32_t) activeThreadCountInfo;//[6-8-8][1:2]
    _Atomic(uint_fast32_t) activeRc;

    
} CEGlobalThreadManager_s;

CETaskSchedulerRef _Nullable CEGlobalThreadManagerDeQueue(void) {
    //    static const CEQueuePriority_t CEQueuePriorityHigh = 192;
    //    static const CEQueuePriority_t CEQueuePriorityDefault = 128;
    //    static const CEQueuePriority_t CEQueuePriorityLow = 64;
    
    return NULL;
}



 void CEGlobalThreadManagerEnQueue(CETaskSchedulerRef _Nonnull schedule) {
//    static const CEQueuePriority_t CEQueuePriorityHigh = 192;
//    static const CEQueuePriority_t CEQueuePriorityDefault = 128;
//    static const CEQueuePriority_t CEQueuePriorityLow = 64;
    
     
     
}


typedef CEGlobalThreadManager_s * CEGlobalThreadManagerPtr;

static CEGlobalThreadManager_s __CEGlobalThreadManager = {};


#if defined(__i386__) || defined(__x86_64__)
#define CENTHREADS 32
#else
#define CENTHREADS 16
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
    if (n < 16) {
        n = 16;
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

//队列状态
struct _CEQueue {
    CERuntimeAtomicRcBase_s runtime;
    char * _Nonnull label;
    uint8_t isSerialQueue;
    uint8_t schedPriority;
    CESourceRef _Nonnull source;
    CESpinLock_t lock;
};


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

void CESourceSerialQueueAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    assert(source);
    assert(task);
    assert(NULL == task->next);
    
    _Bool weakup = false;
    
    CESourceLock(source);
    
    if (source->head == NULL) {
        assert(source->last == NULL);
        assert(source->count == 0);
        source->head = task;
        source->last = task;
        source->count = 1;
        weakup = true;
    } else {
        assert(source->last != NULL);
        assert(source->count > 0);
        source->last->next = task;
        source->last = task;
        source->count += 1;
    }
    CESourceUnlock(source);
    if (weakup) {
        source->weakUp(source);
    }
}

//执行中任务的百分比 4 : 2 : 1

CETaskPtr _Nullable CESourceSerialQueueRemove(CESourceRef _Nonnull source) {
    assert(source);
    
    CETaskPtr result = NULL;
    uint32_t count = 0;
    CESourceLock(source);
    if (source->head == NULL) {
        assert(source->last == NULL);
    } else {
        assert(source->last != NULL);
        if (source->head != source->last) {
            result = source->head;
            source->head = result->next;
            result->next = NULL;
            assert(source->count > 1);
            source->count -= 1;
            count = source->count;
        } else {
            result = source->head;
            source->head = NULL;
            source->last = NULL;
            result->next = NULL;
            assert(source->count == 1);
            source->count = 0;
        }
    }
    CESourceUnlock(source);
    
    return result;
}

//

//removeFirst

//status

//信号量

//count

extern CEThreadSpecificRef _Nonnull CEThreadSpecificGetCurrent(void);

extern CEThreadRef _Nullable _CEThreadCreate(CEThreadConfig_s config,
                                             CETaskSchedulerRef _Nullable scheduler,
                                             void (* _Nullable beforeMain)(CEThreadSpecificRef _Nonnull specific),
                                             void (* _Nonnull main)(void * _Nullable),
                                             void * _Nullable params,
                                             void (* _Nullable paramsDealloc)(void * _Nonnull));



#endif /* CEThreadBaseInternal_h */
