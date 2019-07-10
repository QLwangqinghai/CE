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
#include <string.h>
#include <errno.h>
#include "CInteger.h"
#include "CType.h"
#include "CConfig.h"

typedef struct __CCRange {
    uint32_t location;
    uint32_t length;
} CCRange_s;

typedef struct __CCVector {
    void * _Nullable base;
    size_t itemCount;
} CCVector_s;


static inline CCRange_s CCRangeMake(uint32_t location, uint32_t length) {
    CCRange_s range = {
        .location = location,
        .length = length,
    };
    return range;
}

typedef void (*CCRetainCallBack_f)(const void * _Nonnull value, uint32_t valueSize);
typedef void (*CCReleaseCallBack_f)(const void * _Nonnull value, uint32_t valueSize);
typedef _Bool (*CCEqualCallBack_f)(const void * _Nonnull value1, uint32_t value1Size, const void * _Nonnull value2, uint32_t value2Size);


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
        fprintf(stderr, "CCAllocate ptr is NULL\n");
        fflush(stderr);
        abort();
    }
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

static inline void CCDeallocate(void * _Nonnull ptr) {
    if (NULL == ptr) {
        fprintf(stderr, "CCDeallocate ptr is NULL\n");
        fflush(stderr);
        abort();
    }
    free(ptr);
}

#endif /* CCBase_h */
