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
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include <assert.h>

#include "Core2D.h"



// 2 3 3 5 7 
/*
 int32_t originX;
 int32_t originY;
 int32_t width;
 int32_t height;
 */

typedef int64_t CCMicrosecondTime;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t from;
    uint32_t to;
} C2DBitMapChange_s;

//64
#define C2DBitMapChunkSize 0x10
#define C2DBitMapChunkShift 4
#define C2DBitMapChunkMask 0xF


#define C2DBitMapChangePageSize 0x2000
#define C2DBitMapChangePageSizeShift 13

//64KB
typedef struct {
    C2DBitMapChange_s changes[C2DBitMapChangePageSize];
} C2DBitMapChangePage_s;

//10 * C2DBitMapChunkChangePageSize = 2n

static inline C2DBitMapChangePage_s * _Nonnull C2DBitMapChangePageCreate(void) {
    C2DBitMapChangePage_s * result = malloc(sizeof(C2DBitMapChangePage_s));
    assert(result);
    memset(result, 0, sizeof(C2DBitMapChangePage_s));
    return result;
}
static inline void C2DBitMapChangePageDestroy(C2DBitMapChangePage_s * _Nonnull ptr) {
    assert(ptr);
    free(ptr);
}

typedef struct {
    uint32_t count;
    C2DBitMapChangePage_s * _Nonnull pages[8];
} C2DBitMapChangeArray_s;

typedef struct {
    int32_t x;
    int32_t y;
    uint32_t sequence;
    uint32_t count;
    C2DBitMapChangePage_s * _Nonnull pages[8];
} C2DBitMapChunkChange_s;

static inline C2DBitMapChangePage_s * _Nonnull C2DBitMapChunkChangeGetPage(C2DBitMapChunkChange_s * _Nonnull ptr, uint32_t pageIndex) {
    assert(ptr);
    assert(pageIndex < 8);
    C2DBitMapChangePage_s * result = ptr->pages[pageIndex];
    if (NULL == result) {
        result = C2DBitMapChangePageCreate();
        ptr->pages[pageIndex] = result;
    }
    return result;
}

#define C2DBitMapChunkRowSize 0x80
#define C2DBitMapChunkRowSizeShift 7
#define C2DBitMapChunkRowSizeMask 0x7F

#define C2DBitMapChunkByteSize 0x10000
#define C2DBitMapChunkRowByteSize 0x200


typedef struct {
    C2DRect frame;
    int32_t countPerRow;
    int32_t numberOfRows;
    uint32_t * _Nonnull content;
} C2DBitMapPage_s;


typedef struct {
    C2DRect frame;
    uint32_t * _Nonnull content;
} C2DBitMapChunk_s;

//static inline uint32_t C2DBitMapContentChanges(C2DBitMapChunk_s * _Nonnull oldValue, C2DBitMapChunk_s * _Nonnull newValue, int32_t y, C2DBitMapChunkChange_s * _Nonnull change) {
//    assert(oldValue);
//    assert(newValue);
//    assert(change);
//    assert(change->count == 0);
//    assert(change->x == newValue->x);
//    assert(change->y == newValue->y);
//
//    assert(oldValue != newValue);
//    assert(oldValue->x == newValue->x);
//    assert(oldValue->y == newValue->y);
//    assert(oldValue->y <= y && oldValue->y + C2DBitMapChunkRowSize > y);
//
//
//    uint32_t * lc = oldValue->content;
//    uint32_t * rc = newValue->content;
//
//    int32_t row = y - oldValue->y;
//    lc += row * C2DBitMapChunkRowSize;
//    rc += row * C2DBitMapChunkRowSize;
//
//    uint32_t offset = 0;
//
//    uint32_t pageIndex = 0;
//    C2DBitMapChangePage_s * page = C2DBitMapChunkChangeGetPage(change, pageIndex);
//
//    for (int x=0; x<C2DBitMapChunkRowSize; x++) {
//        if (lc[x] != rc[x]) {
//            C2DBitMapChange_s * item = &(page->changes[offset]);
//            item->x = oldValue->x + x;
//            item->y = y;
//            item->from =
//            offset += 1;
//        }
//    }
//    change->count += offset;
//    return offset;
//}

