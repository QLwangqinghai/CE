//
//  CCPageTable.h
//  CCollection
//
//  Created by vector on 2019/11/20.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCPageTable_h
#define CCPageTable_h

#include "CInteger.h"
#include "CType.h"
#include "CCBase.h"
#include "CCAtomic.h"

typedef struct {
    CCIndex _mutable: 1;
    CCIndex _capacityMutable: 1;
    CCIndex __: 24;
    CCIndex _sectionIndexShift: 6;
    
    CCIndex _indexInSectionMask: 32;
    CCIndex _pageCountPerSection: 32;
    
    CCIndex _sectionOffset: 32;
    CCIndex _sectionCapacity: 32;
    
    CCIndex _offset;
    CCIndex _capacity;
    CCIndex _count;

    void * _Nonnull _content;
} CCPageTable_s;

typedef CCPageTable_s * CCPageTablePtr;
typedef uint8_t * CCPagePtr;

static inline CCIndex __CCPageTableGetSectionCount(CCPageTablePtr _Nonnull table) {
    if (table->_capacity == 0) {
        return 0;
    } else if (table->_capacity <= table->_pageCountPerSection) {
        //1维
        return 1;
    } else {
        //2维
        return table->_capacity >> table->_sectionIndexShift;
    }
}

static inline CCIndex __CCPageTableSectionIndexToLoaction(CCPageTablePtr _Nonnull table, CCIndex index) {
    CCIndex r = table->_sectionCapacity - table->_sectionOffset;
    if (index >= r) {
        return index - r;
    } else {
        return index + table->_sectionOffset;
    }
}

static inline CCIndex __CCPageTableSectionLoactionToIndex(CCPageTablePtr _Nonnull table, CCIndex location) {
    if (location >= table->_sectionOffset) {
        return location - table->_sectionOffset;
    } else {
        return table->_sectionCapacity - (table->_sectionOffset - location);
    }
}

static inline CCIndex __CCPageTableIndexToLoaction(CCPageTablePtr _Nonnull table, CCIndex index) {
    CCIndex r = table->_capacity - table->_offset;
    if (index >= r) {
        return index - r;
    } else {
        return index + table->_offset;
    }
}

static inline CCIndex __CCPageTableLoactionToIndex(CCPageTablePtr _Nonnull table, CCIndex location) {
    if (location >= table->_offset) {
        return location - table->_offset;
    } else {
        return table->_capacity - (table->_offset - location);
    }
}

static inline CCPagePtr _Nullable * _Nonnull __CCPageTableGetSection(CCPageTablePtr _Nonnull table, CCIndex sectionIndex) {
    CCIndex sectionCount = __CCPageTableGetSectionCount(table);
    assert(sectionIndex < sectionCount);
    if (sectionCount <= 1) {
        //1维
        CCPagePtr * storage = table->_content;
        assert(storage);
        return storage;
    } else {
        CCIndex location = __CCPageTableSectionIndexToLoaction(table, sectionIndex);
        //2维
        CCPagePtr * * storage = table->_content;
        CCPagePtr * result = storage[location];
        assert(result);
        return result;
    }
}
static inline CCPagePtr _Nonnull __CCPageTableGetPage(CCPageTablePtr _Nonnull table, CCIndex pageIndex) {
    assert(pageIndex < table->_count);
    CCIndex location = __CCPageTableIndexToLoaction(table, pageIndex);
    return __CCPageTableGetSection(table, location >> CCPageMaxCountPerGroupShift)[location & CCPageIndexInGroupMask];
}
static inline void __CCPageTableAppendPage(CCPageTablePtr _Nonnull table, CCPagePtr _Nullable page) {

}
static inline void __CCPageTablePrependPage(CCPageTablePtr _Nonnull table, CCPagePtr _Nullable page) {

    
}
static inline CCPagePtr _Nullable __CCPageTableRemoveFirst(CCPageTablePtr _Nonnull table) {

    return NULL;
}
static inline CCPagePtr _Nullable __CCPageTableRemoveLast(CCPageTablePtr _Nonnull table) {

    return NULL;
}

static inline void __CCPageTableDeallocate(CCPageTablePtr _Nonnull table) {
    if (table->_capacity <= CCPageMaxCountPerGroup) {
        //1维
        CCDeallocate(table->_content);
        CCDeallocate(table);
    } else {
        //2维
        CCPagePtr * * storage = table->_content;
        CCIndex max = table->_capacity / CCPageMaxCountPerGroup;
        for (CCIndex si=0; si<max; si++) {
            CCDeallocate(storage[si]);
        }
        CCDeallocate(table->_content);
        CCDeallocate(table);
    }
}

static inline CCIndex __CCPageTableAlignCapacity(CCIndex capacity) {
    if (capacity == 0) {
        return capacity;
    } else if (capacity <= CCPageMaxCountPerGroup) {
        //2维
        CCIndex result = CCPowerAlign2(capacity);
        assert(result >= capacity);
        return result;
    } else {
        //3维
        //扩容
        CCIndex result = (capacity + CCPageMaxCountPerGroup - 1) / CCPageMaxCountPerGroup * CCPageMaxCountPerGroup;
        assert(result >= capacity);
        return result;
    }
}

