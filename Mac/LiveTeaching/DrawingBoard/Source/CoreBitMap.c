//
//  CoreBitMap.c
//  ScrollView
//
//  Created by vector on 2019/8/22.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CoreBitMap.h"
#include <stdlib.h>
#include <string.h>


_Bool C2DArgbPixelsIsEqual(void const * _Nonnull a, void const * _Nonnull b, uint32_t pixelsCount) {
    uint32_t count = pixelsCount;
    
    uint64_t * left = (uint64_t *)a;
    uint64_t * right = (uint64_t *)b;
    while (count >= 2) {
        if (*left != *right) {
            return false;
        }
        left += 1;
        right += 1;
        count -= 2;
    }
    
    if (count > 0) {
        uint32_t left32 = *((uint32_t *)left);
        uint32_t right32 = *((uint32_t *)right);
        if (left32 != right32) {
            return false;
        }
    }
    return true;
}


void * ContextCreate(void) {
    size_t w = 256 * 6;
    size_t h = 256 * 4;
    size_t bytesPerPixel = 4;
    size_t page = 10;

    size_t size = w * h * bytesPerPixel * page; //117 964 800
    
//    size_t size = (1920 * 4) * (1920 * 8); //117 964 800
    void * p = malloc(size);
    memset(p, 0, size);
    printf("width:%ld, height:%ld, bytesPerPixel:%ld, page:%ld, size %ld, %p\n", w, h, bytesPerPixel, page, size, p);
    return p;
}

C2DBitMapBlock_s * addddd() {
    return NULL;
}

void C2DLittle16ArgbPixelsSet(void * _Nonnull buffer, uint16_t color, uint32_t pixelsCount) {
    uint64_t c = (((uint64_t)color) << 48) | (((uint64_t)color) << 32) | (((uint64_t)color) << 16) | (((uint64_t)color) << 0);
    uint32_t loopCount = pixelsCount / 4;
    uint8_t * ptr = buffer;
    for (uint32_t idx=0; idx<loopCount; idx++) {
        *((uint64_t *)ptr) = c;
        ptr += 8;
    }
}
void C2DLittle32ArgbPixelsSet(void * _Nonnull buffer, uint32_t color, uint32_t pixelsCount) {
    uint64_t c = (((uint64_t)color) << 32) | (((uint64_t)color) << 0);
    uint32_t loopCount = pixelsCount / 2;
    uint8_t * ptr = buffer;
    for (uint32_t idx=0; idx<loopCount; idx++) {
        *((uint64_t *)ptr) = c;
        ptr += 8;
    }
}
