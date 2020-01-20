//
//  CCReferenceBase.h
//  SITP
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 angfung. All rights reserved.
//

#ifndef CCReferenceBase_h
#define CCReferenceBase_h

#include "CCFoundation.h"

typedef CCPtr CCRef;
typedef CCPtr CCValueRef;

typedef CCValueRef CCDataRef;
typedef CCValueRef CCStringRef;
typedef CCValueRef CCTypeRef;


typedef struct __CCRefBase {
    CCTypeRef _Nonnull type;

#if CCBuild64Bit
    _Atomic(uint_fast64_t) ref;
#else
    _Atomic(uint_fast32_t) ref;
#endif
} CCRefBase;


#if CCBuild64Bit
static inline _Atomic(uint_fast64_t) * _Nonnull CCGetRefPtr(void * _Nonnull ref) {
    return &(((CCRefBase *)ref)->ref);
}
#else
static inline _Atomic(uint_fast32_t) * _Nonnull CCGetRefPtr(void * _Nonnull ref) {
    return &(((CCRefBase *)ref)->ref);
}
#endif

static inline void __CCRefInit(CCRefBase * _Nonnull ref, CCTypeRef _Nonnull type) {
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


#endif /* CCReferenceBase_h */
