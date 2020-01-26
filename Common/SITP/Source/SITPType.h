//
//  SITPType.h
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef SITPType_h
#define SITPType_h

#include <CCFoundation/CCFoundation.h>

typedef uint32_t SITPIndex;
typedef uint32_t SITPSize;
typedef uint64_t SITPByteSize;


//16bit
#define SITPMessageIndexMaxCount 0xFFFFU

//24bit - 1
#define SITPMessageMaxIndex 0xFFFFFEU


#define SITPIndexNotFound 0xFFFFFFU
#define SITPByteSizeNotFound UINT64_MAX

typedef enum {
    SITPTypeCodeSInt = 0x0,
    SITPTypeCodeUInt = 0x1,
    SITPTypeCodeFloat32 = 0x2,
    SITPTypeCodeFloat64 = 0x3,
    SITPTypeCodeBool = 0x4,
    SITPTypeCodeTime = 0x5,
    SITPTypeCodeData = 0x6,
    SITPTypeCodeString = 0x7,
    SITPTypeCodeMessage = 0x8,
    SITPTypeCodeByte16 = 0x9,//using for md5、uuid、uint128_t

    SITPTypeCodeSIntArray = 0x10,
    SITPTypeCodeUIntArray = 0x11,
    SITPTypeCodeFloat32Array = 0x12,
    SITPTypeCodeFloat64Array = 0x13,
    SITPTypeCodeBoolArray = 0x14,
    SITPTypeCodeTimeArray = 0x15,
    SITPTypeCodeDataArray = 0x16,
    SITPTypeCodeStringArray = 0x17,
    SITPTypeCodeMessageArray = 0x18,
    SITPTypeCodeByte16Array = 0x19,
} SITPTypeCode_e;

typedef enum {
    SITPTimeSubtypeCodeMicrosecond = 0x0,
} SITPTimeSubtypeCode_e;


typedef enum {
    SITPStringEncodingCodeUtf8 = 0,
    SITPStringEncodingCodeUtf16LittleEndian = 0x1,
} SITPStringEncodingCode_e;

/*
 string content
 1B encodeCode
 ...content
 */
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
    SITPIndex encoding;//5bit encoding
    SITPIndex control;//3bit control
} SITPArrayHead_t;//1Byte

typedef struct {
    SITPIndex offset;
    SITPIndex length;//[0, 8]
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
    return range.location == SITPByteSizeNotFound || (range.location + range.length < range.location);//越界检查
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

typedef int64_t SITPMicrosecondTime;


//typedef struct {
//    uint32_t _xxx;
//    int32_t sub;
//    int64_t timestamp;
//} SITPTime;
//
//static inline _Bool SITPTimeIsInvalid(SITPTime time) {
//    if (time.timestamp == 0) {
//        return time.sub < 1000000000 && time.sub > -1000000000;
//    } else if (time.timestamp < 0) {
//        return time.sub > -1000000000 && time.sub <= 0;
//    } else {
//        return time.sub < 1000000000 && time.sub >= 0;
//    }
//}


typedef union {
    _Bool boolValue;
    int64_t sintValue;
    uint64_t uintValue;
    SITPMicrosecondTime time;
    SITPByteRange range;
} SITPFieldContent_u;

typedef struct {
    uint32_t index: 24;
    uint32_t type: 7;
    uint32_t boolValue: 1;//Bool时有用
    SITPSize contentLength;
} SITPField_t;

typedef struct {
    uint32_t type: 7;
    uint32_t boolValue: 1;//Bool时有用
    SITPSize contentLength;
} SITPArrayItem_t;

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
