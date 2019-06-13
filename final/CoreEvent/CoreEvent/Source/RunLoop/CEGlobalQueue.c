//
//  CEGlobalQueue.c
//  CoreEvent
//
//  Created by vector on 2019/6/10.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEGlobalQueue.h"
#include "CETaskScheduler.h"
#include "CEConcurrentQueue.h"

typedef uint_fast32_t CETaskSchedulerStatus_t;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusNoThread = 0;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusCreatingThread = 1;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusRunning = 2;
static const CETaskSchedulerStatus_t CETaskSchedulerStatusWaiting = 3;

typedef struct _CEGlobalQueueManager {
    CEQueue_s * _Nonnull highLevelQueue;
    CEQueue_s * _Nonnull defaultLevelQueue;
    CEQueue_s * _Nonnull lowLevelQueue;
    
    CETaskSchedulerPtr _Nonnull schedulers[256];
    CETaskSchedulerPtr _Nonnull schedulersBuffer[256];
    
    CESpinLockPtr _Nonnull lock;

    uint32_t activecpu: 16;
    uint32_t capacity: 16;
    
    uint32_t bufferCount: 16;
    uint32_t defaultLevelCanUsed: 16;//3/4
    uint32_t lowLevelCanUsed: 16;//1/2
    uint32_t highLevelUsed: 16;
    uint32_t defaultLevelUsed: 16;
    uint32_t lowLevelUsed: 16;
    
    //未开始的个数
#if CEBuild64Bit
    _Atomic(uint_fast64_t) highLevelTaskCount;
    _Atomic(uint_fast64_t) defaultLevelTaskCount;
    _Atomic(uint_fast64_t) lowLevelTaskCount;
#else
    _Atomic(uint_fast32_t) highLevelTaskCount;
    _Atomic(uint_fast32_t) defaultLevelTaskCount;
    _Atomic(uint_fast32_t) lowLevelTaskCount;
#endif

    //[4, 2, 1]
    
    _Atomic(uint_fast32_t) adjust;//线程调整
    
    _Atomic(uint_fast32_t) activeThreadCountInfo;//[6-8-8][1:2]
    _Atomic(uint_fast32_t) activeRc;
} CEGlobalQueueManager_s;



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


typedef CEGlobalQueueManager_s * CEGlobalThreadManagerPtr;

static CEGlobalQueueManager_s __CEGlobalQueueManager = {};


#if defined(__i386__) || defined(__x86_64__)
#define CENTHREADS 16
#else
#define CENTHREADS 4
#endif

void __CEGlobalQueueManagerDefaultInitialize(void) {
    
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
    
    if (n < 4) {
        n = 4;
    }

    if (n > 128) {
        n = 128;
    }
    
//#if CEBuild64Bit
//    _Atomic(uint_fast64_t) * status = &(__CEGlobalQueueManager.workerStatus);
//    uint_fast64_t newStatus = 0;
//    atomic_store(status, newStatus);
//#else
//    for (int i=0; i<2; i++) {
//        _Atomic(uint_fast32_t) * status = &(__CEGlobalQueueManager.workerStatus[i]);
//        uint_fast32_t newStatus = 0;
//        atomic_store(status, newStatus);
//    }
//#endif
    __CEGlobalQueueManager.activecpu = activecpu;
    __CEGlobalQueueManager.capacity = (uint32_t)n;
}

CEGlobalThreadManagerPtr _Nonnull CETaskWorkerManagerGetShared(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CEGlobalQueueManagerDefaultInitialize);
    return &__CEGlobalQueueManager;
}




static inline CETaskSchedulerPtr _Nullable _CEGlobalSourceGetAPrivateSchedulerToSignal(CESourceRef _Nonnull source, CESourceConcurrentContext_s * context) {
    if (context->bufferCount <= 0) {
        return NULL;
    }
    CETaskSchedulerPtr result = context->schedulersBuffer[context->bufferCount - 1];
    context->bufferCount -= 1;
    return result;
}

