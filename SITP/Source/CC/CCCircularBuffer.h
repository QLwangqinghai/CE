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
#include "CCPageTable.h"


/*
 pageIndex = index / bufferElementCapacity;
 
 64:
 pageIndexInGroup = pageIndex & CCPageIndexMask
 groupIndex = pageIndex >> 18

 */



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

//typedef struct __CCCircularBufferPageTable {
//    CCIndex _offset;
//    CCIndex _capacity;
//    CCIndex _count;
//    CCIndex _sectionCapacity;
//    void * _Nonnull _content;
//} CCCircularBufferPageTable_s;

typedef struct __CCCircularBufferApi {
    uint32_t _elementSize;
    uint32_t _deep;

} CCCircularBufferApi_s;

#pragma pack(push)
#pragma pack(2)

typedef struct __CCCircularBuffer {
    uint32_t _elementSize: 26;
    uint32_t _quicklyElementSizeShift: 6;
    uint32_t _bufferElementCapacity;
    CCIndex _capacity; //当_capacity <= _bufferElementCapacity 时， _storage 为一维; _capacity > _bufferElementCapacity时 为2维或者3维, _storage为CCCircularBufferPageTable_s
    CCIndex _indexOffset;// <= _capacity
    CCIndex _count;
    void * _Nonnull _storage;
} CCCircularBuffer_s;

static inline CCIndex __CCCircularBufferIndexToLoaction(CCCircularBuffer_s * _Nonnull buffer, CCIndex index) {
    CCIndex r = buffer->_capacity - buffer->_indexOffset;
    if (index >= r) {
        return index - r;
    } else {
        return index + buffer->_indexOffset;
    }
}

static inline CCIndex __CCCircularBufferLoactionToIndex(CCCircularBuffer_s * _Nonnull buffer, CCIndex location) {
    if (location >= buffer->_indexOffset) {
        return location - buffer->_indexOffset;
    } else {
        return buffer->_capacity - (buffer->_indexOffset - location);
    }
}

#if CCBuild64Bit
typedef struct {
    uint64_t index: 21;
    uint64_t pageIndex: 43;
} CCCircularBufferLocationPath;
//没有校验index有效性, index < _count
static inline CCCircularBufferLocationPath __CCCircularBufferGetLocationPath(CCCircularBuffer_s * _Nonnull buffer, CCIndex location) {
    CCCircularBufferLocationPath indexPath = {};
    indexPath.index = (location % buffer->_bufferElementCapacity);
    indexPath.pageIndex = (location / buffer->_bufferElementCapacity);
    return indexPath;
}
#else
typedef struct {
    uint32_t index: 16;
    uint32_t pageIndex: 16;
} CCCircularBufferLocationPath;

//没有校验index有效性, index < _count
static inline CCCircularBufferLocationPath __CCCircularBufferGetLocationPath(CCCircularBuffer_s * _Nonnull buffer, CCIndex index) {
    CCCircularBufferLocationPath indexPath = {};
    indexPath.index = (location % buffer->_bufferElementCapacity);
    indexPath.bufferIndex = (location / buffer->_bufferElementCapacity);
    return indexPath;
}
#endif

#pragma pack(pop)

