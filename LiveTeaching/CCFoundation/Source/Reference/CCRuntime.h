//
//  CCRuntime.h
//  SITP
//
//  Created by vector on 2019/12/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCRuntime_h
#define CCRuntime_h

#include "CCReferenceBase.h"

typedef void(*CCRefFunc_f)(CCTypeRef _Nonnull ref);


typedef void(*CCRefOperate_f)(CCTypeRef _Nonnull ref);
typedef CCBool(*CCRefIsEqualFunc_f)(CCRef _Nonnull ref1, CCRef _Nonnull ref2);
typedef CCHashCode(*CCRefHashFunc_f)(CCTypeRef _Nonnull ref);


// - if the callback is passed 1 in 'op' it should increment the 'cf's reference count and return 0
// - if the callback is passed 0 in 'op' it should return the 'cf's reference count, up to 32 bits
// - if the callback is passed -1 in 'op' it should decrement the 'cf's reference count; if it is now zero, 'cf' should be cleaned up and deallocated; then return 0
typedef CCInt(*CCRefCountFunc_f)(CCTypeRef _Nonnull ref, CCInt op);


#endif /* CCRuntime_h */
