//
//  CCString.c
//  CCollection
//
//  Created by vector on 2019/7/4.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCString.h"

int CUInt32LeastSignificant1111(uint8_t c) {
    if (c >= 0xC0) {
        if (c >= 0xE0) {
            if (c >= 0xF0) {
                if (c >= 0xF8) {
                    return -1;
                } else {
                    return 4;
                }
            } else {
                return 3;
            }
        } else {
            return 1;
        }
        
    } else {
        return 1;
    }
    
    
    

}

