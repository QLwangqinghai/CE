//
//  XAtomic.h
//  X
//
//  Created by vector on 2020/4/28.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XAtomic_h
#define XAtomic_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"
#include <stdatomic.h>

#define XFastSInt32 int_fast32_t
#define XFastUInt32 uint_fast32_t

#define XFastSInt64 int_fast64_t
#define XFastUInt64 uint_fast64_t

#if BUILD_TARGET_RT_64_BIT
    #define XFastSInt int_fast64_t
    #define XFastUInt uint_fast64_t

    #define XFastSIntMin INT_FAST64_MIN
    #define XFastSIntMax INT_FAST64_MAX

    #define XFastUIntMax UINT_FAST64_MAX

#elif BUILD_TARGET_RT_32_BIT
    #define XFastSInt int_fast32_t
    #define XFastUInt uint_fast32_t

    #define XFastSIntMin INT_FAST32_MIN
    #define XFastSIntMax INT_FAST32_MAX

    #define XFastUIntMax UINT_FAST32_MAX

#else
    #error unknown rt
#endif


#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XAtomic_h */