//
////range 必须是有效的，方法没有校验range的有效性
//static inline int32_t __CCCircularBufferGetItemsInRange(CCCircularBuffer_s * _Nonnull buffer, CCRange range, CCVector_s * _Nonnull vectorsPtr) {
//    if (range.length == 0) {
//        return 0;
//    }
//
//    CCCircularBufferLocationPath indexPath = __CCCircularBufferGetIndexPath(buffer, range.location);
//
//    CCIndex index = range.location + buffer->_indexOffset;
//    if (index > buffer->_capacity) {
//        index = index - buffer->_capacity;
//    }
//    if (index + range.length > buffer->_capacity) {//2部分
//        uint32_t length = buffer->_capacity - index;
//        size_t memoryOffset = index;
//        memoryOffset = memoryOffset * buffer->_elementSize;
//
//        CCVector_s * v0 = vectorsPtr;
//        v0->base = &(buffer->items[memoryOffset]);
//        v0->count = length;
//
//        CCVector_s * v1 = vectorsPtr + 1;
//        v1->base = buffer->items;
//        v1->count = range.length - length;
//        return 2;
//    } else {
//        size_t memoryOffset = index;
//        memoryOffset = memoryOffset * buffer->_elementSize;
//        vectorsPtr[0].base = &(buffer->items[memoryOffset]);
//        vectorsPtr[0].count = range.length;
//        return 1;
//    }
//}
//
//static inline void __CCCircularBufferCopyItemsInRange(CCCircularBuffer_s * _Nonnull buffer, CCRange range, void * _Nonnull ptr) {
//    CCVector_s vec[2] = {};
//    int32_t vecCount =__CCCircularBufferGetItemsInRange(buffer, range, vec);
//
//    uint8_t * dst = ptr;
//    for (int vi=0; vi<vecCount; vi++) {
//        CCVector_s v = vec[vi];
//        size_t s = buffer->_elementSize * v.count;
//        memcpy(dst, v.base, s);
//        dst += s;
//    }
//}
//
//static inline void * _Nonnull __CCCircularBufferGetItemAtIndex(CCCircularBuffer_s * _Nonnull buffer, uint32_t idx) {
//    uint32_t index = idx + buffer->_indexOffset;
//    size_t memoryOffset = index;
//    memoryOffset = memoryOffset * buffer->_elementSize;
//    return &(buffer->items[memoryOffset]);
//}
//
//static inline CCCircularBuffer_s * _Nonnull __CCCircularBufferResize(CCCircularBuffer_s * _Nonnull buffer, uint32_t capacity, uint32_t elementSize) {
//    assert(elementSize <= CCElementSizeLimit);
//    assert(elementSize > 0);
//    uint32_t realCapacity = __CCCircularBufferGoodCapacity(capacity);
//    assert(realCapacity <= CCCountLimit);
//
//    assert(capacity <= CCCountLimit);
//    uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
//    assert(elementSize <= CCCircularBufferElementsSizeLimit);
//
//    if (realCapacity != buffer->_capacity) {
//        assert(buffer->_elementSize == elementSize);
//
//        CCCircularBuffer_s * result = CCAllocate(elementsSize + sizeof(CCCircularBuffer_s));
//
//        result->_capacity = realCapacity;
//        result->_elementSize = elementSize;
//        result->_indexOffset = 0;
//        result->_count = buffer->_count;
//
//        CCVector_s vec[2] = {};
//        int32_t vecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(0, buffer->_count), vec);
//        uint8_t * dst = result->items;
//        for (int vi=0; vi<vecCount; vi++) {
//            CCVector_s v = vec[vi];
//            size_t s = elementSize * v.count;
//            memcpy(dst, v.base, s);
//            dst += s;
//        }
//        CCDeallocate(buffer);
//        return result;
//    } else {
//        return buffer;
//    }
//}
//
//static inline CCCircularBuffer_s * _Nonnull __CCCircularBufferCopy(CCCircularBuffer_s * _Nonnull buffer, uint32_t capacity) {
//    uint32_t elementSize = buffer->_elementSize;
//    assert(elementSize <= CCElementSizeLimit);
//    assert(elementSize > 0);
//    assert(buffer);
//
//    assert(capacity <= CCCountLimit);
//    uint32_t realCapacity = __CCCircularBufferGoodCapacity(capacity);
//    assert(realCapacity <= CCCountLimit);
//
//    uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
//    assert(elementSize <= CCCircularBufferElementsSizeLimit);
//
//    CCCircularBuffer_s * result = CCAllocate(elementsSize + sizeof(CCCircularBuffer_s));
//
//    result->_capacity = realCapacity;
//    result->_elementSize = elementSize;
//    result->_indexOffset = 0;
//    result->_count = buffer->_count;
//
//    CCVector_s vec[2] = {};
//    int32_t vecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(0, buffer->_count), vec);
//    uint8_t * dst = result->items;
//    for (int vi=0; vi<vecCount; vi++) {
//        CCVector_s v = vec[vi];
//        size_t s = elementSize * v.count;
//        memcpy(dst, v.base, s);
//        dst += s;
//    }
//    return result;
//}

