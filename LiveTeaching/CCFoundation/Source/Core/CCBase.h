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

#pragma mark - mach

#if !defined(MIN)
    #define MIN(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#endif

#if !defined(MAX)
    #define MAX(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif


#pragma mark - log

#define CCLogError(format, ...) do {\
fprintf(stderr, format, ##__VA_ARGS__);                                               \
fflush(stderr);\
} while(0)

#pragma mark - CCIndex

//2的n幂次方对齐
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


#pragma mark - CCByte

void CCMemoryCopy(void * _Nonnull dst, const void * _Nonnull src, size_t size);

extern CCInt CCByteBase64EncodeBytes(const CCUInt8 * _Nonnull bytes, CCInt length, CCChar * _Nonnull outputBuffer, CCInt bufferLength);
extern CCInt CCByteBase64DecodeBytes(const CCChar * _Nonnull encoded, CCInt length, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);

extern const CCChar __CCBase64ByteToCharMappings[64];

#pragma mark - CCMicrosecondTime

extern CCMicrosecondTime CCMicrosecondTimeGetCurrent(void);


#pragma mark - Atomic

#include <stdatomic.h>
static inline void CCAtomicUInt32Init(_Atomic(uint_fast32_t) * _Nonnull ref, uint_fast32_t v) {
    atomic_init(ref, v);
}
static inline uint_fast32_t CCAtomicUInt32Load(_Atomic(uint_fast32_t) * _Nonnull ref) {
    uint_fast32_t v = atomic_load(ref);
    return v;
}
static inline CCBool CCAtomicUInt32CompareExchange(_Atomic(uint_fast32_t) * _Nonnull ref, uint_fast32_t oldValue, uint_fast32_t newValue) {
    CCBool result = atomic_compare_exchange_strong(ref, &oldValue, newValue);
    return result;
}

static inline void CCAtomicUInt64Init(_Atomic(uint_fast64_t) * _Nonnull ref, uint_fast64_t v) {
    atomic_init(ref, v);
}
static inline uint_fast64_t CCAtomicUInt64Load(_Atomic(uint_fast64_t) * _Nonnull ref) {
    uint_fast64_t v = atomic_load(ref);
    return v;
}
static inline CCBool CCAtomicUInt64CompareExchange(_Atomic(uint_fast64_t) * _Nonnull ref, uint_fast64_t oldValue, uint_fast64_t newValue) {
    CCBool result = atomic_compare_exchange_strong(ref, &oldValue, newValue);
    return result;
}



#endif /* CCBase_h */