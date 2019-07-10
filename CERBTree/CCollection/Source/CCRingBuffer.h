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

static const uint32_t __CCRingBufferCapacitys[35] = {0, 0x4uL, 0x8uL, 0x10uL, 0x20uL, 0x40uL, 0x80uL, 0x100uL, 0x1c0uL, 0x310uL, 0x55cuL, 0x960uL, 0x1068uL, 0x1cb4uL, 0x3238uL, 0x57e0uL, 0x99c8uL, 0x10d1cuL, 0x1d6f0uL, 0x33824uL, 0x5a23cuL, 0x9dbe8uL, 0x1140d4uL, 0x1e3170uL, 0x34d684uL, 0x5c7764uL, 0xa1d0ecuL, 0x11b2d9cuL, 0x1ef8fd0uL, 0x3633bacuL, 0x5eda86cuL, 0xa5fe6bcuL, 0x1227d3c8uL, 0x1fc5b29cuL, 0x20000000uL};

static inline uint32_t __CCRingBufferGoodCapacity(uint32_t capacity) {
    int low = 1;
    int high = 34;
    while (low <= high) {
        int mid = low + (high - low)/2;
        
        uint32_t right = __CCRingBufferCapacitys[mid];
        uint32_t left = __CCRingBufferCapacitys[mid-1];
        
        if (right > capacity && left <= capacity) {
            return right;
        } else {
            if (right < capacity) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
    }
    return __CCRingBufferCapacitys[34];
}


typedef struct __CCRingBuffer {
    uint32_t _capacity;
    uint32_t _indexOffset;
    uint32_t _elementSize;
    uint32_t _count;

    uint8_t items[];
} CCRingBuffer_s;


//range 必须是有效的，方法没有校验range的有效性
static inline int32_t __CCRingBufferGetItemsInRange(CCRingBuffer_s * _Nonnull buffer, CCRange_s range, CCVector_s * _Nonnull vectorsPtr) {
    uint32_t index = range.location + buffer->_indexOffset;
    if (index > buffer->_capacity) {
        index = index - buffer->_capacity;
    }
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

static inline void * _Nonnull __CCRingBufferGetItemAtIndex(CCRingBuffer_s * _Nonnull buffer, uint32_t idx) {
    uint32_t index = idx + buffer->_indexOffset;
    size_t memoryOffset = index;
    memoryOffset = memoryOffset * buffer->_elementSize;
    return &(buffer->items[memoryOffset]);
}

static inline CCRingBuffer_s * _Nonnull __CCRingBufferResize(CCRingBuffer_s * _Nullable buffer, uint32_t capacity, uint32_t elementSize) {
    assert(elementSize <= CCElementSizeLimit);
    assert(elementSize > 0);
    uint32_t realCapacity = __CCRingBufferGoodCapacity(capacity);
    assert(realCapacity <= CCCountLimit);

    assert(capacity <= CCCountLimit);
    uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
    assert(elementSize <= CCRingBufferElementsSizeLimit);
    
    CCRingBuffer_s * result = NULL;
    if (NULL == buffer) {
        result = CCAllocate(elementsSize + sizeof(CCRingBuffer_s));
        
        result->_capacity = realCapacity;
        result->_elementSize = elementSize;
        result->_indexOffset = 0;
        result->_count = 0;
        
        return result;
    } else {
        if (realCapacity != buffer->_capacity) {
            assert(buffer->_elementSize == elementSize);
            
            result = CCAllocate(elementsSize + sizeof(CCRingBuffer_s));
            
            result->_capacity = realCapacity;
            result->_elementSize = elementSize;
            result->_indexOffset = 0;
            result->_count = buffer->_count;
            
            CCVector_s vec[2] = {};
            int vecCount = __CCRingBufferGetItemsInRange(buffer, CCRangeMake(0, buffer->_count), vec);
            uint8_t * dst = result->items;
            for (int vi=0; vi<vecCount; vi++) {
                CCVector_s v = vec[vi];
                size_t s = elementSize * v.itemCount;
                memcpy(dst, v.base, s);
                dst += s;
            }
            CCDeallocate(buffer);
            return result;
        } else {
            return buffer;
        }
    }
}

static inline CCRingBuffer_s * _Nonnull __CCRingBufferCopy(CCRingBuffer_s * _Nonnull buffer, uint32_t capacity) {
    uint32_t elementSize = buffer->_elementSize;
    assert(elementSize <= CCElementSizeLimit);
    assert(elementSize > 0);
    assert(buffer);

    assert(capacity <= CCCountLimit);
    uint32_t realCapacity = __CCRingBufferGoodCapacity(capacity);
    assert(realCapacity <= CCCountLimit);

    uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
    assert(elementSize <= CCRingBufferElementsSizeLimit);
    
    CCRingBuffer_s * result = CCAllocate(elementsSize + sizeof(CCRingBuffer_s));
    
    result->_capacity = realCapacity;
    result->_elementSize = elementSize;
    result->_indexOffset = 0;
    result->_count = buffer->_count;
    
    CCVector_s vec[2] = {};
    int vecCount = __CCRingBufferGetItemsInRange(buffer, CCRangeMake(0, buffer->_count), vec);
    uint8_t * dst = result->items;
    for (int vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        size_t s = elementSize * v.itemCount;
        memcpy(dst, v.base, s);
        dst += s;
    }
    return result;
}

static inline CCRingBuffer_s * _Nonnull __CCRingBufferCreate(uint32_t elementSize, const CCVector_s * _Nullable vec, uint32_t vecCount) {
    if (vecCount > 0) {
        assert(vec);
    }
    
    assert(elementSize <= CCElementSizeLimit);
    assert(elementSize > 0);

    
    uint64_t count = 0;
    for (int vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        count += v.itemCount;
    }
    
    assert(count <= CCCountLimit);
    uint32_t realCapacity = __CCRingBufferGoodCapacity((uint32_t)count);
    assert(realCapacity <= CCCountLimit);
    
    uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
    assert(elementSize <= CCRingBufferElementsSizeLimit);
    
    CCRingBuffer_s * result = CCAllocate(elementsSize + sizeof(CCRingBuffer_s));
    
    result->_capacity = realCapacity;
    result->_elementSize = elementSize;
    result->_indexOffset = 0;
    result->_count = (uint32_t)count;

    uint8_t * dst = result->items;
    for (int vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        size_t s = elementSize * v.itemCount;
        memcpy(dst, v.base, s);
        dst += s;
    }

    return result;
}




#endif /* CCRingBuffer_h */
