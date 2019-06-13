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
    
    CESpinLockPtr _Nonnull lock;
    
    CESourceListStore_s highTasks;
    CESourceListStore_s defaultTasks;
    CESourceListStore_s lowTasks;
    
    CETaskSchedulerPtr _Nonnull highSchedulers[16];
    CETaskSchedulerPtr _Nonnull highSchedulersBuffer[16];
    
    CETaskSchedulerPtr _Nonnull defaultSchedulers[8];
    CETaskSchedulerPtr _Nonnull defaultSchedulersBuffer[8];
    
    CETaskSchedulerPtr _Nonnull lowSchedulers[4];
    CETaskSchedulerPtr _Nonnull lowSchedulersBuffer[4];
    
    CETaskSchedulerPtr _Nonnull globalSchedulers[32];
    CETaskSchedulerPtr _Nonnull globalSchedulersBuffer[32];
    
    uint32_t activecpu: 16;
    uint32_t privateHighSchedulerCount: 16;
    uint32_t privateDefaultSchedulerCount: 16;
    uint32_t privateLowSchedulerCount: 16;
    uint32_t privateSchedulerCount: 16;
    uint32_t globalSchedulerCount: 16;
    
    uint32_t highExecutingCount;
    uint32_t defaultExecutingCount;
    uint32_t lowExecutingCount;
    

    
    uint32_t currentHighConcurrencyCount;
    uint32_t currentDefaultConcurrencyCount;
    uint32_t currentLowConcurrencyCount;
    
    uint32_t highBufferCount;
    uint32_t defaultBufferCount;
    uint32_t lowBufferCount;
    uint32_t globalBufferCount;
    
    _Atomic(uint_fast32_t) executingInfo;
} CEGlobalQueueManager_s;
typedef CEGlobalQueueManager_s * CEGlobalThreadManagerPtr;

//uint32_t currentConcurrencyCount;
//uint32_t executingCount;

static inline uint_fast32_t CEGlobalThreadManagerGetExecutingInfo(CEGlobalThreadManagerPtr _Nonnull context, uint32_t * _Nonnull currentConcurrencyCount, uint32_t * _Nonnull executingCount) {
    uint_fast32_t result = atomic_load(&(context->executingInfo));
    *currentConcurrencyCount = ((result >> 16) & 0xFFFFul);
    *executingCount = (result & 0xffff);
    return result;
}

static inline _Bool CEGlobalThreadManagerSetExecutingInfo(CEGlobalThreadManagerPtr _Nonnull context, uint32_t currentConcurrencyCount, uint32_t executingCount, uint_fast32_t status) {
    uint_fast32_t newStatus = ((currentConcurrencyCount << 16) & 0xFFFF0000ul) | (executingCount & 0xFFFF0000ul);
    return atomic_compare_exchange_strong(&(context->executingInfo), &status, newStatus);
}



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
//    __CEGlobalQueueManager.capacity = (uint32_t)n;
}

CEGlobalThreadManagerPtr _Nonnull CETaskWorkerManagerGetShared(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CEGlobalQueueManagerDefaultInitialize);
    return &__CEGlobalQueueManager;
}




typedef struct _CESourceGlobalContext {

} CESourceGlobalContext_s;




//static inline CETaskSchedulerPtr _Nullable _CEGlobalSourceGetAPrivateSchedulerToSignal(CESourceRef _Nonnull source, CEGlobalThreadManagerPtr context) {
//    if (context->bufferCount <= 0) {
//        return NULL;
//    }
//    CETaskSchedulerPtr result = context->schedulersBuffer[context->bufferCount - 1];
//    context->bufferCount -= 1;
//    return result;
//}
//
//static inline CETaskSchedulerPtr _Nullable _CEGlobalSourceGetASharedSchedulerToSignal(CESourceRef _Nonnull source, CEGlobalThreadManagerPtr context) {
//    CEGlobalThreadManagerPtr manager = CETaskWorkerManagerGetShared();
//
//
//    switch (context->type) {
//        case CESourceConcurrentContextTypeGlobalHigh: {
//            if (manager->bufferCount <= 0) {
//                //剥夺其他队列中的线程
//
//                return NULL;
//            }
//            CETaskSchedulerPtr result = manager->schedulersBuffer[manager->bufferCount - 1];
//            manager->bufferCount -= 1;
//            return result;
//        }
//            break;
//        case CESourceConcurrentContextTypeGlobalDefault: {
//
//
//
//        }
//            break;
//        case CESourceConcurrentContextTypeGlobalLow: {
//
//
//
//        }
//            break;
//        default: {
//            abort();
//        }
//            break;
//    }
//
//    return NULL;
//}

void CEGlobalSourceHighAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    CEGlobalThreadManagerPtr context = source->context;
    CESourceListStore_s * tasksPtr = &(context->highTasks);
    //添加任务最多只能唤醒一个线程
    CETaskSchedulerPtr scheduler = NULL;
    CESourceCount_t count = 0;
    uint32_t * executingCountPtr = &(context->highExecutingCount);
    
    
    
    CESpinLockLock(source->lock);
    CESourceTaskStoreAppend(tasksPtr, task);
    count = tasksPtr->count;
    
    if (count == 1) {//列表数据从空到有

        //链表中存储的task+正在执行的 task个数
        CESourceCount_t totalCount = count + context->highExecutingCount;
        uint32_t workerCount = context->privateHighSchedulerCount - context->highBufferCount + context->globalSchedulerCount - context->globalBufferCount;
        
        
        if (context->highExecutingCount < totalCount) {//当前并发数小于task总个数
            //do weak up a taskScheduler
            
            
            
            
        }
    } else {//列表数据增加
        if (0 == context->isBarrier) {//当前不在屏障状态下
            if (context->currentConcurrencyCount < context->maxConcurrencyCount) {
                //链表中存储的task+正在执行的 task个数
                CESourceCount_t totalCount = count + context->executingCount;
                if (context->currentConcurrencyCount < totalCount) {//当前并发数小于task总个数
                    //do weak up a taskScheduler
                    scheduler = _CEConcurrentSourceGetASchedulerToSignal(source, context);
                }
            }
        }
    }
    
    CESpinLockUnlock(source->lock);
    
    if (NULL != scheduler) {
        CESemSignal(scheduler->waiter);
    }
}

CETaskPtr _Nonnull CEGlobalSourceFinishOneTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler, uint32_t isBarrierTask) {
    if (0 != isBarrierTask) {
        isBarrierTask = 1;
    }
    
    CETaskSchedulerPtr _Nonnull schedulersBuffer[256] = {};
    uint32_t weakUpCount = 0;
    
    int action = 0;
    CETaskPtr result = NULL;
    CESourceConcurrentContext_s * context = source->context;
    
    //移除任务会改变 isBarrier
    CESpinLockLock(source->lock);
    CESourceCount_t count = context->tasks.count;
    if (isBarrierTask) {//屏障task 执行完成
        assert(context->isBarrier);
        result = CESourceTaskStoreRemove(&(context->tasks));
        if (NULL == result) {
            context->isBarrier = 0;
            context->executingCount -= 1;
            
            //挂起
            action = -1;
            
        } else {
            uint32_t headTaskIsBarrier = context->tasks.head->isBarrier;
            if (0 == headTaskIsBarrier) { //解除屏障
                context->isBarrier = 0;
                
                //唤醒更多线程
                action = 1;
                
                uint32_t loopCount = context->maxConcurrencyCount - context->executingCount;
                if (loopCount > context->tasks.count) {
                    loopCount = (uint32_t)(context->tasks.count);
                }
                
                CETaskPtr tmp = context->tasks.head;
                weakUpCount = loopCount;
                
                for (uint32_t index=0; index<loopCount; index++) {
                    assert(tmp);
                    if (0 != tmp->isBarrier) {
                        weakUpCount = index;
                        break;
                    }
                    tmp = tmp->next;
                }
                
                assert(weakUpCount <= context->bufferCount);
                
                memcpy(schedulersBuffer, &(context->schedulersBuffer[context->bufferCount - weakUpCount]), sizeof(CETaskSchedulerPtr) * weakUpCount);
                
                context->bufferCount -= weakUpCount;
            }
        }
    } else {
        //非屏障task 执行完成
        if (0 == context->isBarrier) {//当前不在屏障状态下
            result = CESourceTaskStoreRemove(&(context->tasks));
            if (NULL == result) {
                context->executingCount -= 1;
                
                //挂起
                action = -1;
            } else {
                uint32_t headTaskIsBarrier = context->tasks.head->isBarrier;
                if (0 != headTaskIsBarrier) { //开始屏障
                    context->isBarrier = 1;
                }
            }
        } else {
            context->executingCount -= 1;
            
            //挂起
            
            action = -1;
        }
    }
    
    if (action < 0) {
        context->currentConcurrencyCount -= 1;
        context->schedulersBuffer[context->bufferCount] = scheduler;
        context->bufferCount += 1;
    }
    
    CESpinLockUnlock(source->lock);
    
    
    if (NULL != result) {
        if (action > 1) {
            for (uint32_t index=0; index<weakUpCount; index++) {
                CETaskSchedulerPtr s = schedulersBuffer[index];
                assert(s);
                CESemSignal(s->waiter);
            }
        }
        return result;
    } else {
        if (action < 0) {
            CESemWait(scheduler->waiter);
        }
        while (NULL == result) {
            CESpinLockLock(source->lock);
            _Bool wait = false;
            if (0 == context->isBarrier) {//当前不在屏障状态下
                result = CESourceTaskStoreRemove(&(context->tasks));
                if (NULL == result) {
                    context->executingCount -= 1;
                    
                    //挂起
                    wait = true;
                } else {
                    uint32_t headTaskIsBarrier = context->tasks.head->isBarrier;
                    if (0 != headTaskIsBarrier) { //开始屏障
                        context->isBarrier = 1;
                    }
                }
            } else {
                context->executingCount -= 1;
                
                //挂起
                wait = true;
            }
            if (wait) {
                context->currentConcurrencyCount -= 1;
                context->schedulersBuffer[context->bufferCount] = scheduler;
                context->bufferCount += 1;
            }
            CESpinLockUnlock(source->lock);
        }
        return result;
    }
}

CETaskPtr _Nonnull CEConcurrentSourceRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler) {
    assert(source);
    assert(scheduler);
    
    CESourceConcurrentContext_s * context = source->context;
    CESemWait(scheduler->waiter);
    
    CETaskPtr result = NULL;
    CESpinLockLock(source->lock);
    result = CESourceTaskStoreRemove(&(context->tasks));
    CESpinLockUnlock(source->lock);
    assert(result);
    return result;
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
