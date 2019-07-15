//
//  CCCircularBuffer.h
//  CCollection
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCCircularBuffer_h
#define CCCircularBuffer_h

#include "CInteger.h"
#include "CType.h"
#include "CCBase.h"
#include "CCAtomic.h"

static const uint32_t __CCCircularBufferCapacitys[35] = {0, 0x4uL, 0x8uL, 0x10uL, 0x20uL, 0x40uL, 0x80uL, 0x100uL, 0x1c0uL, 0x310uL, 0x55cuL, 0x960uL, 0x1068uL, 0x1cb4uL, 0x3238uL, 0x57e0uL, 0x99c8uL, 0x10d1cuL, 0x1d6f0uL, 0x33824uL, 0x5a23cuL, 0x9dbe8uL, 0x1140d4uL, 0x1e3170uL, 0x34d684uL, 0x5c7764uL, 0xa1d0ecuL, 0x11b2d9cuL, 0x1ef8fd0uL, 0x3633bacuL, 0x5eda86cuL, 0xa5fe6bcuL, 0x1227d3c8uL, 0x1fc5b29cuL, 0x20000000uL};

static inline uint32_t __CCCircularBufferGoodCapacity(uint32_t capacity) {
    int low = 1;
    int high = 34;
    while (low <= high) {
        int mid = low + (high - low)/2;
        
        uint32_t right = __CCCircularBufferCapacitys[mid];
        uint32_t left = __CCCircularBufferCapacitys[mid-1];
        
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
    return __CCCircularBufferCapacitys[34];
}


typedef struct __CCCircularBuffer {
    uint32_t _capacity;
    uint32_t _indexOffset;
    uint32_t _elementSize;
    uint32_t _count;

    uint8_t items[];
} CCCircularBuffer_s;


//range 必须是有效的，方法没有校验range的有效性
static inline int32_t __CCCircularBufferGetItemsInRange(CCCircularBuffer_s * _Nonnull buffer, CCRange_s range, CCVector_s * _Nonnull vectorsPtr) {
    if (range.length == 0) {
        return 0;
    }
    
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
        v0->count = length;
        
        CCVector_s * v1 = vectorsPtr + 1;
        v1->base = buffer->items;
        v1->count = range.length - length;
        return 2;
    } else {
        size_t memoryOffset = index;
        memoryOffset = memoryOffset * buffer->_elementSize;
        vectorsPtr[0].base = &(buffer->items[memoryOffset]);
        vectorsPtr[0].count = range.length;
        return 1;
    }
}

static inline void __CCCircularBufferCopyItemsInRange(CCCircularBuffer_s * _Nonnull buffer, CCRange_s range, void * _Nonnull ptr) {
    CCVector_s vec[2] = {};
    int32_t vecCount =__CCCircularBufferGetItemsInRange(buffer, range, vec);
    
    uint8_t * dst = ptr;
    for (int vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        size_t s = buffer->_elementSize * v.count;
        memcpy(dst, v.base, s);
        dst += s;
    }
}

static inline void * _Nonnull __CCCircularBufferGetItemAtIndex(CCCircularBuffer_s * _Nonnull buffer, uint32_t idx) {
    uint32_t index = idx + buffer->_indexOffset;
    size_t memoryOffset = index;
    memoryOffset = memoryOffset * buffer->_elementSize;
    return &(buffer->items[memoryOffset]);
}

static inline CCCircularBuffer_s * _Nonnull __CCCircularBufferResize(CCCircularBuffer_s * _Nonnull buffer, uint32_t capacity, uint32_t elementSize) {
    assert(elementSize <= CCElementSizeLimit);
    assert(elementSize > 0);
    uint32_t realCapacity = __CCCircularBufferGoodCapacity(capacity);
    assert(realCapacity <= CCCountLimit);

    assert(capacity <= CCCountLimit);
    uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
    assert(elementSize <= CCCircularBufferElementsSizeLimit);
    
    if (realCapacity != buffer->_capacity) {
        assert(buffer->_elementSize == elementSize);
        
        CCCircularBuffer_s * result = CCAllocate(elementsSize + sizeof(CCCircularBuffer_s));
        
        result->_capacity = realCapacity;
        result->_elementSize = elementSize;
        result->_indexOffset = 0;
        result->_count = buffer->_count;
        
        CCVector_s vec[2] = {};
        int32_t vecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(0, buffer->_count), vec);
        uint8_t * dst = result->items;
        for (int vi=0; vi<vecCount; vi++) {
            CCVector_s v = vec[vi];
            size_t s = elementSize * v.count;
            memcpy(dst, v.base, s);
            dst += s;
        }
        CCDeallocate(buffer);
        return result;
    } else {
        return buffer;
    }
}

