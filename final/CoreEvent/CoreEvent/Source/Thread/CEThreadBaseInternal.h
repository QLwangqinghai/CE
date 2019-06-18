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

struct _CEQueue;
typedef struct _CEQueue CEQueue_s;



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
    result->sem = CESemCreate(0);
    return result;
}
static inline void CEThreadSyncWaiterDestroy(CEThreadSyncWaiter_s * _Nonnull waiter) {
    assert(waiter);
    CESemDestroy(waiter->sem);
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
    CESpinLockPtr _Nonnull lock;
    CESemPtr _Nonnull waiter;
    CEThread_s * _Nonnull thread;
    CEQueue_s * _Nullable ownerQueue;//当前queue， 如果是个串行队列的线程，ownerQueue 一直有值
    CESourceRef _Nonnull source;
    uint16_t qid;
    uint16_t sid;
    uint32_t executingTaskTag;
    
} CETaskScheduler_s;


#pragma mark - CEThreadSpecific

typedef struct _CEThreadSpecific {
    CEThread_s * _Nonnull thread;
    CEThreadSyncWaiter_s * _Nonnull syncWaiter;
    CETaskSchedulerPtr _Nullable scheduler;
    CEQueue_s * _Nullable owner;
} CEThreadSpecific_s;

typedef CEThreadSpecific_s * CEThreadSpecificPtr;



#pragma mark - CETask
struct _CETask;
typedef struct _CETask CETask_s;
typedef CETask_s * CETaskPtr;

struct _CETask {
    CETaskPtr _Nullable next;
    CERef _Nonnull obj;
    CETaskFinish_f _Nullable finish;
    CEFunction_f _Nonnull execute;
    CETaskParamRef _Nullable param;
    CETaskParamRef _Nullable resultReceiver;
    CEThreadSyncWaiter_s * _Nullable syncTaskWaiter;

    uint32_t isBarrier: 1;
    uint32_t mask: 31;
    uint32_t tag;
};



#pragma mark - CEQueue

typedef void (*CEQueueAppendTask_f)(CEQueue_s * _Nonnull queue, CETaskPtr _Nonnull task);
struct _CEQueue {
    CERuntimeAtomicRcBase_s runtime;
    char label[64];
    CESourceRef _Nonnull source;
    uint16_t id;
    uint16_t concurrencyCount;
    CEQueuePriority_t priority;
    CEQueueType_t type;
};

static inline float CEQueuePriorityToThreadPriority(CEQueuePriority_t priority) {
    CEQueuePriority_t p = priority;
    if (p > 256) {
        p = 256;
    }
    return (float)(p) / 128.0f - 1.0f;
}

#pragma mark - CETaskScheduler



#pragma mark - CESource

typedef void (*CESourceCallback_f)(CESourceRef _Nonnull source);
typedef void (*CESourceAppendTask_f)(CESourceRef _Nonnull source, CETaskPtr _Nonnull task);
typedef CETaskPtr _Nullable (*CESourceRemoveTask_f)(CESourceRef _Nonnull source);

#if CEBuild64Bit
typedef uint64_t CESourceCount_t;
static const CESourceCount_t CESourceCountMax = UINT64_MAX;
#else
typedef uint32_t CESourceCount_t;
static const CESourceCount_t CESourceCountMax = UINT32_MAX;
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

typedef struct _CESourceListStore {
    CETaskPtr _Nullable head;
    CETaskPtr _Nullable last;
    CESourceCount_t count;//列表中存储的个数
} CESourceListStore_s;

static inline void CESourceTaskStoreAppend(CESourceListStore_s * _Nonnull list, CETaskPtr _Nonnull task) {
    assert(list);
    assert(task);
    assert(NULL == task->next);
    if (list->head == NULL) {
        assert(list->last == NULL);
        assert(list->count == 0);
        list->head = task;
        list->last = task;
        list->count = 1;
    } else {
        assert(list->last != NULL);
        assert(list->count > 0);
        list->last->next = task;
        list->last = task;
        list->count += 1;
    }
}

static inline CETaskPtr _Nullable CESourceTaskStoreRemove(CESourceListStore_s * _Nonnull list) {
    CETaskPtr result = NULL;
    if (list->head == NULL) {
        assert(list->last == NULL);
    } else {
        assert(list->last != NULL);
        if (list->head != list->last) {
            result = list->head;
            list->head = result->next;
            result->next = NULL;
            assert(list->count > 1);
            list->count -= 1;
        } else {
            result = list->head;
            list->head = NULL;
            list->last = NULL;
            result->next = NULL;
            assert(list->count == 1);
            list->count = 0;
        }
    }
    return result;
}

typedef struct _CESourceSerialMainContext {
    CESourceCount_t count;
    CESourceListStore_s tasks;
    CESourceListStore_s interactiveTasks;
    CETaskSchedulerPtr _Nonnull scheduler;
} CESourceSerialMainContext_s;


struct _CESource {
    CEQueue_s * _Nonnull queue;
    CESpinLockPtr _Nonnull lock;
    CESourceAppendTask_f _Nonnull append;    
    CEPtr _Nonnull context;
};
static inline void CESourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    assert(source);
    assert(task);
    assert(NULL == task->next);
    
    source->append(source, task);
}


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



struct _CERunLoop {
    pthread_t _Nullable thread;
};




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