//capacity must >= 2 && table->_sectionCapacity >= 2
static inline void __CCPageTableResizeSection(CCPageTablePtr _Nonnull table, CCIndex sectionCapacity) {
    assert(table);
    assert(table->_sectionCapacity >= 2);
    assert(sectionCapacity >= 2);
    sectionCapacity = CCPowerAlign2(sectionCapacity);
    if (table->_sectionCapacity == sectionCapacity) {
        return;
    }
    CCIndex sectionCount = __CCPageTableGetSectionCount(table);
    assert(sectionCount <= sectionCapacity);

    CCPagePtr * content = CCAllocate(sizeof(void *) * sectionCount);
    CCPagePtr * old = table->_content;
    CCIndex iter = 0;
    for (CCIndex idx=table->_sectionOffset; idx<table->_sectionCapacity && iter<sectionCount; idx++, iter++) {
        content[iter] = old[idx];
    }
    for (CCIndex idx=0; idx<table->_sectionOffset && iter<sectionCount; idx++, iter++) {
        content[iter] = old[idx];
    }
    table->_content = content;
    table->_sectionOffset = 0;
    table->_sectionCapacity = sectionCapacity;
}


static inline void __CCPageTableResize(CCPageTablePtr _Nonnull table, CCIndex capacity) {
    assert(table);
    capacity = __CCPageTableAlignCapacity(capacity);
    if (table->_capacity == capacity) {
        return;
    }
    assert(table->_count <= capacity);
    
    CCIndex sectionCount = CCPowerAlign2(capacity / CCPageMaxCountPerGroup);
    if (sectionCount == 1) {
        sectionCount = 0;
    }
    
    //去掉多余的
    if (table->_capacity == 0) {
        if (capacity == 0) {
        } else if (capacity <= CCPageMaxCountPerGroup) {
            //1维
            table->_content = CCAllocate(sizeof(void *) * capacity);
            table->_capacity = capacity;
            table->_sectionCapacity = sectionCount;
            table->_offset = 0;
        } else {
            //2维
            //扩容
            CCPagePtr * content = CCAllocate(sizeof(void *) * sectionCount);
            for (CCIndex idx=0; idx<sectionCount; idx++) {
                content[idx] = CCAllocate(sizeof(void *) * CCPageMaxCountPerGroup);
            }
            table->_content = content;
            table->_capacity = capacity;
            table->_sectionCapacity = sectionCount;
            table->_offset = 0;
        }
    } else if (table->_capacity <= CCPageMaxCountPerGroup) {
        //1维
        if (capacity == 0) {
            //缩容
            CCDeallocate(table->_content);
            table->_content = NULL;
            table->_capacity = capacity;
            table->_sectionCapacity = sectionCount;
            table->_offset = 0;
        } else if (capacity <= CCPageMaxCountPerGroup) {
            //1维
            CCPagePtr content = CCAllocate(sizeof(void *) * capacity);
            CCRange range = CCRangeMake(0, table->_count);
            while (range.length > 0) {
                
            }
        } else {
            //2维
            //扩容
            
        }
    } else {
        //2维
        if (capacity == 0) {
            //缩容

            
        } else if (capacity <= CCPageMaxCountPerGroup) {
            //1维
            //缩容

        } else {
            //2维
            
            
        }
    }
}

/*
 typedef struct {
     CCIndex _mutable: 1;
     CCIndex _capacityMutable: 1;
     CCIndex __: 24;
     CCIndex _sectionIndexShift: 6;
     CCIndex _pageCountPerSection: 32;
     CCIndex _sectionOffset: 32;
     CCIndex _sectionCapacity: 32;
     
     CCIndex _offset;
     CCIndex _capacity;
     CCIndex _count;

     void * _Nonnull _content;
 } CCPageTable_s;

 */

static inline CCPageTablePtr _Nonnull __CCPageTableAllocate(CCIndex capacity) {
    CCPageTablePtr table = CCAllocate(sizeof(CCPageTable_s));
    table->_capacity = capacity;
    table->_count = 0;
    table->_offset = 0;
    table->_sectionCapacity = 0;
    table->_content = NULL;
    __CCPageTableResize(table, capacity);
//    if (capacity <= CCPageMaxCountPerGroup) {
//        //2维
//        table->_content = CCAllocate(sizeof(void *) * capacity);
//    } else {
//        assert(capacity % CCPageMaxCountPerGroup == 0);
//        //3维
//        CCIndex sctionCount = table->_capacity / CCPageMaxCountPerGroup;
//        uint8_t * * * storage = CCAllocate(sizeof(void *) * sctionCount);
//        for (CCIndex si=0; si<sctionCount; si++) {
//            storage[si] = CCAllocate(sizeof(void *) * CCPageMaxCountPerGroup);
//        }
//        table->_content = storage;
//    }
    return table;
}


typedef void (*CCCircularBufferEnumerateCallBack_f)(void * _Nullable context, CCRange range, size_t elementSize, const void * _Nonnull values);

static inline void __CCPageTableEnumerate(CCPageTablePtr _Nonnull table, CCRange range, void * _Nullable context, CCCircularBufferEnumerateCallBack_f _Nonnull func) {
    assert(table);
    assert(func);
    if (range.length == 0) {
        return;
    }
    assert(range.location + range.length >= range.location);
    assert(range.location + range.length <= table->_count);
    CCRange remain = range;
    
    if (table->_sectionCapacity > 0) {
        CCIndex pageIndex = 0;
        do {
            CCIndex location = __CCPageTableLoactionToIndex(table, remain.location);
            CCPagePtr * storage = __CCPageTableGetPage(table, pageIndex);
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
















#endif /* CCPageTable_h */