//static inline CCCircularBuffer_s * _Nonnull __CCCircularBufferCreate(uint32_t elementSize, const CCVector_s * _Nullable vec, uint32_t vecCount) {
//    if (vecCount > 0) {
//        assert(vec);
//    }
//
//    assert(elementSize <= CCElementSizeLimit);
//    assert(elementSize > 0);
//
//
//    uint64_t count = 0;
//    for (int vi=0; vi<vecCount; vi++) {
//        CCVector_s v = vec[vi];
//        count += v.count;
//    }
//
//    assert(count <= CCCountLimit);
//    uint32_t realCapacity = __CCCircularBufferGoodCapacity((uint32_t)count);
//    assert(realCapacity <= CCCountLimit);
//
//    uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
//    assert(elementSize <= CCCircularBufferElementsSizeLimit);
//
//    CCCircularBuffer_s * result = CCAllocate(elementsSize + sizeof(CCCircularBuffer_s));
//
//    result->_capacity = realCapacity;
//    result->_elementSize = elementSize;
//    result->_indexOffset = 0;
//    result->_count = (uint32_t)count;
//
//    uint8_t * dst = result->items;
//    for (int vi=0; vi<vecCount; vi++) {
//        CCVector_s v = vec[vi];
//        size_t s = elementSize * v.count;
//        memcpy(dst, v.base, s);
//        dst += s;
//    }
//
//    return result;
//}
//
//static inline void ___CCCircularBufferSetValues(CCCircularBuffer_s * _Nonnull buffer, uint32_t beginIndex, CCVector_s vector) {
//    CCVector_s vec[2] = {};
//    int32_t vecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(beginIndex, (uint32_t)vector.count), vec);
//
//    size_t elementSize = buffer->_elementSize;
//    uint8_t * src = vector.base;
//    for (int32_t vi=0; vi<vecCount; vi++) {
//        CCVector_s v = vec[vi];
//        size_t s = elementSize * v.count;
//        memcpy(v.base, src, s);
//        src += s;
//    }
//}
//
//static inline void ___CCCircularBufferSetValuesInRange(CCCircularBuffer_s * _Nonnull buffer, CCRange range, void * _Nonnull ptr) {
//    CCVector_s vec[2] = {};
//    int32_t vecCount = __CCCircularBufferGetItemsInRange(buffer, range, vec);
//
//    size_t elementSize = buffer->_elementSize;
//    uint8_t * src = ptr;
//    for (int32_t vi=0; vi<vecCount; vi++) {
//        CCVector_s v = vec[vi];
//        size_t s = elementSize * v.count;
//        memcpy(v.base, src, s);
//        src += s;
//    }
//}
//
//static inline void ___CCCircularBufferMoveIndexOffset(CCCircularBuffer_s * _Nonnull buffer, int32_t offset) {
//    if (0 == offset) {
//        return;
//    }
//    assert(offset <= CCCountLimit);
//    assert(offset >= -CCCountLimit);
//
//    int32_t indexOffset = buffer->_indexOffset;
//    indexOffset = indexOffset + offset;
//
//    while (indexOffset < 0) {
//        indexOffset += buffer->_capacity;
//    }
//    while (indexOffset > buffer->_capacity) {
//        indexOffset -= buffer->_capacity;
//    }
//    buffer->_indexOffset = indexOffset;
//}
//
////range 必须是 有效的区间，
//static inline void ___CCCircularBufferMoveValues(CCCircularBuffer_s * _Nonnull buffer, CCRange range, int32_t offset) {
//    if (0 == offset) {
//        return;
//    }
//
//    int32_t offsetMin = - range.location;
//    int32_t offsetMax = (buffer->_count - range.location - range.length);
//
//    assert(offset >= offsetMin);
//    assert(offset <= offsetMax);
//    uint8_t tmp[CCDefineElementSizeLimit] = {};
//    uint32_t stepSize = CCElementSizeLimit / buffer->_elementSize;
//
//    if (offset > 0) {//后移
//        uint32_t oldEndIndex = range.location + range.length;
//        uint32_t newEndIndex = oldEndIndex + offset;
//        uint32_t completedLength = 0;
//        while (completedLength < range.length) {
//            uint32_t len = stepSize;
//            uint32_t remain = range.length - completedLength;
//            if (len > remain) {
//                len = remain;
//            }
//            __CCCircularBufferCopyItemsInRange(buffer, CCRangeMake(oldEndIndex - completedLength, len), tmp);
//            ___CCCircularBufferSetValuesInRange(buffer, CCRangeMake(newEndIndex - completedLength, len), tmp);
//            completedLength += len;
//        }
//    } else {//前移
//        uint32_t oldBeginIndex = range.location;
//        uint32_t newBeginIndex = oldBeginIndex + offset;
//        uint32_t completedLength = 0;
//        while (completedLength < range.length) {
//            uint32_t len = stepSize;
//            uint32_t remain = range.length - completedLength;
//            if (len > remain) {
//                len = remain;
//            }
//            __CCCircularBufferCopyItemsInRange(buffer, CCRangeMake(oldBeginIndex + completedLength, len), tmp);
//            ___CCCircularBufferSetValuesInRange(buffer, CCRangeMake(newBeginIndex + completedLength, len), tmp);
//            completedLength += len;
//        }
//    }
//}

