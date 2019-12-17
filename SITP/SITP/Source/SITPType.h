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
typedef uint32_t SITPByteLength;

#define SITPIndexNotFound UINT32_MAX

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

typedef struct {
    SITPIndex offset;
    SITPIndex length;
} SITPIndexShift_t;

typedef struct {
//    SITPIndex index;
    SITPTypeCode_e typeCode;
    
    SITPByteLength byteLength;
} SITPFieldHeader_t;

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
