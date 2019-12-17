//
//  SITPIndexShift.c
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "SITPIndexShift.h"


SITPIndexShift_t SITPIndexShiftMake(uint8_t byte) {
    SITPIndexShift_t shift = {};
    if (byte >= 128) {
       uint8_t add = (byte >> 4) & 0x7;
       shift.offset = add;
       shift.length = (byte & 0xf) + 1;
    } else {
       uint8_t add = byte & 0x7F;
       shift.offset = add;
    }
    return shift;
}
