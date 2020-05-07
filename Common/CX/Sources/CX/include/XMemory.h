//
//  XMemory.h
//  
//
//  Created by vector on 2020/4/4.
//

#ifndef XMemory_h
#define XMemory_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"
#include <stdlib.h>


#if BUILD_TARGET_OS_WINDOWS
#include <malloc.h>
static inline XPtr _Nonnull XAlignedAllocate(XSize size, XSize alignment) {
    assert(size > 0);
    XPtr ptr = _aligned_malloc(size, alignment);
    return ptr;
}
#else
static inline XPtr _Nonnull XAlignedAllocate(XSize size, XSize alignment) {
    assert(size > 0);
    XPtr result = NULL;
    posix_memalign(&result, alignment, size);
    return result;
}
#endif

static inline XPtr _Nullable XTryAllocate(size_t size) {
    if (size < 0) {
        return NULL;
    }
    XPtr ptr = malloc(size);
    return ptr;
}

static inline XPtr _Nonnull XAllocate(XSize size) {
    XPtr ptr = XTryAllocate(size);
    assert(ptr);
    return ptr;
}
static inline void XDeallocate(XPtr _Nonnull ptr) {
    free(ptr);
}

extern XComparisonResult XMemoryCompare(const XPtr _Nonnull lhs, const XPtr _Nonnull rhs, XSize size);

#if defined(__cplusplus)
}  // extern C
#endif


#endif /* XMemory_h */
