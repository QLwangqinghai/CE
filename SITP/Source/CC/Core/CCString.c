//
//  CCString.c
//  CCollection
//
//  Created by vector on 2019/7/4.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCString.h"
#include "CCValue.h"

typedef struct __CCStringContent {
    CCHashCode code;
    CCU16Char content[2];
} CCStringContent;

typedef struct __CCString {
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
    CCHashCode code;
    
} CCString;


/*
 32
 8 + 4(CCHashCode) + 4    2     16
 8 + 4(CCHashCode) + 20   10    32
 8 + 4(CCHashCode) + 52   26    64
 8 + 4(CCHashCode) + 4(Ptr)     16
 
 64
 16 + 8(CCHashCode) + 8    4     32
 16 + 8(CCHashCode) + 40   20    64
 16 + 8(CCHashCode) + 8(Ptr)     32
 */

CCStringRef CCStringAlloc(CCUInt length) {
    //
    
    
    
    
    
    
    
    
    
}



static int CCUInt32LeastSignificant1111(uint8_t c) {
    if (c >= 0xE0) {
        if (c >= 0xF0) {
            if (c == 0xFF) {
                return -1;
            } else {
                return 4;
            }
        } else {
            return 3;
        }
    } else {
        if (c < 0x80) {
            return 1;
        } else {
            if (c >= 0xC0) {
                return 2;
            } else {
                return -1;
            }
        }
    }
}

