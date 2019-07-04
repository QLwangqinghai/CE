//
//  CCImmutableBuffer.h
//  CCollection
//
//  Created by vector on 2019/7/4.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCImmutableBuffer_h
#define CCImmutableBuffer_h

#include "CInteger.h"
#include "CType.h"
#include "CCBase.h"
#include "CCAtomic.h"

typedef struct __CCImmutableBuffer {
#if CBuild64Bit
    _Atomic(uint_fast64_t) ref;
#else
    _Atomic(uint_fast32_t) ref;
#endif
    uint32_t _capacity;
    uint32_t _elementSize;
    
    uint8_t items[];
} CCImmutableBuffer_s;

#if CBuild64Bit
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
    
#if CBuild64Bit
    _Atomic(uint_fast64_t) * refPtr = CCImmutableBufferGetRefPtr(buffer);
    CCAtomicUInt64Init(refPtr, 1);
#else
    _Atomic(uint_fast32_t) * refPtr = CCImmutableBufferGetRefPtr(buffer);
    CCAtomicUInt32Init(refPtr, 1);
#endif
    
    return buffer;
}

static inline void __CCImmutableBufferDeallocate(CCImmutableBuffer_s * _Nonnull buffer) {
    CCDeallocate(buffer);
}

//如果返回空 就是有异常
static inline CCImmutableBuffer_s * _Nonnull __CCImmutableBufferRetain(CCImmutableBuffer_s * _Nonnull buffer) {
    if (NULL == buffer) {
        return buffer;
    } else {
#if CBuild64Bit
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
            CERcType_t rc = rcInfo & CERuntimeRcMask;
            
            if (__builtin_expect((rc < CERuntimeRcStatic), true)) {
                rc += 1;
            } else if (rc == CERuntimeRcStatic) {
                return object;
            } else {
                if (!tryR) {
                    CELogError("retain a dealloced object");
                    abort();
                }
                return NULL;
            }
            CERcType_t weakBit = rcInfo & CERuntimeRcWeakBit;
            newRcInfo = weakBit | rc;
        } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    }
        
    return buffer;
}

static inline CCImmutableBuffer_s * _Nonnull __CCImmutableBufferRelease(CCImmutableBuffer_s * _Nonnull buffer) {

    return buffer;
}




#endif /* CCImmutableBuffer_h */