//
//// This function does no ObjC dispatch or argument checking;
//// It should only be called from places where that dispatch and check has already been done, or NSCCArray
//static inline CCCircularBuffer_s * _Nonnull __CCCircularBufferReplaceValues(CCCircularBuffer_s * _Nonnull buffer, CCRange range, const CCVector_s * _Nullable newVectors, int32_t newVectorCount) {
//    assert(newVectorCount >= 0);
//    if (newVectorCount > 0) {
//        assert(newVectors);
//    }
//
//    uint32_t elementSize = buffer->_elementSize;
//    uint64_t count = 0;
//    for (int32_t vi=0; vi<newVectorCount; vi++) {
//        CCVector_s v = newVectors[vi];
//        count += v.count;
//    }
//
//    assert(count <= CCCountLimit);
//    uint32_t newCount = (uint32_t)count;
//
//    uint32_t capacity = buffer->_count - range.length + newCount;
//    if (capacity > buffer->_capacity) {//拓容
//        uint32_t realCapacity = __CCCircularBufferGoodCapacity(capacity);
//        assert(realCapacity <= CCCountLimit);
//        assert(capacity <= CCCountLimit);
//        uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
//        assert(elementSize <= CCCircularBufferElementsSizeLimit);
//
//        CCCircularBuffer_s * result = CCAllocate(elementsSize + sizeof(CCCircularBuffer_s));
//
//        result->_capacity = realCapacity;
//        result->_elementSize = elementSize;
//        result->_indexOffset = 0;
//        result->_count = capacity;
//
//        CCVector_s tVec[2] = {};
//        int32_t tVecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(0, range.location), tVec);
//        uint32_t index = 0;
//        for (int32_t vi=0; vi<tVecCount; vi++) {
//            CCVector_s v = tVec[vi];
//            ___CCCircularBufferSetValues(result, index, v);
//            index += v.count;
//        }
//
//        for (int32_t vi=0; vi<newVectorCount; vi++) {
//            CCVector_s v = newVectors[vi];
//            ___CCCircularBufferSetValues(result, index, v);
//            index += v.count;
//        }
//
//        tVecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(range.location + newCount, capacity - range.location - newCount), tVec);
//        for (int32_t vi=0; vi<tVecCount; vi++) {
//            CCVector_s v = tVec[vi];
//            ___CCCircularBufferSetValues(result, index, v);
//            index += v.count;
//        }
//
//        CCDeallocate(buffer);
//        return result;
//    } else {
//        if (range.length == newCount) {
//            uint32_t index = range.location;
//            for (int32_t vi=0; vi<newVectorCount; vi++) {
//                CCVector_s v = newVectors[vi];
//                ___CCCircularBufferSetValues(buffer, index, v);
//                index += v.count;
//            }
//        } else {
//            uint32_t rightCount = buffer->_count - range.location - range.length;
//            int32_t countChanged = (int32_t)newCount - (int32_t)range.length;
//            if (range.location < rightCount) {//移动首部
//                int32_t offset = - countChanged;
//                if (offset < 0) {//前移
//                    ___CCCircularBufferMoveIndexOffset(buffer, offset);
//                    buffer->_count = capacity;
//                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(countChanged, range.location), offset);
//                } else {//后移
//                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(0, range.location), offset);
//                    ___CCCircularBufferMoveIndexOffset(buffer, offset);
//                    buffer->_count = capacity;
//                }
//            } else {//移动尾部
//                int32_t offset = countChanged;
//                if (offset < 0) {//前移
//                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(range.location + range.length, rightCount), offset);
//                    buffer->_count = capacity;
//                } else {//后移
//                    buffer->_count = capacity;
//                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(range.location + range.length, rightCount), offset);
//                }
//            }
//            uint32_t index = range.location;
//            for (int32_t vi=0; vi<newVectorCount; vi++) {
//                CCVector_s v = newVectors[vi];
//                ___CCCircularBufferSetValues(buffer, index, v);
//                index += v.count;
//            }
//        }
//        return buffer;
//    }
//}
//

