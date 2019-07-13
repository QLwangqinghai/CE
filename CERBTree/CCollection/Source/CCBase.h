//
//  CCBase.h
//  CCollection
//
//  Created by vector on 2019/6/26.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCBase_h
#define CCBase_h



#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <errno.h>
#include "CInteger.h"
#include "CType.h"
#include "CConfig.h"

static const uint32_t CCIndexNotFound = UINT32_MAX;

#define CCLogError(format, ...) do {\
fprintf(stderr, format, ##__VA_ARGS__);                                               \
fflush(stderr);\
} while(0)

//count 的最大值
static const uint32_t CCCountLimit = 0x20000000uL;
static const uint32_t CCElementSizeLimit = 0x800uL;//2KB
static const uint32_t CCCircularBufferElementsSizeLimit = 0x80000000uL;

typedef struct __CCRange {
    uint32_t location;
    uint32_t length;
} CCRange_s;

typedef struct __CCVector {
    void * _Nullable base;
    size_t count;//item count， not memory lenth
} CCVector_s;
static inline CCVector_s CCVectorMake(void * _Nullable base, size_t count) {
    CCVector_s vec = {
        .base = base,
        .count = count,
    };
    return vec;
}

static inline CCRange_s CCRangeMake(uint32_t location, uint32_t length) {
    CCRange_s range = {
        .location = location,
        .length = length,
    };
    return range;
}

typedef void (*CCRetainCallBack_f)(const void * _Nonnull value, uint32_t elementSize);
typedef void (*CCReleaseCallBack_f)(const void * _Nonnull value, uint32_t elementSize);
typedef _Bool (*CCEqualCallBack_f)(const void * _Nonnull value1, const void * _Nonnull value2, uint32_t elementSize);


typedef struct {
    CCRetainCallBack_f _Nullable retain;
    CCReleaseCallBack_f _Nullable release;
    CCEqualCallBack_f _Nonnull equal;
} CCBaseCallBacks;


static inline void * _Nonnull CCAllocate(size_t size) {
    if (size <= 0) {
        size = 4;
    }
    void * ptr = malloc(size);
    if (NULL == ptr) {
        CCLogError("CCAllocate ptr is NULL\n");
        abort();
    }
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

static inline void CCDeallocate(void * _Nonnull ptr) {
    if (NULL == ptr) {
        CCLogError("CCDeallocate ptr is NULL\n");
        abort();
    }
    free(ptr);
}




#endif /* CCBase_h */