static inline C2DBitMapPage_s * _Nonnull C2DBitMapPageCreate(C2DRect frame) {
    C2DBitMapPage_s * result = malloc(sizeof(C2DBitMapPage_s));
    assert(result);
    assert(frame.origin.x >= 0);
    assert(frame.origin.y >= 0);
    assert(frame.size.width > 0);
    assert(frame.size.height > 0);
    
    int32_t countPerRow = (frame.size.width + C2DBitMapChunkSize - 1) >> C2DBitMapChunkShift;
    int32_t numberOfRows = (frame.size.height + C2DBitMapChunkSize - 1) >> C2DBitMapChunkShift;
    size_t size = countPerRow * numberOfRows * 4;
    result->frame = frame;
    result->countPerRow = countPerRow;
    result->numberOfRows = numberOfRows;
    void * content = malloc(size);
    assert(content);
    memset(content, 0, size);
    result->content = content;
    return result;
}
static inline C2DBitMapPage_s * _Nonnull C2DBitMapPageCreateByCopy(C2DBitMapPage_s * _Nonnull ptr) {
    assert(ptr);
    C2DRect frame = ptr->frame;
    assert(frame.origin.x >= 0);
    assert(frame.origin.y >= 0);
    assert(frame.size.width > 0);
    assert(frame.size.height > 0);
    assert((frame.size.width & C2DBitMapChunkMask) == 0);
    assert((frame.size.height & C2DBitMapChunkMask) == 0);

    int32_t countPerRow = frame.size.width;
    int32_t numberOfRows = frame.size.height;
    size_t size = countPerRow * numberOfRows * 4;
    assert(numberOfRows == ptr->numberOfRows);
    assert(countPerRow == ptr->countPerRow);

    C2DBitMapPage_s * result = malloc(sizeof(C2DBitMapPage_s));
    assert(result);
    result->frame = frame;
    result->countPerRow = countPerRow;
    result->numberOfRows = numberOfRows;
    void * content = malloc(size);
    assert(content);
    memcpy(content, ptr->content, size);
    result->content = content;
    return result;
}
static inline void C2DBitMapPageDestroy(C2DBitMapPage_s * _Nonnull ptr) {
    assert(ptr);
    free(ptr->content);
    free(ptr);
}


//static inline uint32_t C2DBitMapContentChanges(C2DBitMapChunk_s * _Nonnull oldValue, C2DBitMapChunk_s * _Nonnull newValue, int32_t y, C2DBitMapChunkChange_s * _Nonnull change) {
//    assert(oldValue);
//    assert(newValue);
//    assert(change);
//    assert(change->count == 0);
//    assert(change->x == newValue->x);
//    assert(change->y == newValue->y);
//
//    assert(oldValue != newValue);
//    assert(oldValue->x == newValue->x);
//    assert(oldValue->y == newValue->y);
//    assert(oldValue->y <= y && oldValue->y + C2DBitMapChunkRowSize > y);
//
//
//    uint32_t * lc = oldValue->content;
//    uint32_t * rc = newValue->content;
//
//    int32_t row = y - oldValue->y;
//    lc += row * C2DBitMapChunkRowSize;
//    rc += row * C2DBitMapChunkRowSize;
//
//    uint32_t offset = 0;
//
//    uint32_t pageIndex = 0;
//    C2DBitMapChangePage_s * page = C2DBitMapChunkChangeGetPage(change, pageIndex);
//
//    for (int x=0; x<C2DBitMapChunkRowSize; x++) {
//        if (lc[x] != rc[x]) {
//            C2DBitMapChange_s * item = &(page->changes[offset]);
//            item->x = oldValue->x + x;
//            item->y = y;
//            item->from =
//            offset += 1;
//        }
//    }
//    change->count += offset;
//    return offset;
//}
//
//static inline C2DBitMapChunk_s * _Nonnull C2DBitMapChunkCreate(C2DRect frame) {
//    C2DBitMapChunk_s * result = malloc(sizeof(C2DBitMapChunk_s));
//    assert(result);
//    assert(frame.origin.x >= 0);
//    assert(frame.origin.y >= 0);
//    assert((frame.origin.x & C2DBitMapChunkRowSizeMask) == 0);
//    assert((frame.origin.y & C2DBitMapChunkRowSizeMask) == 0);
//    result->origin = origin;
//    void * content = malloc(C2DBitMapChunkByteSize);
//    assert(content);
//    memset(content, 0, C2DBitMapChunkByteSize);
//    result->content = content;
//    return result;
//}
//static inline C2DBitMapChunk_s * _Nonnull C2DBitMapChunkCreateByCopy(C2DBitMapChunk_s * _Nonnull ptr) {
//    assert(ptr);
//    assert(ptr->x >= 0);
//    assert(ptr->y >= 0);
//    assert((ptr->x & C2DBitMapChunkRowSizeMask) == 0);
//    assert((ptr->y & C2DBitMapChunkRowSizeMask) == 0);
//    C2DBitMapChunk_s * result = malloc(sizeof(C2DBitMapChunk_s));
//    assert(result);
//    memcpy(result, ptr, sizeof(C2DBitMapChunk_s));
//    void * content = malloc(C2DBitMapChunkByteSize);
//    assert(content);
//    memcpy(content, ptr->content, C2DBitMapChunkByteSize);
//    result->content = content;
//    return result;
//}
//static inline void C2DBitMapChunkDestroy(C2DBitMapChunk_s * _Nonnull ptr) {
//    assert(ptr);
//    free(ptr->content);
//    free(ptr);
//}



