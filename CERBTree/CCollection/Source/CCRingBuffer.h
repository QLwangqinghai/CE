//
//  CCRingBuffer.h
//  CCollection
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCRingBuffer_h
#define CCRingBuffer_h

#include "CInteger.h"
#include "CType.h"
#include "CCBase.h"
#include "CCAtomic.h"


typedef struct __CCRingBuffer {
    uint32_t _capacity;
    uint32_t _elementSize;
    
    uint32_t _indexOffset;
    uint32_t _indexMask;
    
    uint8_t items[];
} CCRingBuffer_s;

//range 必须是有效的，方法没有校验range的有效性
static inline int32_t __CCRingBufferGetItemsInRange(CCRingBuffer_s * _Nonnull buffer, CCRange_s range, CCVector_s * _Nonnull vectorsPtr) {
    uint32_t index = range.location + buffer->_indexOffset;
    index = index & buffer->_indexMask;
    if (index + range.length > buffer->_capacity) {//2部分
        uint32_t length = buffer->_capacity - index;
        size_t memoryOffset = index;
        memoryOffset = memoryOffset * buffer->_elementSize;
        
        CCVector_s * v0 = vectorsPtr;
        v0->base = &(buffer->items[memoryOffset]);
        v0->itemCount = length;
        
        CCVector_s * v1 = vectorsPtr + 1;
        v1->base = buffer->items;
        v1->itemCount = range.length - length;
        return 2;
    } else {
        size_t memoryOffset = index;
        memoryOffset = memoryOffset * buffer->_elementSize;
        vectorsPtr[0].base = &(buffer->items[memoryOffset]);
        vectorsPtr[0].itemCount = range.length;
        return 1;
    }
}

#endif /* CCRingBuffer_h */
