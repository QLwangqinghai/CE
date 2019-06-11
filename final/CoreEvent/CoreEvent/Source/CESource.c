//
//  CESource.c
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CESource.h"
#include "CEMemory.h"


CESource_s * _Nonnull CESourceCreate(CEQueue_s * _Nonnull queue, CEPtr _Nonnull context, CESourceAppendTask_f _Nonnull append) {
    assert(queue);
    assert(context);
    assert(append);
    
    
    CESource_s * source = CEAllocateClear(sizeof(CESource_s));
    source->queue = queue;
    source->context = context;
    source->append = append;
    source->lock = CESpinLockCreate();
    return source;
}

void CESourceDestroy(CESource_s * _Nonnull source) {
    assert(source);
    CESpinLockDestroy(source->lock);
    CEDeallocate(source);
}