typedef struct _C2DBitMapBlock {
    uint32_t x;
    uint32_t y;
    uint64_t t;
    uint32_t * _Nonnull storage;
} C2DBitMapBlock_s;

typedef struct _C2DBitMapBlockCacheItem {
    struct _C2DBitMapBlockCacheItem * _Nullable next;
} C2DBitMapBlockCacheItem_s;

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


typedef struct __C2DBitMapBlockCircularBuffer {
    uint32_t _capacity;
    uint32_t _indexOffset;
    C2DBitMapBlock_s * _Nullable items[];
} C2DBitMapBlockCircularBuffer_s;

typedef struct __C2DBitMapBlockRowCircularBuffer {
    uint32_t _capacity;
    uint32_t _indexOffset;
    C2DBitMapBlockCircularBuffer_s * _Nullable items[];
} C2DBitMapBlockRowCircularBuffer_s;

typedef struct {
    int32_t countLimit;
    int32_t blockCount;
    int32_t blockSize;
    int32_t numberOfCaches;
    C2DBitMapBlockRowCircularBuffer_s * _Nonnull buffers;
    C2DBitMapBlock_s * _Nonnull * _Nonnull caches;
} C2DBitMapBlockPool;


typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} MediaTimingBitmapRect_s;

//static inline _Bool C2DArgbPixelIsEqual(uint32_t a, uint32_t b) {
//    if (a == b) {
//        return true;
//    } else {
//        if (a < 0x1000000UL && b < 0x1000000UL) {
//            return true;
//        } else {
//            return false;
//        }
//    }
//}
//
//static inline _Bool C2DTwoArgbPixelIsEqual(uint64_t a, uint64_t b) {
//    if (a == b) {
//        return true;
//    } else {
//        if ((0xFF000000FF000000ULL & a) == 0) {
//            if ((0xFF000000FF000000ULL & b) == 0) {
//                return true;
//            } else {
//                return false;
//            }
//        } else {
//            return false;
//        }
//    }
//}

C2DBitMapBlock_s * addddd();

_Bool C2DArgbPixelsIsEqual(void const * _Nonnull a, void const * _Nonnull b, uint32_t pixelsCount);




//pixelsCount 必须是4 的整数倍
void C2DLittle16ArgbPixelsSet(void * _Nonnull buffer, uint16_t color, uint32_t pixelsCount);

//pixelsCount 必须是2 的整数倍
void C2DLittle32ArgbPixelsSet(void * _Nonnull buffer, uint32_t color, uint32_t pixelsCount);


void * ContextCreate(void);


#endif /* CoreBitMap_h */
