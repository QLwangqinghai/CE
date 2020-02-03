//
//  CSMalloc.c
//  CoreSystem
//
//  Created by vector on 2018/12/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CSMalloc.h"

#if CS_TARGET_OS_DARWIN
static inline size_t _CSMallocSize(const void * _Nonnull ptr) {
    return malloc_size(ptr);
}
#elif CS_TARGET_OS_LINUX
static inline size_t _CSMallocSize(const void * _Nonnull ptr) {
    return malloc_usable_size(ptr);
}
#elif CS_TARGET_OS_WINDOWS
static inline size_t _CSMallocSize(const void * _Nonnull ptr) {
    return _msize(ptr);
}
#else
#error "不支持的平台"
#endif


void * _Nullable CSMalloc(size_t size) {
    return malloc(size);
}

void * _Nullable CSRealloc(void * _Nullable ptr, size_t size) {
    return realloc(ptr, size);
}

size_t CSMallocSize(const void * _Nonnull ptr) {
    return _CSMallocSize(ptr);
}

void CSFree(void * _Nullable ptr) {
    free(ptr);
}

