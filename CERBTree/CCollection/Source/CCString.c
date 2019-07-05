//
//  CCString.c
//  CCollection
//
//  Created by vector on 2019/7/4.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCString.h"

static int CUInt32LeastSignificant1111(uint8_t c) {
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

