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




#endif /* CCBase_h */
