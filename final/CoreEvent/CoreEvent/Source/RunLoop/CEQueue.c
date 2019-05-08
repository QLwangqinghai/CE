//
//  CEQueue.c
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEQueue.h"
#include "CEThread.h"
#include "CEThreadBaseInternal.h"


void CEQueueSerialSync(CEQueueRef _Nonnull queue, CENoescapingTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {
    
    
    
    
}
void CEQueueConcurrentSync(CEQueueRef _Nonnull queue, CENoescapingTaskRef _Nonnull task, CEThreadSpecificRef _Nonnull specific) {

    
    
    
}


void CEQueueSync(CEQueueRef _Nonnull queue, CENoescapingTaskRef _Nonnull task) {
    assert(queue);
    assert(task);
    
    CEThreadSpecificRef specific = CEThreadSpecificGetCurrent();

    if (queue->base.maxConcurrentCount <= 1) {
        CEQueueSerialSync(queue, task, specific);
    } else {
        CEQueueConcurrentSync(queue, task, specific);
    }
}