typedef struct {
    CCRange range;
    CCIndex newLength;
    void * _Nullable newValues;
} CCRangeReplace;

typedef void (*CCCircularBufferUpdateCallBack_f)(void * _Nullable context, CCRange range, uint32_t elementSize, const void * _Nonnull values);

typedef struct {
    uint8_t * _Nullable * _Nullable pages;
    void * _Nullable groups;
} CCCircularBufferStorage;


//return > 0
static inline CCIndex __CCCircularBufferAlignCapacity(CCCircularBuffer_s * _Nonnull buffer, CCIndex capacity) {
    if (capacity == 0) {
        return 0;
    } else if (capacity > buffer->_bufferElementCapacity) {
        return (capacity + buffer->_bufferElementCapacity - 1) / buffer->_bufferElementCapacity * buffer->_bufferElementCapacity;
    } else {
        return CCPowerAlign2(capacity);
    }
}

static inline void __CCCircularBufferResetStorage(CCCircularBuffer_s * _Nonnull buffer, void * _Nullable storage, CCIndex capacity, CCIndex indexOffset) {
    buffer->_storage = storage;
    buffer->_capacity = capacity;
    buffer->_indexOffset = indexOffset;
}


static inline uint8_t * _Nonnull __CCCircularBufferGetPage(CCCircularBuffer_s * _Nonnull buffer, CCIndex pageIndex) {
    if (buffer->_capacity <= buffer->_bufferElementCapacity) {
        //1维
        assert(pageIndex == 0);
        return buffer->_storage;
    } else {
        CCPageTablePtr table = (CCPageTablePtr)(buffer->_storage);
        return __CCPageTableGetPage(table, pageIndex);
    }
}


typedef void (*CCCircularBufferEnumerateCallBack_f)(void * _Nullable context, CCRange range, size_t elementSize, const void * _Nonnull values);

static inline void __CCCircularBufferEnumerate(CCCircularBuffer_s * _Nonnull buffer, CCRange range, void * _Nullable context, CCCircularBufferEnumerateCallBack_f _Nonnull func) {
    assert(buffer);
    assert(func);
    if (range.length == 0) {
        return;
    }
    assert(range.location + range.length >= range.location);
    assert(range.location + range.length <= buffer->_count);

    size_t elementSize = buffer->_elementSize;
    CCIndex bufferElementCapacity = buffer->_bufferElementCapacity;
    
    CCRange remain = range;
    
    if (buffer->_capacity > buffer->_bufferElementCapacity) {
        CCPageTablePtr table = (CCPageTablePtr)(buffer->_storage);

        CCIndex pageIndex = 0;
        
        do {
            CCIndex location = __CCCircularBufferIndexToLoaction(buffer, remain.location);
            CCCircularBufferLocationPath path = __CCCircularBufferGetLocationPath(buffer, location);
            pageIndex = path.pageIndex;

            uint8_t * storage = __CCCircularBufferGetPage(buffer, pageIndex);
            CCIndex rLength = bufferElementCapacity - path.index;
            if (rLength > remain.length) {
                rLength = remain.length;
            }
            assert(rLength > 0);
            func(context, CCRangeMake(remain.location, rLength), elementSize, storage + buffer->_indexOffset * elementSize);
            remain.location += rLength;
            remain.length -= rLength;
            pageIndex += 1;
            if (pageIndex >= table->_count) {
                pageIndex = 0;
            }
        } while (0);
        while (remain.length > 0) {
            uint8_t * storage = __CCCircularBufferGetPage(buffer, pageIndex);
            CCIndex rLength = bufferElementCapacity;
            if (rLength > remain.length) {
                rLength = remain.length;
            }
            assert(rLength > 0);
            func(context, CCRangeMake(remain.location, rLength), elementSize, storage + buffer->_indexOffset * elementSize);
            remain.location += rLength;
            remain.length -= rLength;
            pageIndex += 1;
            if (pageIndex >= table->_count) {
                pageIndex = 0;
            }
        }
    } else {
        CCIndex location = __CCCircularBufferIndexToLoaction(buffer, remain.location);
        CCCircularBufferLocationPath path = __CCCircularBufferGetLocationPath(buffer, location);
        uint8_t * storage = __CCCircularBufferGetPage(buffer, path.pageIndex);
        CCIndex rLength = buffer->_capacity - location;
        if (rLength > range.length) {
            rLength = range.length;
        }
        assert(rLength > 0);
        func(context, CCRangeMake(remain.location, rLength), elementSize, storage + location * elementSize);
        remain.location += rLength;
        remain.length -= rLength;
        if (rLength < range.length) {
            func(context, remain, elementSize, storage);
        }
    }
}

