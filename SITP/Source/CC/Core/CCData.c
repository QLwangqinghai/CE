//
//  CCData.c
//  SITP
//
//  Created by vector on 2019/12/23.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCData.h"




#define ELF_STEP(B) T1 = (H << 4) + B; T2 = T1 & 0xF0000000; if (T2) T1 ^= (T2 >> 24); T1 &= (~T2); H = T1;

CCHashCode CCHashBytes(uint8_t * _Nonnull bytes, CCIndex length) {
    /* The ELF hash algorithm, used in the ELF object file format */
    CCHashCode H = 0;
    CCHashCode T1 = 0;
    CCHashCode T2 = 0;
    CCIndex rem = length;
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
