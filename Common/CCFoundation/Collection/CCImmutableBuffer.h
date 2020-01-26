//
//  CCImmutableBuffer.h
//  CCollection
//
//  Created by vector on 2019/7/4.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCImmutableBuffer_h
#define CCImmutableBuffer_h

#include "CCBase.h"

static int32_t const CCImmutableBufferStatusActive = 1;
static int32_t const CCImmutableBufferStatusWaitDeallocate = 2;

typedef struct __CCImmutableBuffer {
#if CCBuild64Bit
    _Atomic(uint_fast64_t) ref;
#else
    _Atomic(uint_fast32_t) ref;
#endif
    uint32_t _capacity;
    uint32_t _elementSize;
    
    uint8_t items[];
} CCImmutableBuffer_s;

#if CCBuild64Bit
static inline _Atomic(uint_fast64_t) * _Nonnull CCImmutableBufferGetRefPtr(CCImmutableBuffer_s * _Nonnull buffer) {
    return &(buffer->ref);
}
#else
static inline _Atomic(uint_fast32_t) * _Nonnull CCImmutableBufferGetRefPtr(CCImmutableBuffer_s * _Nonnull buffer) {
    return &(buffer->ref);
}
#endif


static inline CCImmutableBuffer_s * _Nonnull __CCImmutableBufferAllocate(uint32_t capacity, uint32_t elementStoreSize) {
    size_t s = capacity;
    s = s * elementStoreSize;
    CCImmutableBuffer_s * buffer = CCAllocate(sizeof(CCImmutableBuffer_s) + s);
    buffer->_elementSize = elementStoreSize;
    buffer->_capacity = capacity;

#if CCBuild64Bit
    _Atomic(uint_fast64_t) * refPtr = CCImmutableBufferGetRefPtr(buffer);
    CCAtomicUInt64Init(refPtr, 2);
#else
    _Atomic(uint_fast32_t) * refPtr = CCImmutableBufferGetRefPtr(buffer);
    CCAtomicUInt32Init(refPtr, 2);
#endif
    
    return buffer;
}

static inline void __CCImmutableBufferDeallocate(CCImmutableBuffer_s * _Nonnull buffer) {
#if CCBuild64Bit
    _Atomic(uint_fast64_t) * refPtr = CCImmutableBufferGetRefPtr(buffer);
    uint_fast64_t refCount = CCAtomicUInt64Load(refPtr);
    assert(refCount == 1);
#else
    _Atomic(uint_fast32_t) * refPtr = CCImmutableBufferGetRefPtr(buffer);
    uint_fast32_t refCount = CCAtomicUInt32Load(refPtr);
    assert(refCount == 1);
#endif
    
    CCDeallocate(buffer);
}

static inline CCImmutableBuffer_s * _Nonnull __CCImmutableBufferRetain(CCImmutableBuffer_s * _Nonnull buffer) {
    if (NULL == buffer) {
        return buffer;
    } else {
#if CCBuild64Bit
        uint_fast64_t rcInfo = 0;
        uint_fast64_t newRcInfo = 0;
        _Atomic(uint_fast64_t) * rcInfoPtr = CCImmutableBufferGetRefPtr(buffer);
#else
        uint_fast32_t rcInfo = 0;
        uint_fast32_t newRcInfo = 0;
        _Atomic(uint_fast32_t) * rcInfoPtr = CCImmutableBufferGetRefPtr(buffer);
#endif
        do {
            rcInfo = atomic_load(rcInfoPtr);
            assert(rcInfo >= 2);
            newRcInfo = rcInfo + 1;
        } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    }
        
    return buffer;
}

//返回true 时 需要deallocate
static inline _Bool __CCImmutableBufferRelease(CCImmutableBuffer_s * _Nonnull buffer) {
#if CCBuild64Bit
    uint_fast64_t rcInfo = 0;
    uint_fast64_t newRcInfo = 0;
    _Atomic(uint_fast64_t) * rcInfoPtr = CCImmutableBufferGetRefPtr(buffer);
#else
    uint_fast32_t rcInfo = 0;
    uint_fast32_t newRcInfo = 0;
    _Atomic(uint_fast32_t) * rcInfoPtr = CCImmutableBufferGetRefPtr(buffer);
#endif
    do {
        rcInfo = atomic_load(rcInfoPtr);
        assert(rcInfo >= 2);
        newRcInfo = rcInfo - 1;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    
    return (newRcInfo == 1);
}

static inline void * _Nonnull __CCImmutableBufferGetItemAtIndex(CCImmutableBuffer_s * _Nonnull buffer, uint32_t idx) {
    return &(buffer->items[idx * buffer->_elementSize]);
}

//range 必须是有效的，方法没有校验range的有效性
static inline void __CCImmutableBufferGetItemsInRange(CCImmutableBuffer_s * _Nonnull buffer, CCRange range, CCVector_s * _Nonnull vectorPtr) {
    vectorPtr->base = __CCImmutableBufferGetItemAtIndex(buffer, range.location);
    vectorPtr->count = range.length;
}


static inline CCImmutableBuffer_s * _Nonnull __CCImmutableBufferCreate(uint32_t elementSize, const CCVector_s * _Nullable vec, uint32_t vecCount) {
    if (vecCount > 0) {
        assert(vec);
    }
    
    assert(elementSize <= CCElementSizeLimit);
    assert(elementSize > 0);
    
    uint64_t count = 0;
    for (int vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        count += v.count;
    }
    
    assert(count <= CCCountLimit);
    uint32_t realCapacity = (uint32_t)count;
    assert(realCapacity <= CCCountLimit);

    CCImmutableBuffer_s * result = __CCImmutableBufferAllocate(realCapacity, elementSize);
    uint8_t * dst = result->items;
    for (int vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        size_t s = elementSize * v.count;
        memcpy(dst, v.base, s);
        dst += s;
    }
    
    return result;
}

#endif /* CCImmutableBuffer_h */
