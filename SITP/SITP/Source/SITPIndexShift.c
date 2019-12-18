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
    SITPIndexShiftUpdate(&shift, byte);
    return shift;
}

void SITPIndexShiftUpdate(SITPIndexShift_t * _Nonnull shift, uint8_t byte) {
    if (byte < 0x80) {
        //[0-16)
        SITPIndex add = (byte >> 4) & 0x7;
        shift->offset = add;
        shift->length = (byte & 0xf) + 1;
    } else if (byte < 0xC0) {
        //[16 - 1024]
        SITPIndex add = (byte & 0x3F) + 1;
        add = (add << 4);
        shift->offset = add;
    } else if (byte < 0xE0) {
        //[2048 - 0x10000]
        SITPIndex add = (byte & 0x1F + 1);
        add = (add << 11);
        shift->offset = add;
    } else if (byte < 0xF0) {
        //[0x20000 - 0x200000]
        SITPIndex add = (byte & 0xF + 1);
        add = (add << 17);
        shift->offset = add;
    } else if (byte < 0xF8) {
        //[0x400000 - 0x2000000]
        SITPIndex add = (byte & 0x7 + 1);
        add = (add << 22);
        shift->offset = add;
    } else {
        //[0x4000000 - 0x20000000]
        SITPIndex add = (byte & 0x7 + 1);
        add = (add << 26);
        shift->offset = add;
    }
}
