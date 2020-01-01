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
#include "CCConfig.h"


typedef uint64_t CCUInt64;
typedef uint32_t CCUInt32;
typedef uint16_t CCUInt16;
typedef uint8_t CCUInt8;
typedef int64_t CCSInt64;
typedef int32_t CCSInt32;
typedef int16_t CCSInt16;
typedef int8_t CCSInt8;
typedef _Bool CCBool;

typedef CCUInt8 CCChar;
typedef CCUInt16 CCU16Char;



#if CCBuild64Bit
typedef CCSInt64 CCInt;
typedef CCUInt64 CCUInt;
#else
typedef CCSInt32 CCInt;
typedef CCUInt32 CCUInt;
#endif

typedef CCUInt CCHashCode;

typedef CCSInt64 CCMicrosecondTime;


typedef struct {
    CCSInt64 seconds;
    CCSInt32 nanosecond;
    CCSInt32 _xx;//must be zero
} CCTime;

typedef void * CCPtr;
typedef CCPtr CCRef;
typedef CCPtr CCValueRef;

typedef CCValueRef CCDataRef;
typedef CCValueRef CCStringRef;
typedef CCValueRef CCTypeRef;








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




/*
 [8B] length
 [8B] (direction, precision, secondSize)
 [secondSize] (second)
 [s] (length - 1 - secondSize)
 */

/*
 *
 * string data date array map bigNumber number
 */

#endif /* CEParamType_h */