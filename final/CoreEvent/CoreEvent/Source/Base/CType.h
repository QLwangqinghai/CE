//
//  CType.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CType_h
#define CType_h

#include <sys/types.h>
#include <stdint.h>

#if defined(__GNUC__)
#include <stdbool.h>
#endif


typedef int64_t sint64_t;
typedef int32_t sint32_t;
typedef int16_t sint16_t;
typedef int8_t sint8_t;
typedef _Bool bool_t;
typedef void * CPointer;


typedef enum {
    CTypeNone = 0,
    
    CTypeBool = 1,
    CTypeSInt8 = 2,
    CTypeUInt8 = 3,
    CTypeSInt16 = 4,
    CTypeUInt16 = 5,
    CTypeSInt32 = 6,
    CTypeUInt32 = 7,
    CTypeFloat = 8,
    CTypePtr = 9,
    CTypeSInt64 = 10,
    CTypeUInt64 = 11,
    CTypeDouble = 12,
    CTypeBuffer = 13,
} CType_e;

static inline _Bool CTypeIsValid(CType_e t) {
    return (t >= CTypeBool && t <= CTypeBuffer);
}


#endif /* CEParamType_h */
