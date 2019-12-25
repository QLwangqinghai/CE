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
#include <wchar.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#include "CCInteger.h"
#include "CCType.h"
#include "CCConfig.h"
#include "CCAtomic.h"


//2kb
#define CCDefineElementSizeLimit 0x800uL

#define CCLogError(format, ...) do {\
fprintf(stderr, format, ##__VA_ARGS__);                                               \
fflush(stderr);\
} while(0)


#if CCBuild64Bit
    typedef uint64_t CCIndex;
#define CCDefineIndexNotFound UINT64_MAX
#define CCTypeMutableMask 0x80000000UL
#define CCBufferMaxSize 0x800000
#define CCPageMaxCountPerGroup 0x100000
#define CCPageMaxCountPerGroupShift 20
#define CCPageIndexInGroupMask 0xFFFFF
#else
    typedef uint32_t CCIndex;
#define CCDefineIndexNotFound UINT32_MAX
#define CCTypeMutableMask 0x8000
#define CCBufferMaxSize 0x20000
#define CCPageMaxCountPerGroup UINT32_MAX
#endif

#if CCBuild64Bit

//256MB
#define CCPageSize 0x10000000
#define CCPageSizeShift 28

#else

//8MB
#define CCPageSize 0x800000
#define CCPageSizeShift 23

#endif



typedef uint8_t * CCPagePtr;


extern const CCIndex CCIndexNotFound;

//count 的最大值
extern const uint32_t CCCountLimit;
extern const uint32_t CCElementSizeLimit;//2KB
extern const uint32_t CCCircularBufferElementsSizeLimit;

//extern const CCType CCTypeArray;
//extern const CCType CCTypeMutableArray;
//
//
//
//#pragma pack(push)
//#pragma pack(2)
//
//typedef struct {
//    CCType type;
//    CCType subtype;
//} CCTypeStorage;
//
//#pragma pack(pop)

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
        .base = (void *)base,
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
    CCTypeRef _Nonnull type;

#if CCBuild64Bit
    _Atomic(uint_fast64_t) ref;
#else
    _Atomic(uint_fast32_t) ref;
#endif
} CCRefBase_s;


#if CCBuild64Bit
static inline _Atomic(uint_fast64_t) * _Nonnull CCGetRefPtr(void * _Nonnull ref) {
    return &(((CCRefBase_s *)ref)->ref);
}
#else
static inline _Atomic(uint_fast32_t) * _Nonnull CCGetRefPtr(void * _Nonnull ref) {
    return &(((CCRefBase_s *)ref)->ref);
}
#endif

static inline void __CCRefInit(CCRefBase_s * _Nonnull ref, CCTypeRef _Nonnull type) {
    assert(ref);
    assert(type);
    ref->type = type;

#if CCBuild64Bit
    uint_fast64_t rcInfo = 2;
    _Atomic(uint_fast64_t) * rcInfoPtr = CCGetRefPtr(ref);
#else
    uint_fast32_t rcInfo = 2;
    _Atomic(uint_fast32_t) * rcInfoPtr = CCGetRefPtr(ref);
#endif
    atomic_store(rcInfoPtr, rcInfo);
}

static inline _Bool __CCRefRetain(CCRef _Nonnull ref) {
    if (NULL == ref) {
        CCLogError("__CCRefRetain NULL");
        return false;
    } else {
        uintptr_t v = (uintptr_t)ref;
        if (v & 0x1) {
            return true;
        }
                
#if CCBuild64Bit
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
static inline _Bool __CCRefRelease(CCRef _Nonnull ref) {
    if (NULL == ref) {
        CCLogError("__CCRefRelease NULL");
        return false;
    }
    uintptr_t v = (uintptr_t)ref;
    if (v & 0x1) {
        return false;
    }
    
#if CCBuild64Bit
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

static inline CCIndex CCPowerAlign2(CCIndex capacity) {
    CCIndex initialCapacity = capacity >> 1;
    initialCapacity |= (initialCapacity >> 1);
    initialCapacity |= (initialCapacity >> 2);
    initialCapacity |= (initialCapacity >> 4);
    initialCapacity |= (initialCapacity >> 8);
    initialCapacity |= (initialCapacity >> 16);
    
#if CCBuild64Bit
    initialCapacity |= (initialCapacity >> 32);
#endif
    initialCapacity += 1;
    return initialCapacity;
}


static inline CCSInt16 CCRefGetTaggedTypeCode(CCRef _Nonnull ref) {
    CCSInt16 result = -1;
    uintptr_t v = (uintptr_t)ref;
    if (0 == (v & 0x1)) {
        return result;
    }
    uint16_t tmp = (v >> 1) & 0xF;
    result = tmp;
    return result;
}

static inline CCUInt8 CCRefGetTaggedSpecific(CCRef _Nonnull ref) {
    uintptr_t v = (uintptr_t)ref;
    assert(v & 0x1);
    return (v >> 5) & 0x7;
}


#endif /* CCBase_h */