static inline CETaskSchedulerPtr _Nullable _CEGlobalSourceGetASharedSchedulerToSignal(CESourceRef _Nonnull source, CESourceConcurrentContext_s * context) {
    CEGlobalThreadManagerPtr manager = CETaskWorkerManagerGetShared();
    
    
    switch (context->type) {
        case CESourceConcurrentContextTypeGlobalHigh: {
            if (manager->bufferCount <= 0) {
                //剥夺其他队列中的线程
                
                return NULL;
            }
            CETaskSchedulerPtr result = manager->schedulersBuffer[manager->bufferCount - 1];
            manager->bufferCount -= 1;
            return result;
        }
            break;
        case CESourceConcurrentContextTypeGlobalDefault: {
            
            
            
        }
            break;
        case CESourceConcurrentContextTypeGlobalLow: {
            
            
            
        }
            break;
        default: {
            abort();
        }
            break;
    }
    
    return NULL;
}

void CEGlobalSourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    CESourceConcurrentContext_s * context = source->context;
    
    assert(CESourceConcurrentContextTypeGlobalHigh <= context->type && CESourceConcurrentContextTypeGlobalLow >= context->type);
    
    //添加任务最多只能唤醒一个线程
    CETaskSchedulerPtr scheduler = NULL;
    CESourceCount_t count = 0;
    CESpinLockLock(source->lock);
    CESourceTaskStoreAppend(&(context->tasks), task);
    count = context->tasks.count;
    
    scheduler = _CEGlobalSourceGetAPrivateSchedulerToSignal(source, context);
    if (NULL == scheduler) {
        scheduler = _CEGlobalSourceGetASharedSchedulerToSignal(source, context);
    }
    CESpinLockUnlock(source->lock);
    
    if (NULL != scheduler) {
        CESemSignal(scheduler->waiter);
    }
    
}








//void CEGlobalThreadManagerDispatch(CESourceRef _Nonnull source) {
//    CETaskSchedulerPtr scheduler = CEGlobalThreadManagerDeQueue();
//    if (scheduler) {
//        assert(CETaskSchedulerBindSource(scheduler, source));
//        CETaskSchedulerSignal(scheduler);
//    }
//}


void CEGlobalTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler);


void CEGlobalQueueMainFunc(void * _Nullable param);
void CEGlobalQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific);


CETaskPtr _Nullable CEGlobalTaskSchedulerRemoveTask(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    
    CETaskPtr task = NULL;
    CESpinLockLock(scheduler->lock);
    if (NULL != scheduler->source) {
        task = CESourceRemove(scheduler->source);
    }
    CESpinLockUnlock(scheduler->lock);
    
    return task;
}

_Bool CEGlobalTaskSchedulerBindSource(CETaskSchedulerPtr _Nonnull scheduler, CESourceRef _Nonnull source) {
    assert(scheduler);
    _Bool result = true;
    CESpinLockLock(scheduler->lock);
    if (NULL == scheduler->source) {
        scheduler->source = source;
    } else {
        result = false;
    }
    CESpinLockUnlock(scheduler->lock);
    
    return result;
}
CESourceRef _Nullable CEGlobalTaskSchedulerUnbindSource(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    CESourceRef _Nonnull source = NULL;
    CESpinLockLock(scheduler->lock);
    source = scheduler->source;
    scheduler->source = NULL;
    CESpinLockUnlock(scheduler->lock);
    return source;
}


CETaskSchedulerPtr _Nonnull CEGlobalTaskSchedulerCreate(uint32_t id) {
    CETaskSchedulerPtr scheduler = CETaskSchedulerCreate(NULL);
    
    CETaskSchedulerConcurrentContext_s * context = (CETaskSchedulerConcurrentContext_s *)scheduler->context;
    context->id = id;
    atomic_store(&(context->status), CETaskSchedulerStatusNoThread);
    return scheduler;
}

void CEGlobalTaskSchedulerDestroy(CETaskSchedulerPtr _Nonnull scheduler) {
    
    
    
    CETaskSchedulerDestroy(scheduler);
}

void CEGlobalTaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler) {
    CESemWait(scheduler->waiter);
}
void CEGlobalTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
    assert(scheduler);
    assert(CEQueueTypeGolbal == scheduler->source->queue->type);
    
    uint_fast32_t status = 0;
    uint_fast32_t newStatus = 0;
    uint32_t times = 0;
    CETaskSchedulerConcurrentContext_s * context = (CETaskSchedulerConcurrentContext_s *)scheduler->context;
    
    do {
        times += 1;
        assert(1 == times);
        status = atomic_load(&(context->status));
        
        switch (status) {
            case CETaskSchedulerStatusNoThread: {
                newStatus = CETaskSchedulerStatusCreatingThread;
            }
                break;
            case CETaskSchedulerStatusWaiting: {
                newStatus = CETaskSchedulerStatusRunning;
            }
                break;
            case CETaskSchedulerStatusRunning://穿透
            default: {
                abort();
                return;
            }
                break;
        }
    } while (!atomic_compare_exchange_strong(&(context->status), &status, newStatus));
    
    
    if (CETaskSchedulerStatusCreatingThread == newStatus) {
        CEThreadConfig_s config = {};
        float schedPriority = 0;
        snprintf(config.name, 63, "CoreEvent.global.thread.%u", context->id);
        config.schedPriority = schedPriority;
        _CEThreadCreate(config, scheduler, CEGlobalQueueBeforeMainFunc, CEGlobalQueueMainFunc, NULL, NULL);
    } else if (CETaskSchedulerStatusRunning == newStatus) {
        //do wake up
        CESemSignal(scheduler->waiter);
    } else {
        abort();
    }
}


void CEGlobalQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific) {
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    CETaskSchedulerConcurrentContext_s * context = (CETaskSchedulerConcurrentContext_s *)scheduler->context;
    uint_fast32_t status = 0;
    uint_fast32_t newStatus = 0;
    uint32_t times = 0;
    do {
        times += 1;
        assert(1 == times);
        status = atomic_load(&(context->status));
        assert(CETaskSchedulerStatusCreatingThread == status);
        newStatus = CETaskSchedulerStatusRunning;
    } while (!atomic_compare_exchange_strong(&(context->status), &status, newStatus));
}

void CEGlobalQueueMainFunc(void * _Nullable param) {
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    CETaskSchedulerConcurrentContext_s * context = (CETaskSchedulerConcurrentContext_s *)scheduler->context;
    
    uint_fast32_t status = atomic_load(&(context->status));
    assert(CETaskSchedulerStatusRunning == status);
    while (1) {
        switch (status) {
            case CETaskSchedulerStatusWaiting: {
                CEGlobalTaskSchedulerUnbindSource(scheduler);
                CEGlobalThreadManagerEnQueue(scheduler);
                
                CEGlobalTaskSchedulerWait(scheduler);
            }
                break;
            case CETaskSchedulerStatusRunning: {
                CETaskPtr task = CEGlobalTaskSchedulerRemoveTask(scheduler);
                if (task) {
                    //do task
                    
                    CETaskSchedulerExecuteTask(scheduler, task);
                } else {
                    //change status
                    uint_fast32_t newStatus = CETaskSchedulerStatusWaiting;
                    if (atomic_compare_exchange_strong(&(context->status), &status, newStatus)) {
                        status = newStatus;
                        continue;
                    }
                }
            }
                break;
            case CETaskSchedulerStatusCreatingThread://穿透
            case CETaskSchedulerStatusNoThread://穿透
            default: {
                abort();
            }
                break;
        }
        status = atomic_load(&(context->status));
    }
}


//source

CESource_s * _Nonnull CEGlobalSourceCreate(CEQueue_s * _Nonnull queue) {
    assert(queue);
    CESourceConcurrentContext_s * context = CEAllocateClear(sizeof(CESourceConcurrentContext_s));
    context->scheduler = CEGlobalTaskSchedulerCreate(queue);
    CESource_s * source = CESourceCreate(queue, context, CEGlobalSourceAppend);
    return source;
}

//queue

CEQueue_s * _Nonnull CEGlobalQueueCreate(char * _Nullable label, CEQueuePriority_t priority) {
    CEQueue_s * queue = CEQueueCreate(label, 1, priority, CEQueueTypeGolbal);
    queue->source = CEGlobalSourceCreate(queue);
    return queue;
}
