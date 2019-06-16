//
//  CECType.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CECType_h
#define CECType_h

#include <CoreEvent/CEConfig.h>
#include <sys/types.h>
#include <stdint.h>

#if defined(__GNUC__)
#include <stdbool.h>
#endif


#define CEParamBufferItemSizeMax 243

typedef int64_t sint64_t;
typedef int32_t sint32_t;
typedef int16_t sint16_t;
typedef int8_t sint8_t;
typedef _Bool bool_t;


typedef enum {
    CECTypeNone = 0,
    
    CECTypeBool = 1,
    CECTypeSInt8 = 2,
    CECTypeUInt8 = 3,
    CECTypeSInt16 = 4,
    CECTypeUInt16 = 5,
    CECTypeSInt32 = 6,
    CECTypeUInt32 = 7,
    CECTypeFloat = 8,
    CECTypePtr = 9,
    CECTypeSInt64 = 10,
    CECTypeUInt64 = 11,
    CECTypeDouble = 12,
    CECTypeBuffer = 13,
} CECType_e;

static inline _Bool CECTypeIsValid(CECType_e t) {
    return (t >= CECTypeBool && t <= CECTypeBuffer);
}


#endif /* CEParamType_h */
