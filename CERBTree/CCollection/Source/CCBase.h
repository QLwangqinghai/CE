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


//2kb
#define CCDefineElementSizeLimit 0x800uL

#define CCLogError(format, ...) do {\
fprintf(stderr, format, ##__VA_ARGS__);                                               \
fflush(stderr);\
} while(0)


#if CBuild64Bit
    typedef uint64_t CCIndex;
    typedef uint32_t CCType;
#define CCDefineIndexNotFound UINT64_MAX
#define CCTypeMutableMask 0x80000000UL
#define CCBufferMaxSize 0x200000
#define CCPageMaxCountPerGroup 0x40000
#define CCPageMaxCountPerGroupShift 18
#define CCPageIndexInGroupMask 0x3FFFF

#define CCPageMaxCount 0x40000
#else
    typedef uint32_t CCIndex;
    typedef uint16_t CCType;
#define CCDefineIndexNotFound UINT32_MAX
#define CCTypeMutableMask 0x8000
#define CCBufferMaxSize 0x10000

#define CCPageMaxCountPerGroup UINT32_MAX
#define CCPageMaxCount UINT32_MAX
#endif

extern const CCIndex CCIndexNotFound;

//count 的最大值
extern const uint32_t CCCountLimit;
extern const uint32_t CCElementSizeLimit;//2KB
extern const uint32_t CCCircularBufferElementsSizeLimit;

extern const CCType CCTypeArray;
extern const CCType CCTypeMutableArray;



#pragma pack(push)
#pragma pack(2)

typedef struct {
    CCType type;
    CCType subtype;
} CCTypeStorage;

#pragma pack(pop)

typedef struct {
    CCIndex location;
    CCIndex length;
} CCRange;

typedef struct __CCBuffer {
    size_t count;//item count， not memory lenth
    void * _Nullable storage;

} CCBuffer_s;

typedef struct __CCPage {
    size_t count;//item count， not memory lenth
    CCBuffer_s buffers[0];
} CCPage_s;

typedef struct __CCVector {
    void * _Nullable base;
    size_t count;//item count， not memory lenth
} CCVector_s;

static inline CCVector_s CCVectorMake(const void * _Nullable base, size_t count) {
    CCVector_s vec = {
        .base = base,
        .count = count,
    };
    return vec;
}

static inline CCRange CCRangeMake(CCIndex location, CCIndex length) {
    CCRange range = {
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
    CCEqualCallBack_f _Nullable equal;
} CCBaseCallBacks;


static inline void * _Nullable CCTryAllocate(size_t size) {
    if (size <= 0) {
        return NULL;
    }
    void * ptr = malloc(size);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}


static inline void * _Nonnull CCAllocate(size_t size) {
    if (size <= 0) {
        size = 4;
    }
    void * ptr = malloc(size);
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
    free(ptr);
}



typedef struct __CCRefBase {
#if CBuild64Bit
    CCTypeStorage type;
    _Atomic(uint_fast64_t) ref;
#else
    CCTypeStorage type;
    _Atomic(uint_fast32_t) ref;
#endif
} CCRefBase_s;


#if CBuild64Bit
static inline _Atomic(uint_fast64_t) * _Nonnull CCGetRefPtr(void * _Nonnull ref) {
    return &(((CCRefBase_s *)ref)->ref);
}
#else
static inline _Atomic(uint_fast32_t) * _Nonnull CCGetRefPtr(void * _Nonnull ref) {
    return &(((CCRefBase_s *)ref)->ref);
}
#endif

static inline void __CCRefInit(CCRefBase_s * _Nonnull ref, CCType type, CCType subtype) {
    assert(ref);
#if CBuild64Bit
    uint_fast64_t rcInfo = 2;
    _Atomic(uint_fast64_t) * rcInfoPtr = CCGetRefPtr(ref);
#else
    uint_fast32_t rcInfo = 2;
    _Atomic(uint_fast32_t) * rcInfoPtr = CCGetRefPtr(ref);
#endif
    atomic_store(rcInfoPtr, rcInfo);
    CCTypeStorage t = {.type = type, .subtype = subtype};
    ref->type = t;
}

static inline _Bool __CCRefRetain(void * _Nonnull ref) {
    if (NULL == ref) {
        return false;
    } else {
#if CBuild64Bit
        uint_fast64_t rcInfo = 0;
        uint_fast64_t newRcInfo = 0;
        _Atomic(uint_fast64_t) * rcInfoPtr = CCGetRefPtr(ref);
#else
        uint_fast32_t rcInfo = 0;
        uint_fast32_t newRcInfo = 0;
        _Atomic(uint_fast32_t) * rcInfoPtr = CCGetRefPtr(ref);
#endif
        do {
            rcInfo = atomic_load(rcInfoPtr);
            assert(rcInfo >= 2);
            newRcInfo = rcInfo + 1;
        } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    }
        
    return true;
}

//返回true 时 需要deallocate
static inline _Bool __CCRefRelease(void * _Nonnull ref) {
#if CBuild64Bit
    uint_fast64_t rcInfo = 0;
    uint_fast64_t newRcInfo = 0;
    _Atomic(uint_fast64_t) * rcInfoPtr = CCGetRefPtr(ref);
#else
    uint_fast32_t rcInfo = 0;
    uint_fast32_t newRcInfo = 0;
    _Atomic(uint_fast32_t) * rcInfoPtr = CCGetRefPtr(ref);
#endif
    do {
        rcInfo = atomic_load(rcInfoPtr);
        assert(rcInfo >= 2);
        newRcInfo = rcInfo - 1;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    return (newRcInfo == 1);
}



#endif /* CCBase_h */
