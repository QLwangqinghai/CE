//
//  CETaskContext.c
//  CoreEvent
//
//  Created by vector on 2019/5/22.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CETaskContext.h"

CETaskContextPtr _Nonnull CETaskContextGetCurrent(void) {
    return NULL;
}

CETaskContext_s CETaskContexPush(void) {
    CETaskContext_s context = {};
    context.prev = CETaskContextGetCurrent();
    
    return context;
}
