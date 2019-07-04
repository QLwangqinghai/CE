//
//  CCAtomic.h
//  CCollection
//
//  Created by vector on 2019/7/4.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCAtomic_h
#define CCAtomic_h


#include <stdatomic.h>


static inline void CCAtomicUInt32Init(_Atomic(uint_fast32_t) * _Nonnull ref, uint_fast32_t v) {
    atomic_init(ref, v);
}
static inline uint_fast32_t CCAtomicUInt32Load(_Atomic(uint_fast32_t) * _Nonnull ref) {
    uint_fast32_t v = atomic_load(ref);
    return v;
}
static inline _Bool CCAtomicUInt32CompareExchange(_Atomic(uint_fast32_t) * _Nonnull ref, uint_fast32_t oldValue, uint_fast32_t newValue) {
    _Bool result = atomic_compare_exchange_strong(ref, &oldValue, newValue);
    return result;
}


static inline void CCAtomicUInt64Init(_Atomic(uint_fast64_t) * _Nonnull ref, uint_fast64_t v) {
    atomic_init(ref, v);
}
static inline uint_fast64_t CCAtomicUInt64Load(_Atomic(uint_fast64_t) * _Nonnull ref) {
    uint_fast64_t v = atomic_load(ref);
    return v;
}
static inline _Bool CCAtomicUInt64CompareExchange(_Atomic(uint_fast64_t) * _Nonnull ref, uint_fast64_t oldValue, uint_fast64_t newValue) {
    _Bool result = atomic_compare_exchange_strong(ref, &oldValue, newValue);
    return result;
}

#endif /* CCAtomic_h */
