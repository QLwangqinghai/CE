//
//  CEMemory.c
//  CoreEvent
//
//  Created by vector on 2018/11/23.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEMemory.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct _CEMemoryManager {
    void * _Nullable (* _Nonnull allocate)(size_t size);
    void * _Nullable (* _Nonnull allocateClear)(size_t size);
    void * _Nullable (* _Nonnull reallocate)(void * _Nonnull ptr, size_t size);
    void (* _Nonnull deallocate)(void * _Nonnull ptr);
} CEMemoryManager_s;

void * mallocClear(size_t size) {
    void * ptr = malloc(size);
    
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}


static CEMemoryManager_s CEMemoryManager = {
    .allocate = malloc,
    .allocateClear = mallocClear,
    .reallocate = realloc,
    .deallocate = free,
};



void * _Nonnull CEAllocate(size_t size) {
    if (size <= 0) {
        size = 4;
    }
    void * result = CEMemoryManager.allocate(size);
    if (NULL == result) {
        fprintf(stderr, "CEAllocate faild\n");
        fflush(stderr);
        abort();
    }
    return result;
}
void * _Nonnull CEAllocateClear(size_t size) {
    if (size <= 0) {
        size = 4;
    }
    void * result = CEMemoryManager.allocateClear(size);
    if (NULL == result) {
        fprintf(stderr, "CEAllocateClear faild\n");
        fflush(stderr);
        abort();
    }
    return result;
}
void * _Nonnull CEReallocate(void * _Nonnull ptr, size_t size) {
    if (NULL == ptr) {
        fprintf(stderr, "CEReallocate ptr is NULL\n");
        fflush(stderr);
        abort();
    }
    if (size <= 0) {
        size = 4;
    }
    void * result = CEMemoryManager.reallocate(ptr, size);
    if (NULL == result) {
        fprintf(stderr, "CEReallocate faild\n");
        fflush(stderr);
        abort();
    }
    return result;
}

void CEDeallocate(void * _Nonnull ptr) {
    if (NULL == ptr) {
        fprintf(stderr, "CEDeallocate ptr is NULL\n");
        fflush(stderr);
        abort();
    }
    CEMemoryManager.deallocate(ptr);
}

void CEMemoryManagerUpdateAllocate(void * _Nullable (* _Nonnull func)(size_t size)) {
    if (NULL == func) {
        fprintf(stderr, "CEMemoryManagerUpdateAllocate func is NULL\n");
        fflush(stderr);
        abort();
    }
    CEMemoryManager.allocate = func;
}
void CEMemoryManagerUpdateAllocateClear(void * _Nullable (* _Nonnull func)(size_t size)) {
    if (NULL == func) {
        fprintf(stderr, "CEMemoryManagerUpdateAllocateClear func is NULL\n");
        fflush(stderr);
        abort();
    }
    CEMemoryManager.allocateClear = func;
}
void CEMemoryManagerUpdateReallocate(void * _Nullable (* _Nonnull func)(void * _Nonnull ptr, size_t size)) {
    if (NULL == func) {
        fprintf(stderr, "CEMemoryManagerUpdateReallocate func is NULL\n");
        fflush(stderr);
        abort();
    }
    CEMemoryManager.reallocate = func;
}
void CEMemoryManagerUpdateDeallocate(void (* _Nonnull func)(void * _Nonnull ptr)) {
    if (NULL == func) {
        fprintf(stderr, "CEMemoryManagerUpdateDeallocate func is NULL\n");
        fflush(stderr);
        abort();
    }
    CEMemoryManager.deallocate = func;
}
