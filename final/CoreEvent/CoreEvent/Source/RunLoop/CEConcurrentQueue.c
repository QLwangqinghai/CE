//
//  CEConcurrentQueue.c
//  CoreEvent
//
//  Created by vector on 2019/6/10.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEConcurrentQueue.h"


void CEConcurrentSourceHighAppend(CESourceRef _Nonnull source, CESourceConcurrentContext_s * context, CETaskPtr _Nonnull task) {
    
    _Bool doWeakUp = false;
    CESourceCount_t count = 0;
    CESpinLockLock(source->lock);
    CESourceTaskStoreAppend(&(context->tasks), task);
    count = context->tasks.count;
    
    if (count + context->executingCount) {
        doWeakUp = true;
    } else {
        
    }
    CESpinLockUnlock(source->lock);
    
    
    if (count == 1) {
        //weak up
        
    }
}
void CEConcurrentSourceDefaultAppend(CESourceRef _Nonnull source, CESourceConcurrentContext_s * context, CETaskPtr _Nonnull task) {
    
    _Bool doWeakUp = false;
    CESourceCount_t count = 0;
    CESpinLockLock(source->lock);
    CESourceTaskStoreAppend(&(context->tasks), task);
    count = context->tasks.count;

    if (count == 1 && context->executingCount==0) {
        doWeakUp = true;
    } else {
        
    }
    CESpinLockUnlock(source->lock);
    
    
    if (count == 1) {
        //weak up
        
    }
}
void CEConcurrentSourceLowAppend(CESourceRef _Nonnull source, CESourceConcurrentContext_s * context, CETaskPtr _Nonnull task) {
    
    _Bool doWeakUp = false;
    CESourceCount_t count = 0;
    CESpinLockLock(source->lock);
    CESourceTaskStoreAppend(&(context->tasks), task);
    count = context->tasks.count;

    if (count == 1 && context->executingCount==0) {
        doWeakUp = true;
    } else {
        
    }
    CESpinLockUnlock(source->lock);
    
    
    if (count == 1) {
        //weak up
        
    }
}

void CEConcurrentSourceCustomAppend(CESourceRef _Nonnull source, CESourceConcurrentContext_s * context, CETaskPtr _Nonnull task) {
    //添加任务最多只能唤醒一个线程
    int weakUpIndex = -1;
    CESourceCount_t count = 0;
    CESpinLockLock(source->lock);
    CESourceTaskStoreAppend(&(context->tasks), task);
    count = context->tasks.count;

    if (context->currentConcurrencyCount < context->maxConcurrencyCount) {
        //链表中存储的task+正在执行的 task个数
        CESourceCount_t totalCount = count + context->executingCount;
        if (totalCount > context->currentConcurrencyCount) {
            //do weak up a taskScheduler
            
            
            
            
        }
    }
    
    CESpinLockUnlock(source->lock);
    
    
    if (count == 1) {
        //weak up
        
    }
}



void CEConcurrentSourceAppend(CESourceRef _Nonnull source, CETaskPtr _Nonnull task) {
    CESourceConcurrentContext_s * context = source->context;

    assert(CESourceConcurrentContextTypeCustom >= context->type && CESourceConcurrentContextTypeGlobalHigh <= context->type);
    
    
    switch (context->type) {
        case CESourceConcurrentContextTypeGlobalHigh: {
            CEConcurrentSourceHighAppend(source, context, task);
        }
            break;
        case CESourceConcurrentContextTypeGlobalDefault: {
            CEConcurrentSourceDefaultAppend(source, context, task);
        }
            break;
        case CESourceConcurrentContextTypeGlobalLow: {
            CEConcurrentSourceLowAppend(source, context, task);
        }
            break;
        case CESourceConcurrentContextTypeCustom: {
            CEConcurrentSourceCustomAppend(source, context, task);
        }
            break;
        default: {
            abort();
        }
            break;
    }
}

void CEConcurrentSourceFinishOneTask(CESourceRef _Nonnull source) {
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

CETaskPtr _Nonnull CEConcurrentSourceRemove(CESourceRef _Nonnull source) {
    assert(source);
    
    CETaskPtr result = NULL;
    CESpinLockLock(source->lock);
    CESourceConcurrentContext_s * context = source->context;
    result = CESourceTaskStoreRemove(&(context->tasks));
    CESpinLockUnlock(source->lock);
    assert(result);
    return result;
}
