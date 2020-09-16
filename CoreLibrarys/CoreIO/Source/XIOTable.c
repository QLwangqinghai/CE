//
//  XIOTable.c
//  CoreIO
//
//  Created by vector on 2020/7/30.
//  Copyright © 2020 com.wangqinghai. All rights reserved.
//

#include "XIOTable.h"


struct __XIOConnect {
    
    //fired events
    //    uint32_t valid:  1;
    uint32_t firedMask: 4;
    uint32_t mask: 4;
    uint32_t apiRegistered: 1;
    uint32_t apiMask: 4;//当前mask
    uint32_t readTimerSequence: 11;
    uint32_t writeTimerSequence: 11;
    
    CEFileSourceGroupRef _Nonnull readGroup;
    CEFileSourceGroupRef _Nonnull writeGroup;
    
    CEFileEventCallback_f _Nonnull callback;

    CEFileEventHandler_f _Nonnull readHandle;
    CEFileEventHandler_f _Nonnull writeHandle;

    void * _Nullable clientData;
};
