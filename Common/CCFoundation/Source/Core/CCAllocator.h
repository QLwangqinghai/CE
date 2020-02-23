//
//  CCAllocator.h
//  SITP
//
//  Created by vector on 2019/12/24.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCAllocator_h
#define CCAllocator_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "CCType.h"
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>


#if BUILD_TARGET_OS_WINDOWS

#include<malloc.h>

static inline CCPtr _Nonnull CCAlignedMalloc(size_t size, size_t alignment) {
    assert(size > 0);
    CCPtr ptr = _aligned_malloc(size, alignment);
    return ptr;
}

#else

static inline CCPtr _Nonnull CCAlignedMalloc(size_t size, size_t alignment) {
    assert(size > 0);
    CCPtr result = NULL;
    posix_memalign(&result, alignment, size);
    return result;
}
#endif

static inline CCPtr _Nonnull CCMalloc(size_t size) {
    assert(size >= 0);
    if (size <= 0) {
        size = 4;
    }
    CCPtr ptr = malloc(size);
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


//默认清0
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

static inline void * _Nonnull CCAlignedAllocate(size_t size, size_t alignment) {
    if (size <= 0) {
        size = 4;
    }
    CCPtr ptr = CCAlignedMalloc(size, alignment);
    if (NULL == ptr) {
        CCLogError("CCAlignedAllocate ptr is NULL\n");
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


#if defined(__cplusplus)
}  // extern C
#endif

#endif /* CFAllocator_h */
