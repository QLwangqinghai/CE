//
//  CCData.c
//  SITP
//
//  Created by vector on 2019/12/23.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCData.h"
#include "CCValue.h"

typedef struct __CCDataContent {
    CCUInt32 code;
    CCUInt8 content[0];
} CCDataContent;

typedef struct __CCData {
    CCValueBase base;
#if CCBuild64Bit
    CCUInt hashed: 1;
    CCUInt freePtr: 1;
    CCUInt length: 62;
#else
    CCUInt hashed: 1;
    CCUInt freePtr: 1;
    CCUInt length: 30;
#endif
    CCUInt32 code;
} CCData;

/*
32
8 + 4(CCHashCode) + 4       16
8 + 4(CCHashCode) + 20      32
8 + 4(CCHashCode) + 52      64
8 + 4(CCHashCode) + 4(Ptr)  16

64
16 + 4(CCHashCode) + 12      32
16 + 4(CCHashCode) + 44      64
16 + 4(CCHashCode) + 8(Ptr)  32

*/


#define ELF_STEP(B) T1 = (H << 4) + B; T2 = T1 & 0xF0000000; if (T2) T1 ^= (T2 >> 24); T1 &= (~T2); H = T1;

CCHashCode CCHashBytes(uint8_t * _Nonnull bytes, CCIndex length) {
    /* The ELF hash algorithm, used in the ELF object file format */
    CCUInt32 H = 0;
    CCUInt32 T1 = 0;
    CCUInt32 T2 = 0;
    CCIndex rem = (CCUInt32)length;
    while (3 < rem) {
        ELF_STEP(bytes[length - rem]);
        ELF_STEP(bytes[length - rem + 1]);
        ELF_STEP(bytes[length - rem + 2]);
        ELF_STEP(bytes[length - rem + 3]);
        rem -= 4;
    }
    switch (rem) {
    case 3:  ELF_STEP(bytes[length - 3]);
    case 2:  ELF_STEP(bytes[length - 2]);
    case 1:  ELF_STEP(bytes[length - 1]);
    case 0:  ;
    }
    return H;
}

//CCIndex CCDataGetLength(CCDataRef _Nonnull theData) {
//    
//    
//}
//const CCUInt8 * _Nullable CFDataGetBytePtr(CCDataRef _Nonnull theData) {
//    
//}
//void CCDataGetBytes(CCRef _Nonnull CCDataRef, CCRange range, void * _Nonnull buffer) {
//    
//    
//}
