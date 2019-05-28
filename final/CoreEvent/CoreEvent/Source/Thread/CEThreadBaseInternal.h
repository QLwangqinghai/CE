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

typedef struct _CEQueueBase {
    char * _Nonnull label;
    size_t maxConcurrentCount;//[1, ...], 取值为1 时 为串行队列
} CEQueueBase_t;

typedef CEQueueBase_t * CEQueueBaseRef;

struct _CEThread {
    CERuntimeAtomicRcBase_s runtime;
#if __APPLE__
    pthread_t _Nullable pthread;
#else
    pthread_t pthread;
#endif
    size_t nameBufferLength;//==0时 为静态数据
    char * _Nonnull name;
    CEQueueRef _Nullable queue;
};

struct _CERunLoop {
    pthread_t _Nullable thread;
};

struct _CEThreadLoader {
    CEThreadRef _Nonnull thread;
    CEThreadStatus_t status;
    
    CERunLoopRef _Nonnull (* _Nullable loadRunLoop)(CEThreadRef _Nonnull);
    CERunLoopRef _Nonnull runLoop;
};

struct _CETaskWorker {
    CEThreadRef _Nonnull thread;
    CEQueueRef _Nonnull queue;
    void * owmerQueue;
    CESemPtr _Nonnull sem;
};

struct _CEThreadSpecific {
    CEThreadRef _Nonnull thread;
    CEThreadLoaderRef _Nullable loader;
    CETaskWorkerRef _Nullable worker;
    
    CEQueueBaseRef _Nullable queue;
    CETaskSyncContextPtr _Nullable syncContext;
    CETaskStackPtr _Nonnull taskStack;
    
    CETaskContextPtr _Nullable executing;
    
};

typedef struct _CEThreadContext {
    CEThreadRef _Nonnull thread;
    CEQueueBaseRef _Nonnull queue;
} CEThreadContext_t;



typedef struct _CETaskSyncContextItem {
    void * _Nonnull task;
    CEQueueBaseRef _Nonnull queue;
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
    CETaskRef _Nonnull task;
    
    //当前队列
    CEQueueBaseRef _Nullable queue;
    
    //当前线程
    CEThreadRef _Nonnull thread;


};
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

    
    CESpinLock_t lock;
    
    CETaskRef _Nullable head;
    CETaskRef _Nullable last;

    
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
//    CETaskRef _Nullable head;
//    CETaskRef _Nullable last;
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
//    CETaskRef _Nullable head;
//    CETaskRef _Nullable last;
//
//
//} CETaskWorkerManager_s;
//#endif

typedef struct _CETaskWorkerManager {
#if CEBuild64Bit
    _Atomic(uint_fast64_t) workerStatus[8];
    CETaskWorkerRef _Nonnull works[512];
#else
    _Atomic(uint_fast32_t) workerStatus[8];
    CETaskWorkerRef _Nonnull works[256];
#endif
    
    uint32_t capacity;
    
    
} CETaskWorkerManager_s;

typedef void * CETaskWorkerManagerPtr;

static CETaskWorkerManager_s __CETaskWorkerManagerDefault = {};


#if defined(__i386__) || defined(__x86_64__)
#define CENTHREADS 32
#else
#define CENTHREADS 16
#endif

void __CETaskWorkerManagerDefaultInitialize(void) {
    
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
    assert(n >= 16);
    
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
        _Atomic(uint_fast64_t) * status = &(__CETaskWorkerManagerDefault.workerStatus[i]);
        uint_fast64_t s = 0;
        atomic_store(status, s);
    }
#else
    for (int i=0; i<8; i++) {
        _Atomic(uint_fast32_t) * status = &(__CETaskWorkerManagerDefault.workerStatus[i]);
        uint_fast32_t s = 0;
        atomic_store(status, s);
    }
#endif
    __CETaskWorkerManagerDefault.capacity = (uint32_t)n;
}

CETaskWorkerManagerPtr _Nonnull CETaskWorkerManagerGetDefault(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CETaskWorkerManagerDefaultInitialize);
    return &__CETaskWorkerManagerDefault;
}


//append

void CESourceLock(CESourceRef _Nonnull source) {
    
    
    
}

void CESourceUnlock(CESourceRef _Nonnull source) {
    
    
    
}
void CESourceWeakup(CESourceRef _Nonnull source) {
    
    
    
}
void CESourceWait(CESourceRef _Nonnull source) {
    
    
    
}


void CESourceAppend(CESourceRef _Nonnull source, CETaskRef _Nonnull task) {
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
        CESourceWeakup(source);
    }
}

CETaskRef _Nullable CESourceRemoveFirst(CESourceRef _Nonnull source) {
    assert(source);
    
    CETaskRef result = NULL;
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

//removeFirst

//status

//信号量

//count




extern CEThreadSpecificRef _Nonnull _CEThreadSpecificGetCurrent(char * _Nullable threadName, _Bool copyThreadName);

extern CEThreadSpecificRef _Nonnull CEThreadSpecificGetCurrent(void);

#endif /* CEThreadBaseInternal_h */
