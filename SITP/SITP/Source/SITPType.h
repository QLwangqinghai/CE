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
    SITPTypeCodeData = 0x5,
    SITPTypeCodeString = 0x6,
    SITPTypeCodeMessage = 0x7,

    SITPTypeCodeSIntArray = 0x8,
    SITPTypeCodeUIntArray = 0x9,
    SITPTypeCodeFloat32Array = 0xa,
    SITPTypeCodeFloat64Array = 0xb,
    SITPTypeCodeBoolArray = 0xc,
    SITPTypeCodeDataArray = 0xd,
    SITPTypeCodeStringArray = 0xe,
    SITPTypeCodeMessageArray = 0xf,
} SITPTypeCode_e;

typedef enum {
    SITPDataSubtypeCodeNormal = 0x0,
    SITPDataSubtypeCodeJsonMap = 0x1,
    SITPDataSubtypeCodeByte16 = 0x2,
    SITPDataSubtypeCodeByte32 = 0x3,
    SITPDataSubtypeCodeByte48 = 0x4,
    SITPDataSubtypeCodeByte64 = 0x5,
} SITPDataSubtypeCode_e;

typedef enum {
    SITPStringSubtypeCodeUtf8String = 0x0,
} SITPStringSubtypeCode_e;


/*
 4b(type) 0b(0 subtypeControl) 4b(control) SITPTypeCodeSInt、SITPTypeCodeUInt
 4b(type) 4b(0 subtypeControl control) SITPTypeCodeFloat32、SITPTypeCodeFloat64
 4b(type) 4b(content) SITPTypeCodeBool
 4b(type) 2b(subtypeControl) 2b(length control) SITPTypeCodeData
 4b(type) 2b(subtypeControl) 2b(length control) SITPTypeCodeString
 4b(type) 2b(0) 2b(length control) SITPTypeCodeMessage

 4b(type) 2b(0) 2b(length control) SITPTypeCodeSIntArray、SITPTypeCodeUIntArray、SITPTypeCodeFloat32Array、SITPTypeCodeFloat64Array、SITPTypeCodeMessageArray
 4b(type) 2b(subtypeControl) 2b(length control) SITPTypeCodeDataArray
 4b(type) 2b(subtypeControl) 2b(length control) SITPTypeCodeStringArray

 
 4b(type) 2b(control) 2b(length control) SITPTypeCodeBoolArray

 
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

typedef struct {
    SITPIndex index;
    uint32_t type: 8;
    uint32_t subtype: 8;//data、 string 时有用
    uint32_t contentControl: 16;
    SITPByteRange contentRange;
} SITPField_t;

extern SITPField_t const SITPFieldInvalid;

_Bool SITPFieldIsInvalid(SITPField_t field);


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
