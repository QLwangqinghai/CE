//
//  CEConcurrentQueue.c
//  CoreEvent
//
//  Created by vector on 2019/6/10.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEConcurrentQueue.h"


static inline CETaskSchedulerPtr _Nonnull _CEConcurrentSourceGetASchedulerToSignal(CESourceRef _Nonnull source, CESourceConcurrentContext_s * context) {
    assert(context->bufferCount > 0);
    CETaskSchedulerPtr result = context->schedulersBuffer[context->bufferCount - 1];
    context->bufferCount -= 1;
    return result;
}

void CEConcurrentSourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    CESourceConcurrentContext_s * context = source->context;

    assert(CESourceConcurrentContextTypeCustom == context->type);
    
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
        CESemSignal(scheduler->waiter);
    }
}

CETaskPtr _Nonnull CEConcurrentSourceFinishOneTaskAndRemove(CESourceRef _Nonnull source, CETaskSchedulerPtr _Nonnull scheduler, uint32_t isBarrierTask) {
    if (0 != isBarrierTask) {
        isBarrierTask = 1;
    }
    CETaskPtr result;
    CESourceConcurrentContext_s * context = source->context;
    
    assert(CESourceConcurrentContextTypeCustom == context->type);
    
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
            
            
        } else {
            uint32_t headTaskIsBarrier = context->tasks.head->isBarrier;
            if (0 == headTaskIsBarrier) { //解除屏障
                context->isBarrier = 0;
                
                //唤醒更多线程
                
            }
        }
    } else {
        //非屏障task 执行完成
        if (0 == context->isBarrier) {//当前不在屏障状态下
            result = CESourceTaskStoreRemove(&(context->tasks));
            if (NULL == result) {
                context->executingCount -= 1;
                
                //挂起
                
                
            } else {
                uint32_t headTaskIsBarrier = context->tasks.head->isBarrier;
                if (0 != headTaskIsBarrier) { //开始屏障
                    context->isBarrier = 1;
                }
            }
        } else {
            context->executingCount -= 1;
            
            //挂起
            
            
        }
    }
    


    
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
        CESemSignal(scheduler->waiter);
    }
    
    
    
    
    
    
    
    
    
    CESourceCount_t count = 0;
    
    CESourceConcurrentContext_s * context = source->context;
    CESpinLockLock(source->lock);
    context->executingCount -= 1;
    count = context->count;
    CESpinLockUnlock(source->lock);
    
    if (count == 0) {
        // wait
        
        CESemWait(context->scheduler->waiter);
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
