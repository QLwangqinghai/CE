//
//  CEConcurrentQueue.c
//  CoreEvent
//
//  Created by vector on 2019/6/10.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEConcurrentQueue.h"
#include "CETaskScheduler.h"
#include "CESource.h"
#include "CEQueueInternal.h"
#include "CEQueueLog.h"



//typedef struct _CESourceConcurrentContext {
//    CESourceListStore_s tasks;
//
//    CETaskSchedulerPtr _Nonnull schedulers[256];
//    CETaskSchedulerPtr _Nonnull schedulersBuffer[256];
//
//    uint32_t maxConcurrencyCount: 16;
//    uint32_t bufferCount: 16;
//    uint32_t executingCount: 16;
//    uint32_t running: 1;
//    uint32_t isBarrier: 1;
//    uint32_t xxx: 14;
//    uint32_t barrierTaskTag;
//} CESourceConcurrentContext_s;



////running == 0 时调用
//static inline void _CESourceConcurrentContextRunning(CESourceConcurrentContext_s * _Nonnull context, CETaskPtr _Nonnull task) {
//    assert(0 == context->isBarrier);
//    assert(context->bufferCount + 1 == context->maxConcurrencyCount);
//
//    context->isBarrier = task->isBarrier;
//    context->barrierTaskTag = task->tag;
//}
//
////running == 1 && context->bufferCount == context->maxConcurrencyCount 时调用
//static inline void _CESourceConcurrentContextStop(CESourceConcurrentContext_s * _Nonnull context) {
//    assert(1 == context->running);
//    assert(context->bufferCount == context->maxConcurrencyCount);
//
//    context->running = 0;
//    context->isBarrier = 0;
//    context->barrierTaskTag = 0;
//}

void CEConcurrentQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific);

void CEConcurrentQueueMainFunc(void * _Nullable param);

static inline void _CEConcurrentTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler) {
    if (NULL == CETaskSchedulerGetThread(scheduler)) {
        CEThreadConfig_s config = {};
        snprintf(config.name, 64, "%s.%x", scheduler->source->queue->label, scheduler->sid);
        float schedPriority = CEQueuePriorityToThreadPriority(scheduler->source->queue->priority);
        config.schedPriority = schedPriority;
        _CEThreadCreate(config, scheduler, CEConcurrentQueueBeforeMainFunc, CEConcurrentQueueMainFunc, NULL, NULL);
        CESemSignal(scheduler->waiter);
    }
    CESemSignal(scheduler->waiter);
}

static inline CETaskSchedulerPtr _Nonnull _CEConcurrentSourcePopScheduler(CESourceRef _Nonnull source, CESourceConcurrentContext_s * _Nonnull context) {
    assert(context->bufferCount > 0);
    CETaskSchedulerPtr result = context->schedulersBuffer[context->bufferCount - 1];
    context->bufferCount -= 1;
    return result;
}

static inline void _CEConcurrentSourcePopSchedulers(CESourceRef _Nonnull source, CESourceConcurrentContext_s * _Nonnull context, uint32_t count, CETaskSchedulerPtr _Nullable * _Nonnull schedulers) {
    assert(context->bufferCount >= count);
    
    memcpy(schedulers, &(context->schedulersBuffer[context->bufferCount - count]), sizeof(CETaskSchedulerPtr) * count);
    context->bufferCount -= count;
}

static inline void _CEConcurrentSourcePushScheduler(CESourceRef _Nonnull source, CESourceConcurrentContext_s * _Nonnull context, CETaskSchedulerPtr _Nonnull scheduler) {
    assert(context->bufferCount < context->maxConcurrencyCount);
    context->schedulersBuffer[context->bufferCount] = scheduler;
    context->bufferCount += 1;
}

void CEConcurrentSourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    CESourceConcurrentContext_s * context = source->context;
    
    //添加任务最多只能唤醒一个线程
    CETaskSchedulerPtr scheduler = NULL;
    CESourceCount_t count = 0;
    CESpinLockLock(source->lock);
    CESourceTaskStoreAppend(&(context->tasks), task);
    count = context->tasks.count;
    
    if (count == 1) {//列表数据从空到有
        if (0 == context->isBarrier) {//当前不在屏障状态下
            uint32_t headTaskIsBarrier = context->tasks.head->isBarrier;
            if (1 == headTaskIsBarrier) { //开始屏障
                context->isBarrier = 1;
                
                if (context->bufferCount == context->maxConcurrencyCount) {//没有正在执行中的任务， 唤醒线程以执行屏障任务
                    scheduler = _CEConcurrentSourcePopScheduler(source, context);
                }
            } else {
                if (context->bufferCount != 0) {
                    //链表中存储的task+正在执行的 task个数
                    CESourceCount_t totalCount = count + context->executingCount;
                    if (context->maxConcurrencyCount - context->bufferCount < totalCount) {//当前并发数小于task总个数
                        //do weak up a taskScheduler
                        scheduler = _CEConcurrentSourcePopScheduler(source, context);
                    }
                }
            }
        }
        //else 当前在屏障状态下 不再唤醒新线程
    } else {//列表数据增加
        if (0 == context->isBarrier) {//当前不在屏障状态下
            if (context->bufferCount != 0) {
                //链表中存储的task+正在执行的 task个数
                CESourceCount_t totalCount = count + context->executingCount;
                if (context->maxConcurrencyCount - context->bufferCount < totalCount) {//当前并发数小于task总个数
                    //do weak up a taskScheduler
                    scheduler = _CEConcurrentSourcePopScheduler(source, context);
                }
            }
        }
    }
    
    CESpinLockUnlock(source->lock);
    
    if (NULL != scheduler) {
        _CEConcurrentTaskSchedulerSignal(scheduler);
    }
}

CETaskPtr _Nonnull CEConcurrentSourceRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler);


static inline CETaskPtr _Nonnull CEConcurrentSourceFinishBarrierTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler, uint32_t barrierTaskTag) {

    CETaskSchedulerPtr _Nonnull schedulersBuffer[256] = {};
    uint32_t weakUpCount = 0;
    
    CETaskPtr result = NULL;
    CESourceConcurrentContext_s * context = source->context;
    
    //移除任务会改变 isBarrier
    CESpinLockLock(source->lock);
    assert(context->executingCount == 1);
    assert(context->isBarrier == 1);
    assert(context->barrierTaskTag == barrierTaskTag);
    
    if (0 == context->tasks.count) {
        _CEConcurrentSourcePushScheduler(source, context, scheduler);
        context->executingCount -= 1;
        context->isBarrier = 0;
        CESpinLockUnlock(source->lock);
        CESemWait(scheduler->waiter);
    } else {
        _Bool keepBarrier = false;
        result = CESourceTaskStoreRemove(&(context->tasks));
        assert(result);
        if (1 == result->isBarrier) {
            keepBarrier = true;
        } else {
            if (NULL != context->tasks.head) {
                if (1 == context->tasks.head->isBarrier) {
                    keepBarrier = true;
                }
            }
        }
        if (!keepBarrier) {
            weakUpCount = context->bufferCount;
            if (weakUpCount > context->tasks.count) {
                weakUpCount = (uint32_t)(context->tasks.count);
            }
            CETaskPtr tmp = context->tasks.head;
            
            for (uint32_t index=0; index<weakUpCount; index++) {
                assert(tmp);
                if (0 != tmp->isBarrier) {
                    weakUpCount = index;
                    break;
                }
                tmp = tmp->next;
            }
            _CEConcurrentSourcePopSchedulers(source, context, weakUpCount, schedulersBuffer);
        }
        CESpinLockUnlock(source->lock);
        
        for (uint32_t index=0; index<weakUpCount; index++) {
            CETaskSchedulerPtr s = schedulersBuffer[index];
            assert(s);
            
            _CEConcurrentTaskSchedulerSignal(s);
        }
        return result;
    }
   
    return CEConcurrentSourceRemove(source, scheduler);
}