static inline CCCircularBuffer_s * _Nonnull __CCCircularBufferCopy(CCCircularBuffer_s * _Nonnull buffer, uint32_t capacity) {
    uint32_t elementSize = buffer->_elementSize;
    assert(elementSize <= CCElementSizeLimit);
    assert(elementSize > 0);
    assert(buffer);

    assert(capacity <= CCCountLimit);
    uint32_t realCapacity = __CCCircularBufferGoodCapacity(capacity);
    assert(realCapacity <= CCCountLimit);

    uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
    assert(elementSize <= CCCircularBufferElementsSizeLimit);
    
    CCCircularBuffer_s * result = CCAllocate(elementsSize + sizeof(CCCircularBuffer_s));
    
    result->_capacity = realCapacity;
    result->_elementSize = elementSize;
    result->_indexOffset = 0;
    result->_count = buffer->_count;
    
    CCVector_s vec[2] = {};
    int32_t vecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(0, buffer->_count), vec);
    uint8_t * dst = result->items;
    for (int vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        size_t s = elementSize * v.count;
        memcpy(dst, v.base, s);
        dst += s;
    }
    return result;
}

static inline CCCircularBuffer_s * _Nonnull __CCCircularBufferCreate(uint32_t elementSize, const CCVector_s * _Nullable vec, uint32_t vecCount) {
    if (vecCount > 0) {
        assert(vec);
    }
    
    assert(elementSize <= CCElementSizeLimit);
    assert(elementSize > 0);

    
    uint64_t count = 0;
    for (int vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        count += v.count;
    }
    
    assert(count <= CCCountLimit);
    uint32_t realCapacity = __CCCircularBufferGoodCapacity((uint32_t)count);
    assert(realCapacity <= CCCountLimit);
    
    uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
    assert(elementSize <= CCCircularBufferElementsSizeLimit);
    
    CCCircularBuffer_s * result = CCAllocate(elementsSize + sizeof(CCCircularBuffer_s));
    
    result->_capacity = realCapacity;
    result->_elementSize = elementSize;
    result->_indexOffset = 0;
    result->_count = (uint32_t)count;

    uint8_t * dst = result->items;
    for (int vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        size_t s = elementSize * v.count;
        memcpy(dst, v.base, s);
        dst += s;
    }

    return result;
}

static inline void ___CCCircularBufferSetValues(CCCircularBuffer_s * _Nonnull buffer, uint32_t beginIndex, CCVector_s vector) {
    CCVector_s vec[2] = {};
    int32_t vecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(beginIndex, (uint32_t)vector.count), vec);
    
    size_t elementSize = buffer->_elementSize;
    uint8_t * src = vector.base;
    for (int32_t vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        size_t s = elementSize * v.count;
        memcpy(v.base, src, s);
        src += s;
    }
}

static inline void ___CCCircularBufferSetValuesInRange(CCCircularBuffer_s * _Nonnull buffer, CCRange_s range, void * _Nonnull ptr) {
    CCVector_s vec[2] = {};
    int32_t vecCount = __CCCircularBufferGetItemsInRange(buffer, range, vec);

    size_t elementSize = buffer->_elementSize;
    uint8_t * src = ptr;
    for (int32_t vi=0; vi<vecCount; vi++) {
        CCVector_s v = vec[vi];
        size_t s = elementSize * v.count;
        memcpy(v.base, src, s);
        src += s;
    }
}

static inline void ___CCCircularBufferMoveIndexOffset(CCCircularBuffer_s * _Nonnull buffer, int32_t offset) {
    if (0 == offset) {
        return;
    }
    assert(offset <= CCCountLimit);
    assert(offset >= -CCCountLimit);
    
    int32_t indexOffset = buffer->_indexOffset;
    indexOffset = indexOffset + offset;
    
    while (indexOffset < 0) {
        indexOffset += buffer->_capacity;
    }
    while (indexOffset > buffer->_capacity) {
        indexOffset -= buffer->_capacity;
    }
    buffer->_indexOffset = indexOffset;
}

