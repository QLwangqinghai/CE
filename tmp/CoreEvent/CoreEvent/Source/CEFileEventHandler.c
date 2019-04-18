//
//  CEFileEventHandler.c
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEFileEventHandler.h"
#include <pthread.h>
#include "zmalloc.h"

typedef struct _CEFileEventHandlerStoreItem {
    _Atomic(uintptr_t) readFunc;
    _Atomic(uintptr_t) writeFunc;
} CEFileEventHandlerStoreItem_s;


#define CEFileEventHandlerStoreBufferSize 2048

_Atomic(uint_fast32_t) CEFileEventHandlerStoreSize = 0;

_Atomic(uintptr_t) CEFileEventHandlerStore[CEFileEventHandlerStoreBufferSize] = {};


uint32_t CEFileEventHandlerRegister(CEFileEventHandler_f _Nonnull handler) {
    assert(handler);
    
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    

    uint32_t result = 0;
    pthread_mutex_lock(&mutex);
    
    uint32_t size = atomic_load(&CEFileEventHandlerStoreSize);
    if (size + 1 < CEFileEventHandlerStoreBufferSize) {
        result = size + 1;

        _Atomic(uintptr_t) * store = &CEFileEventHandlerStore[size];
        atomic_store(store, (uintptr_t)handler);
        atomic_store(&CEFileEventHandlerStoreSize, result);
    }
    pthread_mutex_unlock(&mutex);
    
    return result;
}


CEFileEventHandler_f _Nullable CEFileEventHandlerGet(uint32_t identifier) {
    uint32_t size = atomic_load(&CEFileEventHandlerStoreSize);
    if (identifier != 0 && identifier <= size) {
        _Atomic(uintptr_t) * store = &CEFileEventHandlerStore[size];
        return (CEFileEventHandler_f)atomic_load(store);
    }
    return NULL;
}