void CCCircularBufferEnumerateCopyToBuffer(void * _Nullable context, CCRange range, size_t elementSize, const void * _Nonnull values);

static inline void __CCCircularBufferResize(CCCircularBuffer_s * _Nonnull buffer, CCIndex capacity) {
    assert(buffer);
    size_t elementSize = buffer->_elementSize;
    CCIndex bufferElementCapacity = buffer->_bufferElementCapacity;
    if (capacity < buffer->_count) {
        capacity = buffer->_count;
        CCLogError("__CCCircularBufferResize error capacity < buffer->_count");
    }

    if (capacity == 0) {
        if (capacity == buffer->_capacity) {
            return;
        }
        if (buffer->_capacity == 0) {
            __CCCircularBufferResetStorage(buffer, NULL, capacity, 0);
        } else if (buffer->_capacity <= bufferElementCapacity) {
            void * old = buffer->_storage;
            __CCCircularBufferResetStorage(buffer, NULL, capacity, 0);
            CCDeallocate(old);
        } else {
            CCCircularBufferPageTable_s * table = (CCCircularBufferPageTable_s *)(buffer->_storage);
            __CCCircularBufferResetStorage(buffer, NULL, capacity, 0);
            for (CCIndex i=0; i<table->_count; i++) {
                CCDeallocate(__CCCircularBufferPageTableGetPage(table, i));
            }
            __CCCircularBufferPageTableDealloc(table);
        }
    } else if (capacity <= bufferElementCapacity) {
        size_t bufferSize = capacity * elementSize;
        int itr = 0;
        for (itr = 0; itr<__CCCircularBufferSizeCount; itr++) {
            if (bufferSize <= __CCCircularBufferSizes[itr]) {
                bufferSize = __CCCircularBufferSizes[itr];
                capacity = __CCCircularBufferSizes[itr] / buffer->_elementSize;
                break;
            }
        }
        if (capacity == buffer->_capacity) {
            return;
        }
        uint8_t * ptr = CCAllocate(bufferSize);
        __CCCircularBufferEnumerate(buffer, CCRangeMake(0, buffer->_count), ptr, CCCircularBufferEnumerateCopyToBuffer);
        
        //替换旧数据
        if (buffer->_capacity == 0) {
            __CCCircularBufferResetStorage(buffer, ptr, capacity, 0);
        } else if (buffer->_capacity <= bufferElementCapacity) {
            void * old = buffer->_storage;
            __CCCircularBufferResetStorage(buffer, ptr, capacity, 0);
            CCDeallocate(old);
        } else {
            CCCircularBufferPageTable_s * table = (CCCircularBufferPageTable_s *)(buffer->_storage);
            __CCCircularBufferResetStorage(buffer, ptr, capacity, 0);

            for (CCIndex i=0; i<table->_count; i++) {
                CCDeallocate(__CCCircularBufferPageTableGetPage(table, i));
            }
            __CCCircularBufferPageTableDealloc(table);
        }
    } else {
        CCIndex pageCount = (capacity + bufferElementCapacity - 1) / bufferElementCapacity;
        capacity = pageCount * bufferElementCapacity;
        
        if (capacity == buffer->_capacity) {
            return;
        }
        CCIndex pageCapacity = (pageCount + CCPageMaxCountPerGroup - 1) / CCPageMaxCountPerGroup;
        if (buffer->_capacity == 0) {
            CCCircularBufferPageTable_s * table = __CCCircularBufferPageTableAlloc(pageCapacity);
            for (CCIndex i=0; i<pageCapacity; i++) {
                uint8_t * page = CCAllocate(CCBufferMaxSize);
                __CCCircularBufferPageTableAppendPage(table, page);
            }
            __CCCircularBufferResetStorage(buffer, table, capacity, 0);
        } else if (buffer->_capacity <= buffer->_bufferElementCapacity) {
            CCCircularBufferPageTable_s * table = __CCCircularBufferPageTableAlloc(pageCapacity);
            void * old = NULL;
            if (buffer->_capacity == buffer->_bufferElementCapacity && buffer->_indexOffset == 0) {
                __CCCircularBufferPageTableAppendPage(table, buffer->_storage);
            } else {
                old = buffer->_storage;
                uint8_t * ptr = CCAllocate(CCBufferMaxSize);
                __CCCircularBufferEnumerate(buffer, CCRangeMake(0, buffer->_count), ptr, CCCircularBufferEnumerateCopyToBuffer);
                __CCCircularBufferPageTableAppendPage(table, ptr);
            }
            for (CCIndex i=1; i<pageCapacity; i++) {
                uint8_t * page = CCAllocate(CCBufferMaxSize);
                __CCCircularBufferPageTableAppendPage(table, page);
            }
            __CCCircularBufferResetStorage(buffer, table, capacity, 0);
            if (old) {
                CCDeallocate(old);
            }
        } else {
            //去掉多余的
            CCCircularBufferPageTable_s * table = (CCCircularBufferPageTable_s *)(buffer->_storage);
            __CCCircularBufferPageTableResize(table, pageCapacity);
            if (oldTable->_capacity <= CCPageMaxCountPerGroup) {
                //2维
                if (pageCapacity <= CCPageMaxCountPerGroup) {
                    //2维

                } else {
                    //3维
                    //扩容
                    
                }
            } else {
                //3维
                if (pageCapacity <= CCPageMaxCountPerGroup) {
                    //2维
                    //缩容

                } else {
                    //3维
                    
                    
                }
            }
        }
    }
}