static inline CETaskPtr _Nonnull CEConcurrentSourceFinishNormalTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler) {
    
    CETaskPtr result = NULL;
    CESourceConcurrentContext_s * context = source->context;
    
    CESpinLockLock(source->lock);
    
    if (0 == context->tasks.count) {
        _CEConcurrentSourcePushScheduler(source, context, scheduler);
        context->executingCount -= 1;
        CESpinLockUnlock(source->lock);
        CESemWait(scheduler->waiter);
    } else {
        if (0 == context->isBarrier) {//当前不在屏障状态下
            result = CESourceTaskStoreRemove(&(context->tasks));
            
            assert(0 == result->isBarrier);
            if (NULL != context->tasks.head) {
                if (1 == context->tasks.head->isBarrier) {
                    context->isBarrier = 1;
                }
            }
            CESpinLockUnlock(source->lock);
            return result;
        } else {//当前在屏障状态下
            if (context->bufferCount + 1 == context->maxConcurrencyCount) {
                result = CESourceTaskStoreRemove(&(context->tasks));
                
                if (0 != result->isBarrier) {
                    context->isBarrier = 1;
                } else {
                    if (NULL != context->tasks.head) {
                        if (1 == context->tasks.head->isBarrier) {
                            context->isBarrier = 1;
                        } else {
                            context->isBarrier = 0;
                        }
                    } else {
                        context->isBarrier = 0;
                    }
                }
                CESpinLockUnlock(source->lock);
                return result;
            } else {
                _CEConcurrentSourcePushScheduler(source, context, scheduler);
                context->executingCount -= 1;
                CESpinLockUnlock(source->lock);
                CESemWait(scheduler->waiter);
            }
        }
    }
    
    return CEConcurrentSourceRemove(source, scheduler);
}


CETaskPtr _Nonnull CEConcurrentSourceFinishOneTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler, uint32_t isBarrierTask, uint32_t barrierTaskTag) {
    if (0 != isBarrierTask) {
        return CEConcurrentSourceFinishBarrierTaskAndRemove(source, scheduler, barrierTaskTag);
    } else {
        return CEConcurrentSourceFinishNormalTaskAndRemove(source, scheduler);
    }
}


