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

static inline void _CEConcurrentSourceTaskStoreAppend(CESourceConcurrentContext_s * _Nonnull context, CETaskPtr _Nonnull task) {
    CESourceTaskStoreAppend(&(context->tasks), task);
    context->headIsBarrier = task->isBarrier;
}

static inline CETaskPtr _Nonnull __CEConcurrentSourceTaskStoreRemove(CESourceConcurrentContext_s * _Nonnull context) {
    CETaskPtr result = CESourceTaskStoreRemove(&(context->tasks));
    assert(result);
    assert(0 == context->executingBarrier);
    if (NULL != context->tasks.head) {
        context->headIsBarrier = context->tasks.head->isBarrier;
    } else {
        context->headIsBarrier = 0;
    }
    
    context->executingBarrier = result->isBarrier;
    if (result->isBarrier) {
        context->barrierTaskTag = result->tag;
    }
    return result;
}

static inline void __CEConcurrentSourceTaskStoreFinishBarrierTask(CESourceConcurrentContext_s * _Nonnull context, uint32_t tag) {
    assert(0 != context->executingBarrier);
    assert(tag == context->barrierTaskTag);
    context->executingBarrier = 0;
    context->barrierTaskTag = 0;
}



void CEConcurrentQueueBeforeMainFunc(CEThreadSpecificPtr _Nonnull specific);
void CEConcurrentQueueMainFunc(void * _Nullable param);

static inline void _CEConcurrentTaskSchedulerSignal(CETaskSchedulerPtr _Nonnull scheduler, CETaskPtr _Nonnull task) {
    CEQueueLog("ce.task.scheduler.signal(qid:%x, sid:%x, tag:%x)", scheduler->qid, scheduler->sid, task->tag);
    CETaskSchedulerStoreTask(scheduler, task);
    if (NULL == CETaskSchedulerGetThread(scheduler)) {
        CEQueueLog("ce.task.scheduler.signal.create_thread(qid:%x, sid:%x)", scheduler->qid, scheduler->sid);

        CEThreadConfig_s config = {};
        snprintf(config.name, 64, "%s.%x", scheduler->source->queue->label, scheduler->sid);
        float schedPriority = CEQueuePriorityToThreadPriority(scheduler->source->queue->priority);
        config.schedPriority = schedPriority;
        _CEThreadCreate(config, scheduler, CEConcurrentQueueBeforeMainFunc, CEConcurrentQueueMainFunc, NULL, NULL);
        CESemSignal(scheduler->waiter);
    }
    CESemSignal(scheduler->waiter);
}

static inline CETaskPtr _Nonnull _CEConcurrentTaskSchedulerWait(CETaskSchedulerPtr _Nonnull scheduler) {
    CEQueueLog("ce.task.scheduler.wait(qid:%x, sid:%x)", scheduler->qid, scheduler->sid);
    CESemWait(scheduler->waiter);
    return CETaskSchedulerRemoveTask(scheduler);
}

static inline CETaskSchedulerPtr _Nonnull _CEConcurrentSourcePopScheduler(CESourceRef _Nonnull source, CESourceConcurrentContext_s * _Nonnull context) {
    assert(context->bufferCount > 0);
    CETaskSchedulerPtr result = context->schedulersBuffer[context->bufferCount - 1];
    context->bufferCount -= 1;
    return result;
}

//static inline void _CEConcurrentSourcePopSchedulers(CESourceRef _Nonnull source, CESourceConcurrentContext_s * _Nonnull context, uint32_t count, CETaskSchedulerPtr _Nullable * _Nonnull schedulers) {
//    assert(context->bufferCount >= count);
//
//    memcpy(schedulers, &(context->schedulersBuffer[context->bufferCount - count]), sizeof(CETaskSchedulerPtr) * count);
//    context->bufferCount -= count;
//}

static inline void _CEConcurrentSourcePushScheduler(CESourceRef _Nonnull source, CESourceConcurrentContext_s * _Nonnull context, CETaskSchedulerPtr _Nonnull scheduler) {
    assert(context->bufferCount < context->maxConcurrencyCount);
    context->schedulersBuffer[context->bufferCount] = scheduler;
    context->bufferCount += 1;
}

void CEConcurrentSourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    CESourceConcurrentContext_s * context = source->context;
    
    //添加任务最多只能唤醒一个线程
    CETaskSchedulerPtr scheduler = NULL;
    CESpinLockLock(source->lock);
    
    if (CESourceConcurrentContextIsBarrier(context)) {
        assert(context->bufferCount < context->maxConcurrencyCount);
        _CEConcurrentSourceTaskStoreAppend(context, task);
    } else {
        if (task->isBarrier) {
            _CEConcurrentSourceTaskStoreAppend(context, task);
            if (context->bufferCount == context->maxConcurrencyCount) {
                CETaskPtr _Nonnull tmp = __CEConcurrentSourceTaskStoreRemove(context);
                assert(tmp == task);
                scheduler = _CEConcurrentSourcePopScheduler(source, context);
            }
        } else {
            if (context->bufferCount > 0) {
                scheduler = _CEConcurrentSourcePopScheduler(source, context);
            } else {
                _CEConcurrentSourceTaskStoreAppend(context, task);
            }
        }
    }
    CESpinLockUnlock(source->lock);
    
    if (NULL != scheduler) {
        _CEConcurrentTaskSchedulerSignal(scheduler, task);
    }
}


static inline CETaskPtr _Nonnull CEConcurrentSourceFinishBarrierTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler, uint32_t barrierTaskTag) {

    CETaskSchedulerPtr _Nonnull schedulersBuffer[256] = {};
    CETaskPtr _Nonnull taskBuffer[256] = {};
    
    uint32_t weakUpCount = 0;
    
    CETaskPtr result = NULL;
    CESourceConcurrentContext_s * context = source->context;
    
    //移除任务会改变 isBarrier
    CESpinLockLock(source->lock);
    
    assert(CESourceConcurrentContextIsBarrier(context));
    assert(context->barrierTaskTag == barrierTaskTag);
    
    __CEConcurrentSourceTaskStoreFinishBarrierTask(context, barrierTaskTag);

    if (0 == context->tasks.count) {
        _CEConcurrentSourcePushScheduler(source, context, scheduler);
        CESpinLockUnlock(source->lock);
        return _CEConcurrentTaskSchedulerWait(scheduler);
    } else {
        result = __CEConcurrentSourceTaskStoreRemove(context);
        if (!CESourceConcurrentContextIsBarrier(context)) {
            weakUpCount = context->bufferCount;
            if (weakUpCount > context->tasks.count) {
                weakUpCount = (uint32_t)(context->tasks.count);
            }
            
            for (uint32_t index=0; index<weakUpCount; index++) {
                if (!CESourceConcurrentContextIsBarrier(context)) {
                    taskBuffer[index] = __CEConcurrentSourceTaskStoreRemove(context);
                    schedulersBuffer[index] = _CEConcurrentSourcePopScheduler(source, context);
                } else {
                    weakUpCount = index;
                    break;
                }
            }
        }
        CESpinLockUnlock(source->lock);
        
        for (uint32_t index=0; index<weakUpCount; index++) {
            CETaskSchedulerPtr s = schedulersBuffer[index];
            CETaskPtr t = taskBuffer[index];
            assert(s);
            assert(t);
            
            _CEConcurrentTaskSchedulerSignal(s, t);
        }
        return result;
    }
}

static inline CETaskPtr _Nonnull CEConcurrentSourceFinishNormalTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler) {
    CETaskPtr result = NULL;
    CESourceConcurrentContext_s * context = source->context;
    
    CESpinLockLock(source->lock);
    
    if (0 == context->tasks.count) {
        _CEConcurrentSourcePushScheduler(source, context, scheduler);
        CESpinLockUnlock(source->lock);
        return _CEConcurrentTaskSchedulerWait(scheduler);
    } else {
        if (!CESourceConcurrentContextIsBarrier(context)) {//当前不在屏障状态下
            result = __CEConcurrentSourceTaskStoreRemove(context);
            CESpinLockUnlock(source->lock);
            return result;
        } else {//当前在屏障状态下
            if (context->bufferCount + 1 == context->maxConcurrencyCount) {
                result = __CEConcurrentSourceTaskStoreRemove(context);
                CESpinLockUnlock(source->lock);
                return result;
            } else {
                _CEConcurrentSourcePushScheduler(source, context, scheduler);
                CESpinLockUnlock(source->lock);
                return _CEConcurrentTaskSchedulerWait(scheduler);
            }
        }
    }
}


CETaskPtr _Nonnull CEConcurrentSourceFinishOneTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler, uint32_t isBarrierTask, uint32_t barrierTaskTag) {
    if (0 != isBarrierTask) {
        return CEConcurrentSourceFinishBarrierTaskAndRemove(source, scheduler, barrierTaskTag);
    } else {
        return CEConcurrentSourceFinishNormalTaskAndRemove(source, scheduler);
    }
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
    CETaskPtr task = _CEConcurrentTaskSchedulerWait(scheduler);
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
    context->executingBarrier = 0;
    context->headIsBarrier = 0;
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
