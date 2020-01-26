//
//  CCAllocator.h
//  SITP
//
//  Created by vector on 2019/12/24.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCAllocator_h
#define CCAllocator_h

#include "CCType.h"
#include <stdio.h>
#include <wchar.h>
#include <stdatomic.h>
#include <stdlib.h>


static inline CCPtr _Nonnull CCMalloc(size_t size) {
    assert(size >= 0);
    if (size <= 0) {
        size = 4;
    }
    CCPtr ptr = malloc(size);
    assert((((uintptr_t)ptr) & 0x3) == 0);
    return ptr;
}
static inline void CCFree(CCPtr _Nonnull ptr) {
    free(ptr);
}

static inline CCPtr _Nullable CCTryAllocate(size_t size) {
    if (size < 0) {
        return NULL;
    }
    CCPtr ptr = CCMalloc(size);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

static inline void * _Nonnull CCAllocate(size_t size) {
    if (size <= 0) {
        size = 4;
    }
    CCPtr ptr = CCMalloc(size);
    if (NULL == ptr) {
        CCLogError("CCAllocate ptr is NULL\n");
        abort();
    }
    memset(ptr, 0, size);
    return ptr;
}

static inline void CCDeallocate(void * _Nonnull ptr) {
    if (NULL == ptr) {
        CCLogError("CCDeallocate ptr is NULL\n");
        abort();
    }
    CCFree(ptr);
}

size_t CCGetCachelineSize(void);




#endif /* CFAllocator_h */
