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


#endif /* CoreBitMap_h */