//range 必须是 有效的区间，
static inline void ___CCCircularBufferMoveValues(CCCircularBuffer_s * _Nonnull buffer, CCRange_s range, int32_t offset) {
    if (0 == offset) {
        return;
    }

    int32_t offsetMin = - range.location;
    int32_t offsetMax = (buffer->_count - range.location - range.length);

    assert(offset >= offsetMin);
    assert(offset <= offsetMax);
    uint8_t tmp[CCElementSizeLimit] = {};
    uint32_t stepSize = CCElementSizeLimit / buffer->_elementSize;
    
    if (offset > 0) {//后移
        uint32_t oldEndIndex = range.location + range.length;
        uint32_t newEndIndex = oldEndIndex + offset;
        uint32_t completedLength = 0;
        while (completedLength < range.length) {
            uint32_t len = stepSize;
            uint32_t remain = range.length - completedLength;
            if (len > remain) {
                len = remain;
            }
            __CCCircularBufferCopyItemsInRange(buffer, CCRangeMake(oldEndIndex - completedLength, len), tmp);
            ___CCCircularBufferSetValuesInRange(buffer, CCRangeMake(newEndIndex - completedLength, len), tmp);
            completedLength += len;
        }
    } else {//前移
        uint32_t oldBeginIndex = range.location;
        uint32_t newBeginIndex = oldBeginIndex + offset;
        uint32_t completedLength = 0;
        while (completedLength < range.length) {
            uint32_t len = stepSize;
            uint32_t remain = range.length - completedLength;
            if (len > remain) {
                len = remain;
            }
            __CCCircularBufferCopyItemsInRange(buffer, CCRangeMake(oldBeginIndex + completedLength, len), tmp);
            ___CCCircularBufferSetValuesInRange(buffer, CCRangeMake(newBeginIndex + completedLength, len), tmp);
            completedLength += len;
        }
    }
}

// This function does no ObjC dispatch or argument checking;
// It should only be called from places where that dispatch and check has already been done, or NSCCArray
static inline CCCircularBuffer_s * _Nonnull __CCCircularBufferReplaceValues(CCCircularBuffer_s * _Nonnull buffer, CCRange_s range, const CCVector_s * _Nullable newVectors, int32_t newVectorCount) {
    assert(newVectorCount >= 0);
    if (newVectorCount > 0) {
        assert(newVectors);
    }
    
    uint32_t elementSize = buffer->_elementSize;
    uint64_t count = 0;
    for (int32_t vi=0; vi<newVectorCount; vi++) {
        CCVector_s v = newVectors[vi];
        count += v.count;
    }
    
    assert(count <= CCCountLimit);
    uint32_t newCount = (uint32_t)count;

    uint32_t capacity = buffer->_count - range.length + newCount;
    if (capacity > buffer->_capacity) {//拓容
        uint32_t realCapacity = __CCCircularBufferGoodCapacity(capacity);
        assert(realCapacity <= CCCountLimit);
        assert(capacity <= CCCountLimit);
        uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
        assert(elementSize <= CCCircularBufferElementsSizeLimit);
        
        CCCircularBuffer_s * result = CCAllocate(elementsSize + sizeof(CCCircularBuffer_s));
        
        result->_capacity = realCapacity;
        result->_elementSize = elementSize;
        result->_indexOffset = 0;
        result->_count = capacity;
        
        CCVector_s tVec[2] = {};
        int32_t tVecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(0, range.location), tVec);
        uint32_t index = 0;
        for (int32_t vi=0; vi<tVecCount; vi++) {
            CCVector_s v = tVec[vi];
            ___CCCircularBufferSetValues(result, index, v);
            index += v.count;
        }
        
        for (int32_t vi=0; vi<newVectorCount; vi++) {
            CCVector_s v = newVectors[vi];
            ___CCCircularBufferSetValues(result, index, v);
            index += v.count;
        }
        
        tVecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(range.location + newCount, capacity - range.location - newCount), tVec);
        for (int32_t vi=0; vi<tVecCount; vi++) {
            CCVector_s v = tVec[vi];
            ___CCCircularBufferSetValues(result, index, v);
            index += v.count;
        }
        
        CCDeallocate(buffer);
        return result;
    } else {
        if (range.length == newCount) {
            uint32_t index = range.location;
            for (int32_t vi=0; vi<newVectorCount; vi++) {
                CCVector_s v = newVectors[vi];
                ___CCCircularBufferSetValues(buffer, index, v);
                index += v.count;
            }
        } else {
            uint32_t rightCount = buffer->_count - range.location - range.length;
            int32_t countChanged = (int32_t)newCount - (int32_t)range.length;
            if (range.location < rightCount) {//移动首部
                int32_t offset = - countChanged;
                if (offset < 0) {//前移
                    ___CCCircularBufferMoveIndexOffset(buffer, offset);
                    buffer->_count = capacity;
                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(countChanged, range.location), offset);
                } else {//后移
                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(0, range.location), offset);
                    ___CCCircularBufferMoveIndexOffset(buffer, offset);
                    buffer->_count = capacity;
                }
            } else {//移动尾部
                int32_t offset = countChanged;
                if (offset < 0) {//前移
                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(range.location + range.length, rightCount), offset);
                    buffer->_count = capacity;
                } else {//后移
                    buffer->_count = capacity;
                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(range.location + range.length, rightCount), offset);
                }
            }
            uint32_t index = range.location;
            for (int32_t vi=0; vi<newVectorCount; vi++) {
                CCVector_s v = newVectors[vi];
                ___CCCircularBufferSetValues(buffer, index, v);
                index += v.count;
            }
        }
        return buffer;
    }
}



#endif /* CCCircularBuffer_h */