//CETaskPtr _Nonnull CEConcurrentSourceFinishOneTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler, uint32_t isBarrierTask) {
//    if (0 != isBarrierTask) {
//        isBarrierTask = 1;
//    }
//
//    CETaskSchedulerPtr _Nonnull schedulersBuffer[256] = {};
//    uint32_t weakUpCount = 0;
//
//    int action = 0;
//    CETaskPtr result = NULL;
//    CESourceConcurrentContext_s * context = source->context;
//
//    //移除任务会改变 isBarrier
//    CESpinLockLock(source->lock);
//
//    uint32_t step = 0;
//
//    CEBarrierChange_e barrierChange = 0;
//
//    if (isBarrierTask) {//屏障task 执行完成
//        assert(context->isBarrier);
//        result = CESourceTaskStoreRemove(&(context->tasks));
//
//        if (NULL == result) {
//            barrierChange = CEBarrierChangeLeave;
//
//
//            context->isBarrier = 0;
//            context->executingCount -= 1;
//
//            //挂起
//            action = -1;
//
//        } else {
//
//
//        }
//
//    }
//    //进入屏障、持续屏障、取消屏障
//
//    if (isBarrierTask) {//屏障task 执行完成
//        assert(context->isBarrier);
//        result = CESourceTaskStoreRemove(&(context->tasks));
//        if (NULL == result) {
//            context->isBarrier = 0;
//            context->executingCount -= 1;
//
//            //挂起
//            action = -1;
//
//        } else {
//            uint32_t headTaskIsBarrier = context->tasks.head->isBarrier;
//            if (0 == headTaskIsBarrier) { //解除屏障
//                context->isBarrier = 0;
//
//                //唤醒更多线程
//                action = 1;
//
//                uint32_t loopCount = context->maxConcurrencyCount - context->executingCount;
//                if (loopCount > context->tasks.count) {
//                    loopCount = (uint32_t)(context->tasks.count);
//                }
//
//                CETaskPtr tmp = context->tasks.head;
//                weakUpCount = loopCount;
//
//                for (uint32_t index=0; index<loopCount; index++) {
//                    assert(tmp);
//                    if (0 != tmp->isBarrier) {
//                        weakUpCount = index;
//                        break;
//                    }
//                    tmp = tmp->next;
//                }
//
//                assert(weakUpCount <= context->bufferCount);
//
//                memcpy(schedulersBuffer, &(context->schedulersBuffer[context->bufferCount - weakUpCount]), sizeof(CETaskSchedulerPtr) * weakUpCount);
//
//                context->bufferCount -= weakUpCount;
//            }
//        }
//    } else {
//        //非屏障task 执行完成
//        if (0 == context->isBarrier) {//当前不在屏障状态下
//            result = CESourceTaskStoreRemove(&(context->tasks));
//            if (NULL == result) {
//                context->executingCount -= 1;
//
//                //挂起
//                action = -1;
//            } else {
//                uint32_t headTaskIsBarrier = result->isBarrier;
//                if (0 != headTaskIsBarrier) { //开始屏障
//                    context->isBarrier = 1;
//                }
//            }
//        } else {
//            context->executingCount -= 1;
//
//            //挂起
//
//            action = -1;
//        }
//    }
//
//    if (action < 0) {
//        context->currentConcurrencyCount -= 1;
//        context->schedulersBuffer[context->bufferCount] = scheduler;
//        context->bufferCount += 1;
//    }
//
//    CESpinLockUnlock(source->lock);
//
//
//    if (NULL != result) {
//        if (action > 1) {
//            for (uint32_t index=0; index<weakUpCount; index++) {
//                CETaskSchedulerPtr s = schedulersBuffer[index];
//                assert(s);
//                CESemSignal(s->waiter);
//            }
//        }
//        return result;
//    } else {
//        if (action < 0) {
//            CESemWait(scheduler->waiter);
//        }
//        while (NULL == result) {
//            CESpinLockLock(source->lock);
//            _Bool wait = false;
//            if (0 == context->isBarrier) {//当前不在屏障状态下
//                result = CESourceTaskStoreRemove(&(context->tasks));
//                if (NULL == result) {
//                    context->executingCount -= 1;
//
//                    //挂起
//                    wait = true;
//                } else {
//                    uint32_t headTaskIsBarrier = context->tasks.head->isBarrier;
//                    if (0 != headTaskIsBarrier) { //开始屏障
//                        context->isBarrier = 1;
//                    }
//                }
//            } else {
//                context->executingCount -= 1;
//
//                //挂起
//                wait = true;
//            }
//            if (wait) {
//                context->currentConcurrencyCount -= 1;
//                context->schedulersBuffer[context->bufferCount] = scheduler;
//                context->bufferCount += 1;
//            }
//            CESpinLockUnlock(source->lock);
//            if (wait) {
//                CESemWait(scheduler->waiter);
//            }
//        }
//        return result;
//    }
//}



CETaskPtr _Nonnull CEConcurrentSourceRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler) {
    assert(source);
    assert(scheduler);
    
    CESourceConcurrentContext_s * context = source->context;
    CETaskPtr result = NULL;
    
    while (NULL == result) {
        CESpinLockLock(source->lock);
        
        if (context->tasks.count >= 1) {
            if (0 == context->isBarrier) {//当前不在屏障状态下
                result = CESourceTaskStoreRemove(&(context->tasks));
                
                assert(0 == result->isBarrier);
                if (NULL != context->tasks.head) {
                    if (1 == context->tasks.head->isBarrier) {
                        context->isBarrier = 1;
                    }
                }
                context->executingCount += 1;
                CESpinLockUnlock(source->lock);
                return result;
            } else {//当前在屏障状态下
                if (context->bufferCount + 1 == context->maxConcurrencyCount) {
                    //当前是屏障线程
                    result = CESourceTaskStoreRemove(&(context->tasks));
                    
                    if (0 != result->isBarrier) {
                        context->isBarrier = 1;
                    } else {
                        if (NULL != context->tasks.head) {
                            if (1 == context->tasks.head->isBarrier) {
                                context->isBarrier = 1;
                            } else {
                                context->isBarrier = 0;
                            }
                        } else {
                            context->isBarrier = 0;
                        }
                    }
                    context->executingCount += 1;
                    CESpinLockUnlock(source->lock);
                    
                    return result;
                } else {
                    _CEConcurrentSourcePushScheduler(source, context, scheduler);
                    context->executingCount -= 1;
                    CESpinLockUnlock(source->lock);
                    CESemWait(scheduler->waiter);
                }
            }
        } else {
            _CEConcurrentSourcePushScheduler(source, context, scheduler);
            context->executingCount -= 1;
            CESpinLockUnlock(source->lock);
            CESemWait(scheduler->waiter);
        }
    }
    return result;
}

void CEConcurrentQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific) {

    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    CEQueueLog("ce.task.scheduler.bm.wait(qid:%x, sid:%x)", scheduler->qid, scheduler->sid);
    CESemWait(scheduler->waiter);
    CEQueueLog("ce.task.scheduler.bm.wake(qid:%x, sid:%x)", scheduler->qid, scheduler->sid);

    assert(scheduler->ownerQueue);
    CETaskSchedulerSetThread(scheduler, specific->thread);
    specific->owner = scheduler->ownerQueue;
    specific->syncWaiter->queue = scheduler->ownerQueue;
    CEQueueLog("ce.task.scheduler.bm.finish(qid:%x, sid:%x)", scheduler->qid, scheduler->sid);
}

void CEConcurrentQueueMainFunc(void * _Nullable param) {
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    CEQueueLog("ce.task.scheduler.m.start(qid:%x, sid:%x)", scheduler->qid, scheduler->sid);

    CETaskPtr task = CEConcurrentSourceRemove(scheduler->source, scheduler);
    CEQueueLog("ce.task.scheduler.m.first.finish(qid:%x, sid:%x)", scheduler->qid, scheduler->sid);

    while (1) {
        CETaskSchedulerExecuteTask(scheduler, task);
        if (task->finish) {
            task->finish(task->obj);
        }
        uint32_t isBarrier = task->isBarrier;
        uint32_t barrierTaskTag = task->tag;
        CETaskDestroy(task);
        
        CEQueueLog("ce.task.scheduler.m.next(qid:%x, sid:%x)", scheduler->qid, scheduler->sid);

        task = CEConcurrentSourceFinishOneTaskAndRemove(scheduler->source, scheduler, isBarrier, barrierTaskTag);
        CEQueueLog("ce.task.scheduler.m.next.geted(qid:%x, sid:%x, %p)", scheduler->qid, scheduler->sid, task);

    }
}

CESource_s * _Nonnull CEConcurrentSourceCreate(CEQueue_s * _Nonnull queue) {
    //source
    assert(queue);
    CESourceConcurrentContext_s * context = CEAllocateClear(sizeof(CESourceConcurrentContext_s));
    CESource_s * source = CESourceCreate(queue, context, CEConcurrentSourceAppend);
    
    uint16_t concurrencyCount = queue->concurrencyCount;
    for (uint16_t index=1; index<=concurrencyCount; index++) {
        CETaskSchedulerPtr scheduler = CETaskSchedulerCreate(queue->id, index);
        scheduler->source = source;
        scheduler->ownerQueue = queue;
        
        uint16_t offset = index-1;
        context->schedulers[offset] = scheduler;
        context->schedulersBuffer[offset] = scheduler;
    }
    context->maxConcurrencyCount = concurrencyCount;
    context->bufferCount = concurrencyCount;
    context->isBarrier = 0;
    context->executingCount = 0;
    context->xxx = 0;
    context->barrierTaskTag = 0;
    return source;
}

//queue
CEQueue_s * _Nonnull _CEConcurrentQueueCreate(const char * _Nullable label, CEQueuePriority_t priority, uint16_t concurrencyCount) {
    assert(concurrencyCount > 0);
    assert(concurrencyCount <= 256);

    CEQueue_s * queue = CEQueueCreate(label, concurrencyCount, priority, CEQueueTypeConcurrent, CEQueueSequenceNext());
    queue->source = CEConcurrentSourceCreate(queue);    
    CEThreadConfig_s config = {};
    memcpy(config.name, queue->label, 64);
    float schedPriority = CEQueuePriorityToThreadPriority(queue->priority);
    config.schedPriority = schedPriority;
    return queue;
}

CEQueueRef _Nonnull CEConcurrentQueueCreate(const char * _Nullable label, CEQueuePriority_t priority, uint16_t concurrencyCount) {
    return _CEConcurrentQueueCreate(label, priority, concurrencyCount);
}
