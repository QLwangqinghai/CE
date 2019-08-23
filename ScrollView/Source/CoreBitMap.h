//
//  CoreBitMap.h
//  ScrollView
//
//  Created by vector on 2019/8/22.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CoreBitMap_h
#define CoreBitMap_h

#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "Core2D.h"

/*
 int32_t originX;
 int32_t originY;
 int32_t width;
 int32_t height;
 */

typedef struct {
    C2DRect frame;
    uint32_t * _Nonnull storage;
} C2DBitMapBlock_s;// 64k


typedef void * C2DBitMapBlockRef;

typedef struct {
    C2DRect frame;
    int32_t numberOfBlocks;
    int32_t numberOfRows;
    C2DBitMapBlock_s const * _Nonnull * _Nonnull blocks;
} C2DBitMapWindow;

typedef struct {
    int32_t blockSize;
    int32_t blocksPerRow;
    int32_t numberOfRows;
    
} C2DBitMap;

typedef struct {
    int32_t countLimit;
    int32_t blockCount;
    int32_t blockSize;
    int32_t numberOfCaches;
    C2DBitMapBlock_s * _Nonnull * _Nonnull caches;
} C2DBitMapBlockPool;


typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} MediaTimingBitmapRect_s;

static inline _Bool C2DArgbPixelIsEqual(uint32_t a, uint32_t b) {
    if (a == b) {
        return true;
    } else {
        if (a < 0x1000000UL && b < 0x1000000UL) {
            return true;
        } else {
            return false;
        }
    }
}

static inline _Bool C2DTwoArgbPixelIsEqual(uint64_t a, uint64_t b) {
    if (a == b) {
        return true;
    } else {
        if ((0xFF000000FF000000ULL & a) == 0) {
            if ((0xFF000000FF000000ULL & b) == 0) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}

_Bool C2DArgbPixelsIsEqual(void const * _Nonnull a, void const * _Nonnull b, uint32_t pixelsCount);



#endif /* CoreBitMap_h */
