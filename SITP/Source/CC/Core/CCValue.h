//
//  CCValue.h
//  SITP
//
//  Created by vector on 2019/12/27.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCValue_h
#define CCValue_h

#include "CCBase.h"

typedef struct __CCValueBase {
#if CCBuild64Bit
    _Atomic(uint_fast64_t) ref;
#else
    _Atomic(uint_fast32_t) ref;
#endif
} CCValueBase;

#if CCBuild64Bit
#define CCValueRefConstantRc 0xFFFFFFFFFFFFFFUL
#else
#define CCValueRefConstantRc 0xFFFFFFU
#endif


#if CCBuild64Bit
static inline _Atomic(uint_fast64_t) * _Nonnull CCValueRefGetRefPtr(CCValueRef _Nonnull ref) {
    return &(((CCValueBase *)ref)->ref);
}
#else
static inline _Atomic(uint_fast32_t) * _Nonnull CCValueRefGetRefPtr(CCValueRef _Nonnull ref) {
    return &(((CCValueBase *)ref)->ref);
}
#endif

static inline void __CCValueRefInit(CCValueBase * _Nonnull ref, uint8_t type, CCBool isConstant) {
    assert(ref);
    assert(type);

#if CCBuild64Bit
    uint_fast64_t t = type;
    t = t << 56;
    uint_fast64_t rcInfo = 0;
    if (isConstant) {
        rcInfo = t | CCValueRefConstantRc;
    } else {
        rcInfo = t | 2L;
    }
    _Atomic(uint_fast64_t) * rcInfoPtr = CCValueRefGetRefPtr(ref);
#else
    uint_fast32_t t = type;
    t = t << 24;
    uint_fast32_t rcInfo = 0;
    if (isConstant) {
        rcInfo = t | CCValueRefConstantRc;
    } else {
        rcInfo = t | 2L;
    }
    _Atomic(uint_fast32_t) * rcInfoPtr = CCValueRefGetRefPtr(ref);
#endif
    atomic_store(rcInfoPtr, rcInfo);
}

static inline void __CCValueRefRetain(CCValueRef _Nonnull ref) {
    assert(ref);

#if CCBuild64Bit
    uint_fast64_t rcInfo = 0;
    uint_fast64_t newRcInfo = 0;
    _Atomic(uint_fast64_t) * rcInfoPtr = CCValueRefGetRefPtr(ref);
#else
    uint_fast32_t rcInfo = 0;
    uint_fast32_t newRcInfo = 0;
    _Atomic(uint_fast32_t) * rcInfoPtr = CCValueRefGetRefPtr(ref);
#endif
    do {
        rcInfo = atomic_load(rcInfoPtr);
        assert((rcInfo & CCValueRefConstantRc) >= 2);
        if ((rcInfo & CCValueRefConstantRc) == CCValueRefConstantRc) {
            return;
        }
        newRcInfo = rcInfo + 1;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
}

//返回true 时 需要deallocate
static inline CCBool __CCValueRefRelease(CCValueRef _Nonnull ref) {
    assert(ref);
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
        assert((rcInfo & CCValueRefConstantRc) >= 2);
        if ((rcInfo & CCValueRefConstantRc) == CCValueRefConstantRc) {
            return false;
        }
        newRcInfo = rcInfo - 1;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    return (rcInfo & CCValueRefConstantRc) == 1;
}



#endif /* CCValue_h */
