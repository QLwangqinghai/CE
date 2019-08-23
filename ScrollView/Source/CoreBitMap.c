//
//  CoreBitMap.c
//  ScrollView
//
//  Created by vector on 2019/8/22.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CoreBitMap.h"


_Bool C2DArgbPixelsIsEqual(void const * _Nonnull a, void const * _Nonnull b, uint32_t pixelsCount) {
    uint32_t count = pixelsCount;
    
    uint64_t * left = (uint64_t *)a;
    uint64_t * right = (uint64_t *)b;
    while (count >= 2) {
        if (!(C2DTwoArgbPixelIsEqual(*left, *right))) {
            return false;
        }
        left += 1;
        right += 1;
        count -= 2;
    }
    
    if (count > 0) {
        uint32_t left32 = *((uint32_t *)left);
        uint32_t right32 = *((uint32_t *)right);
        if (!(C2DArgbPixelIsEqual(left32, right32))) {
            return false;
        }
    }
    return true;
}

