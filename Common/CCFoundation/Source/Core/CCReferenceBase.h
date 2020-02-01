//
//  CCReferenceBase.h
//  SITP
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 angfung. All rights reserved.
//

#ifndef CCReferenceBase_h
#define CCReferenceBase_h

#include "CCBase.h"

typedef CCPtr CCRef;
typedef uintptr_t CCType;


extern const CCType CCTypeClosure;


typedef struct __CCRefBase {
    CCType type;

#if CCBuild64Bit
    _Atomic(uint_fast64_t) counter;
#else
    _Atomic(uint_fast32_t) counter;
#endif
} CCRefBase;


static inline CCPtr _Nonnull CCRefGetContentPtr(CCRef _Nonnull ref) {
    assert(ref);
    CCRefBase * base = (CCRefBase *)ref;
    return base + 1;
}

#if CCBuild64Bit
static inline _Atomic(uint_fast64_t) * _Nonnull CCRefGetCounterPtr(CCRef _Nonnull ref) {
    return &(((CCRefBase *)ref)->counter);
}
#else
static inline _Atomic(uint_fast32_t) * _Nonnull CCRefGetCounterPtr(CCRef * _Nonnull ref) {
    return &(((CCRefBase *)ref)->counter);
}
#endif

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
        _Atomic(uint_fast64_t) * rcInfoPtr = CCRefGetCounterPtr(ref);
#else
        uint_fast32_t rcInfo = 0;
        uint_fast32_t newRcInfo = 0;
        _Atomic(uint_fast32_t) * rcInfoPtr = CCRefGetCounterPtr(ref);
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
    _Atomic(uint_fast64_t) * rcInfoPtr = CCRefGetCounterPtr(ref);
#else
    uint_fast32_t rcInfo = 0;
    uint_fast32_t newRcInfo = 0;
    _Atomic(uint_fast32_t) * rcInfoPtr = CCRefGetCounterPtr(ref);
#endif
    do {
        rcInfo = atomic_load(rcInfoPtr);
        assert(rcInfo >= 2);
        newRcInfo = rcInfo - 1;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    return (newRcInfo == 1);
}

//static inline CCBool CCRefIsTagged(CCRef _Nonnull ref) {
//    assert(ref);
//    uintptr_t v = (uintptr_t)ref;
//    return (v & 0x1) ? (CCUInt8)((v >> 1) & 0x7) : 255;
//}
//
//static inline CCUInt8 CCRefGetTaggedTypeCode(CCRef _Nonnull ref) {
//    uintptr_t v = (uintptr_t)ref;
//    assert(v & 0x1);
//
//    return (CCUInt8)((v >> 1) & 0x7);
//}
//
//static inline CCUInt8 CCRefGetTaggedSpecific(CCRef _Nonnull ref) {
//    uintptr_t v = (uintptr_t)ref;
//    assert(v & 0x1);
//    return (v >> 4) & 0x7;
//}



static inline void __CCRefInit(CCRefBase * _Nonnull ref, CCType type) {
    assert(ref);
    assert(type);
    ref->type = type;

#if CCBuild64Bit
    uint_fast64_t rcInfo = 2;
    _Atomic(uint_fast64_t) * rcInfoPtr = CCRefGetCounterPtr(ref);
#else
    uint_fast32_t rcInfo = 2;
    _Atomic(uint_fast32_t) * rcInfoPtr = CCRefGetCounterPtr(ref);
#endif
    atomic_store(rcInfoPtr, rcInfo);
}
static inline CCRef _Nonnull CCRefAllocate(CCType type, size_t contentSize) {
    assert(contentSize >= 0);

    size_t size = sizeof(CCRefBase) + contentSize;
    CCPtr ptr = CCMalloc(size);
    if (NULL == ptr) {
        CCLogError("CCRefAllocate ptr is NULL\n");
        abort();
    }
    CCRefBase * _Nonnull baseRef = ptr;
    __CCRefInit(baseRef, type);
    return ptr;
}


static inline void CCRefDeallocate(CCRef _Nonnull ref) {
    assert(ref);
    CCDeallocate(ref);
}


#endif /* CCReferenceBase_h */
