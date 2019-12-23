//
//  SITPType.h
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef SITPType_h
#define SITPType_h

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdatomic.h>

typedef uint32_t SITPIndex;
typedef uint32_t SITPLength;
typedef uint64_t SITPByteSize;

#define SITPIndexNotFound UINT32_MAX
#define SITPByteSizeNotFound UINT64_MAX

typedef enum {
    SITPTypeCodeSInt = 0x0,
    SITPTypeCodeUInt = 0x1,
    SITPTypeCodeFloat32 = 0x2,
    SITPTypeCodeFloat64 = 0x3,
    SITPTypeCodeBool = 0x4,
    SITPTypeCodeTimestamp = 0x5,
    SITPTypeCodeHighPrecisionTime = 0x6,
    SITPTypeCodeData = 0x7,
    SITPTypeCodeString = 0x8,
    SITPTypeCodeMessage = 0x9,

    SITPTypeCodeSIntArray = 0x10,
    SITPTypeCodeUIntArray = 0x11,
    SITPTypeCodeFloat32Array = 0x12,
    SITPTypeCodeFloat64Array = 0x13,
    SITPTypeCodeBoolArray = 0x14,
    SITPTypeCodeTimestampArray = 0x15,
    SITPTypeCodeHighPrecisionTimeArray = 0x16,
    SITPTypeCodeDataArray = 0x17,
    SITPTypeCodeStringArray = 0x18,
    SITPTypeCodeMessageArray = 0x19,
} SITPTypeCode_e;

typedef enum {
    SITPDataSubtypeCodeByte16 = 0x0,
    SITPDataSubtypeCodeByte32 = 0x1,
    SITPDataSubtypeCodeByte48 = 0x2,
    SITPDataSubtypeCodeByte64 = 0x3,
    
    SITPDataSubtypeCodeNormal = 0x16,
    SITPDataSubtypeCodeJsonMap = 0x17,
} SITPDataSubtypeCode_e;

static inline SITPByteSize SITPDataSubtypeGetLength(SITPDataSubtypeCode_e code) {
    if (SITPDataSubtypeCodeByte16 == code) {
        return 16;
    } else if (SITPDataSubtypeCodeByte32 == code) {
        return 32;
    } else if (SITPDataSubtypeCodeByte48 == code) {
        return 48;
    } else if (SITPDataSubtypeCodeByte64 == code) {
        return 64;
    } else {
        return 0;
    }
}

/*
 5b(type) 3b(control) SITPTypeCodeSInt、SITPTypeCodeUInt
 5b(type) 3b(0 subtypeControl control) SITPTypeCodeFloat32、SITPTypeCodeFloat64
 5b(type) 2b(0) 1b(content) SITPTypeCodeBool
 5b(type) 3b(subtypeControl) 0b(length control) 5b(subtype) 3b(length control) SITPTypeCodeData
 5b(type) 3b(length control) SITPTypeCodeString
 5b(type) 3b(length control) SITPTypeCodeMessage

 
 5b(type) 3b(length control) SITPTypeCodeSIntArray、SITPTypeCodeUIntArray、SITPTypeCodeStringArray、SITPTypeCodeMessageArray、SITPTypeCodeFloat32Array、SITPTypeCodeFloat64Array、SITPTypeCodeBoolArray
 5b(type) 4b(subtypeControl) 0b(length control) 5b(subtype) 3b(length control) SITPTypeCodeDataArray
 11b
 3lenth 3headerLength
 
 */


typedef struct {
    SITPIndex offset;
    SITPIndex length;
} SITPIndexShift_t;

typedef struct {
    SITPIndex location;
    SITPIndex length;
} SITPIndexRange;

typedef struct {
    SITPByteSize location;
    SITPByteSize length;
} SITPByteRange;

static inline _Bool SITPByteRangeIsInvalid(SITPByteRange range) {
    return range.location == SITPByteSizeNotFound || (range.location + range.length < range.location);
}


static inline SITPByteRange SITPByteRangeMake(SITPByteSize location, SITPByteSize length) {
    SITPByteRange range = {
        .location = location,
        .length = length,
    };
    return range;
}

#pragma pack(push, 4)
typedef struct {
    int32_t sub;
    int64_t timestamp;
} SITPTime1;
#pragma pack(pop)


typedef struct {
    uint32_t _xxx;
    int32_t sub;
    int64_t timestamp;
} SITPTime;

static inline _Bool SITPTimeIsInvalid(SITPTime time) {
    if (time.timestamp == 0) {
        return time.sub < 1000000000 && time.sub > -1000000000;
    } else if (time.timestamp < 0) {
        return time.sub > -1000000000 && time.sub <= 0;
    } else {
        return time.sub < 1000000000 && time.sub >= 0;
    }
}


typedef union {
    _Bool boolValue: 1;
    int64_t sintValue;
    uint64_t uintValue;
    int64_t timeInterval;
    SITPTime time;
    SITPByteRange range;
} SITPFieldContent_u;

typedef struct {
    SITPIndex index;
    uint32_t type: 8;
    uint32_t subtype: 24;//data dataArray时有用
    SITPFieldContent_u content;
} SITPField_t;

extern SITPField_t const SITPFieldInvalid;

_Bool SITPFieldIsInvalid(SITPField_t field);


typedef struct {
    uint8_t content[16];
} SITPByte16;
typedef struct {
    uint8_t content[32];
} SITPByte32;
typedef struct {
    uint8_t content[48];
} SITPByte48;
typedef struct {
    uint8_t content[64];
} SITPByte64;

typedef struct {
    uint8_t content[64];
} SITPData_t;



typedef struct {
    uint32_t type: 8;
    uint32_t ref: 24;
} Object;
/*
 SITPDataSubtypeCodeByte16 = 0x0,
 SITPDataSubtypeCodeByte32 = 0x1,
 SITPDataSubtypeCodeByte48 = 0x2,
 SITPDataSubtypeCodeByte64 = 0x3,
 */
/*
 Message
 
 ------
 shift
 shift
 ......
 
 Field
 Field
 ......

 shift
 shift
 ......
 
 Field
 Field
 ......
 
 ------
 */



/*
 uint8_t byte;
 SITPIndex currentOffset;
 SITPIndex length = 0;
 
 if (byte >= 128) {
    uint8_t add = (byte >> 4) & 0x7;
    currentOffset += add;
    length = (byte & 0xf) + 1;
 } else {
    uint8_t add = byte & 0x7F;
    currentOffset += add;
 }
 
 */













#endif /* SITPType_h */
