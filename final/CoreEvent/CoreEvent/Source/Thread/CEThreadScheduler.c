//
//  CEThreadScheduler.c
//  CoreEvent
//
//  Created by vector on 2019/6/1.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEThreadScheduler.h"

const CETypeSpecific_s CETypeSpecificThreadScheduler = {
    .name = "CEThreadScheduler",
    .descript = CETypeDefaultDescript,
    .deinit = CERuntimeDefaultDeinit,
};

const CEType_s __CETypeThreadScheduler = CEType(CETypeBitHasRc | CETypeBitStatic, 0, (uintptr_t)(&__CETypeThreadScheduler), &CETypeSpecificThreadScheduler);


CETypeRef _Nonnull CETypeThreadScheduler = &__CETypeThreadScheduler;
