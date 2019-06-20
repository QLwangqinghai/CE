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

void CEConcurrentQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific);

void CEConcurrentQueueMainFunc(void * _Nullable param);


static inline CETaskSchedulerPtr _Nonnull _CEConcurrentSourceGetASchedulerToSignal(CESourceRef _Nonnull source, CESourceConcurrentContext_s * context) {
    assert(context->bufferCount > 0);
    CETaskSchedulerPtr result = context->schedulersBuffer[context->bufferCount - 1];
    context->bufferCount -= 1;
    return result;
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
                
                if (0 == context->executingCount) {//没有正在执行中的任务， 唤醒线程以执行屏障任务
                    scheduler = _CEConcurrentSourceGetASchedulerToSignal(source, context);
                }
            } else {
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
        //else 当前在屏障状态下 不再唤醒新线程
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
        if (NULL == CETaskSchedulerGetThread(scheduler)) {
            CEThreadConfig_s config = {};
            snprintf(config.name, 64, "%s.%x", source->queue->label, scheduler->sid);
            float schedPriority = CEQueuePriorityToThreadPriority(source->queue->priority);
            config.schedPriority = schedPriority;
            _CEThreadCreate(config, scheduler, CEConcurrentQueueBeforeMainFunc, CEConcurrentQueueMainFunc, NULL, NULL);
            CESemSignal(scheduler->waiter);
        }
        CESemSignal(scheduler->waiter);
    }
}

CETaskPtr _Nonnull CEConcurrentSourceFinishOneTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler, uint32_t isBarrierTask) {
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
                uint32_t headTaskIsBarrier = result->isBarrier;
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

    CETaskPtr result = NULL;
    
    while (NULL == result) {
        CESemWait(scheduler->waiter);
        CESpinLockLock(source->lock);
        result = CESourceTaskStoreRemove(&(context->tasks));
        CESpinLockUnlock(source->lock);
    }
    
    assert(result);
    return result;
}




void CEConcurrentQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific) {
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    CESemWait(scheduler->waiter);
    assert(scheduler->ownerQueue);
    CETaskSchedulerSetThread(scheduler, specific->thread);
    specific->owner = scheduler->ownerQueue;
    specific->syncWaiter->queue = scheduler->ownerQueue;
}

void CEConcurrentQueueMainFunc(void * _Nullable param) {
    CEThreadSpecificPtr specific = CEThreadSpecificGetCurrent();
    CETaskSchedulerPtr scheduler = specific->scheduler;
    assert(scheduler);
    
    CETaskPtr task = CEConcurrentSourceRemove(scheduler->source, scheduler);
    while (1) {
        CETaskSchedulerExecuteTask(scheduler, task);
        if (task->finish) {
            task->finish(task->obj);
        }
        uint32_t isBarrier = task->isBarrier;
        CETaskDestroy(task);
        task = CEConcurrentSourceFinishOneTaskAndRemove(scheduler->source, scheduler, isBarrier);
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
    context->privateSchedulerCount = concurrencyCount;
    context->maxConcurrencyCount = concurrencyCount;
    context->currentConcurrencyCount = 0;
    context->bufferCount = concurrencyCount;
    context->isBarrier = 0;
    context->executingCount = 0;

    return source;
}

//queue
CEQueue_s * _Nonnull _CEConcurrentQueueCreate(const char * _Nullable label, CEQueuePriority_t priority, uint16_t concurrencyCount) {
    assert(concurrencyCount > 0);
    assert(concurrencyCount <= 256);

    CEQueue_s * queue = CEQueueCreate(label, concurrencyCount, priority, CEQueueTypeSerial, CEQueueSequenceNext());
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