//resize == 0 时， 不进行resize
// This function does no ObjC dispatch or argument checking;
// It should only be called from places where that dispatch and check has already been done, or NSCCArray
static inline void __CCCircularBufferReplace(CCCircularBuffer_s * _Nonnull buffer,
                                             _Bool autoResize,
                                             const CCRangeReplace * _Nonnull items, CCIndex itemsCount,
                                             void * _Nullable removeContext, CCCircularBufferUpdateCallBack_f _Nullable onRemove,
                                             void * _Nullable insertContext, CCCircularBufferUpdateCallBack_f _Nullable onInsert) {
    assert(buffer);
    size_t elementSize = buffer->_elementSize;
    CCIndex bufferElementCapacity = buffer->_bufferElementCapacity;

    if (itemsCount > 0) {
        assert(items);
    } else {
        if (autoResize) {
            //尝试缩容
            if (buffer->_count > bufferElementCapacity) {

            } else {
                size_t size = buffer->_count * elementSize;
                int itr = 0;
                CCIndex capacity = 0;
                for (itr = 0; itr<__CCCircularBufferSizeCount; itr++) {
                    if (size <= __CCCircularBufferSizes[itr]) {
                        size = __CCCircularBufferSizes[itr];
                        capacity = __CCCircularBufferSizes[itr] / buffer->_elementSize;
                        break;
                    }
                }
                if (buffer->_capacity < buffer->_bufferElementCapacity) {
                    if (capacity != 0) {
                        uint8_t * old = buffer->_storage;
                        assert(old);
                        uint8_t * ptr = CCAllocate(size);
                        if (buffer->_capacity <= bufferElementCapacity) {
                            CCIndex rLength = buffer->_capacity - buffer->_indexOffset;
                            if (rLength > buffer->_count) {
                                rLength = buffer->_count;
                            }
                            size_t rSize = elementSize * rLength;
                            memcpy(ptr, old + buffer->_indexOffset * elementSize, rSize);
                            if (rLength < buffer->_count) {
                                memcpy(ptr + rSize, old, elementSize * (buffer->_count - rLength));
                            }
                            __CCCircularBufferResetStorage(buffer, ptr, capacity, 0);
                            CCDeallocate(old);
                        } else {
                            
                            
                        }
                    }
                    
                    
                } else {
                    
                    
                }
                void * old = buffer->_storage;
                assert(old);
                uint8_t * ptr = CCAllocate(size);

   
                assert(itr != __CCCircularBufferSizeCount);
                if (capacity != buffer->_capacity) {
                    assert(capacity < buffer->_capacity);
                    
                    if (buffer->_capacity < buffer->_bufferElementCapacity) {
                        if (capacity != 0) {
                            uint8_t * old = buffer->_storage;
                            assert(old);
                            uint8_t * ptr = CCAllocate(size);
                            if (buffer->_capacity <= bufferElementCapacity) {
                                CCIndex rLength = buffer->_capacity - buffer->_indexOffset;
                                if (rLength > buffer->_count) {
                                    rLength = buffer->_count;
                                }
                                size_t rSize = elementSize * rLength;
                                memcpy(ptr, old + buffer->_indexOffset * elementSize, rSize);
                                if (rLength < buffer->_count) {
                                    memcpy(ptr + rSize, old, elementSize * (buffer->_count - rLength));
                                }
                                __CCCircularBufferResetStorage(buffer, ptr, capacity, 0);
                                CCDeallocate(old);
                            } else {
                                
                                
                            }
                        }
                        
                        
                    } else {
                        
                        
                    }
                    
                    //缩容
                    if (capacity == 0) {
                        //释放全部空间
                        
                    } else {
                        if (buffer->_capacity < buffer->_bufferElementCapacity) {
                            
                            
                            
                            
                        } else {
                            
                            
                        }
                        uint8_t * ptr = CCAllocate(size);
                        CCIndex idx = __CCCircularBufferLoactionToIndex(buffer, 0);
                        
                        memcpy(ptr, <#const void *__src#>, <#size_t __n#>)
                        
                        
                        
                        
                        
                    }
                }
            }
        }
        return;
    }
    
    CCIndex offset = 0;
    CCRange range = {};//最大的不动的区间
    CCIndex removedCount = 0;
    CCIndex insertedCount = 0;
    
    for (CCIndex i=0; i<itemsCount; i++) {
        CCRangeReplace item = items[i];
        assert(item.range.location >= offset);
        CCIndex length = range.location - offset;
        if (length > range.length) {
            range.location = offset;
            range.length = length;
        }
        offset = item.range.location + item.range.length;
        if (item.newLength > 0) {
            assert(item.newValues);
        }
        removedCount += item.range.length;
        insertedCount += item.newLength;
    }
    assert(offset <= buffer->_count);
    
    CCIndex newCount = buffer->_count - removedCount + insertedCount;
    assert(newCount <= buffer->_capacity && !resize);
    do {
        CCRangeReplace item = items[itemsCount-1];
        CCIndex end = item.range.location + item.range.length;
        if (end < buffer->_count) {
            CCIndex length = buffer->_count - end;
            if (length > range.length) {
                range.location = end;
                range.length = length;
            }
        }
    } while (false);
    
    if (buffer->_count == 0) {
        buffer->_indexOffset = 0;
    }
//
//    //        CCIndex loc = __CCCircularBufferIndexToLoaction(<#CCCircularBuffer_s * _Nonnull buffer#>, <#CCIndex index#>)
//
//    else if (newCount == 0) {
//        //remove all
//
//
//    } else {
//
//    }
//
    if (onRemove) {
        for (CCIndex i=0; i<itemsCount; i++) {
            CCRangeReplace item = items[i];
            
            
            
        }
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
