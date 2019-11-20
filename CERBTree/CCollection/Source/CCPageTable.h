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

typedef uint8_t * CCPagePtr;



typedef struct {
    void * _Nonnull _section;
} CCPageSection_s;

typedef struct {
    CCIndex offset: 32;
    CCIndex capacity: 32;
//    CCPagePtr _Nonnull * _Nonnull content;
    void * _Nonnull content;
} CCPageSectionBuffer_s;

typedef struct {
    CCPagePtr _Nullable section;
    CCPageSectionBuffer_s sections;
} CCPageTableContent_u;

typedef struct {
    CCIndex _mutable: 1;
    CCIndex _capacityMutable: 1;
    CCIndex __: 24;
    CCIndex _sectionIndexShift: 6;
    
    CCIndex _indexInSectionMask: 32;
    CCIndex _pageCountPerSection: 32;
    
    CCIndex _offset;
    CCIndex _capacity;
    CCIndex _count;

    CCPageSectionBuffer_s sections;
} CCPageTable_s;







typedef CCPageTable_s * CCPageTablePtr;

static inline CCIndex __CCPageTableGetSectionCount(CCPageTablePtr _Nonnull table) {
    if (table->_capacity == 0) {
        return 0;
    } else if (table->_capacity <= table->_pageCountPerSection) {
        return 1;
    } else {
        return table->_capacity >> table->_sectionIndexShift;
    }
}

static inline CCIndex __CCPageTableSectionIndexToLoaction(CCPageTablePtr _Nonnull table, CCIndex index) {
    CCIndex r = table->sections.capacity - table->sections.offset;
    if (index >= r) {
        return index - r;
    } else {
        return index + table->sections.capacity;
    }
}

static inline CCIndex __CCPageTableSectionLoactionToIndex(CCPageTablePtr _Nonnull table, CCIndex location) {
    if (location >= table->sections.offset) {
        return location - table->sections.offset;
    } else {
        return table->sections.capacity - (table->sections.offset - location);
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
        CCPagePtr * storage = table->sections.content;
        assert(storage);
        return storage;
    } else {
        CCIndex location = __CCPageTableSectionIndexToLoaction(table, sectionIndex);
        //2维
        CCPagePtr * * storage = table->sections.content;
        CCPagePtr * result = storage[location];
        assert(result);
        return result;
    }
}
static inline CCPagePtr _Nonnull __CCPageTableGetPage(CCPageTablePtr _Nonnull table, CCIndex pageIndex) {
    assert(pageIndex < table->_count);
    CCIndex location = __CCPageTableIndexToLoaction(table, pageIndex);
    CCPagePtr * section = __CCPageTableGetSection(table, location >> CCPageMaxCountPerGroupShift);
    CCPagePtr page = section[location & CCPageIndexInGroupMask];
    assert(page);
    return page;
}

static inline CCPagePtr _Nonnull * _Nonnull __CCPageTableGetPages(CCPageTablePtr _Nonnull table, CCIndex beginIndex, CCIndex * _Nonnull lengthPtr) {
    assert(beginIndex < table->_count);
    assert(table);
    assert(lengthPtr);
    CCIndex rLength = table->_count - beginIndex;
    CCIndex location = __CCPageTableIndexToLoaction(table, beginIndex);
    
    CCPagePtr * section = __CCPageTableGetSection(table, location >> CCPageMaxCountPerGroupShift);
    CCIndex offset = location & CCPageIndexInGroupMask;
    section += offset;
    *lengthPtr = CCPageMaxCountPerGroup - offset < rLength ? CCPageMaxCountPerGroup - offset : rLength;
    return section;
}

//static inline void __CCPageTableAppendPage(CCPageTablePtr _Nonnull table, CCPagePtr _Nullable page) {
//
//}
//static inline void __CCPageTablePrependPage(CCPageTablePtr _Nonnull table, CCPagePtr _Nullable page) {
//
//
//}
//static inline CCPagePtr _Nullable __CCPageTableRemoveFirst(CCPageTablePtr _Nonnull table) {
//
//    return NULL;
//}
//static inline CCPagePtr _Nullable __CCPageTableRemoveLast(CCPageTablePtr _Nonnull table) {
//
//    return NULL;
//}


static inline CCIndex __CCPageTableAlignCapacity(CCIndex capacity) {
    if (capacity == 0) {
        return capacity;
    } else if (capacity <= CCPageMaxCountPerGroup) {
        //1维
        CCIndex result = CCPowerAlign2(capacity);
        assert(result >= capacity);
        return result;
    } else {
        //2维
        //扩容
        CCIndex result = (capacity + CCPageMaxCountPerGroup - 1) / CCPageMaxCountPerGroup * CCPageMaxCountPerGroup;
        assert(result >= capacity);
        return result;
    }
}

/*
 remove empty section
 
 insert empty section
 */

static inline void __CCPageTableResize(CCPageTablePtr _Nonnull table, CCIndex capacity) {
    assert(table);
    capacity = __CCPageTableAlignCapacity(capacity);
    if (table->_capacity == capacity) {
        return;
    }
    assert(table->_count <= capacity);
    
    /*
     0->0
     0->1
     0->2
     1->0
     1->1
     1->2
     2->0
     2->1
     2->2
     */
    //去掉多余的
    
    
    
    
    
    if (table->_capacity == 0) {
        if (capacity == 0) {
            //0->0
            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = NULL;
            
            table->_capacity = capacity;
            table->_offset = 0;
        } else if (capacity <= CCPageMaxCountPerGroup) {
            //0->1
            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = CCAllocate(sizeof(CCPagePtr) * capacity);
            
            table->_capacity = capacity;
            table->_offset = 0;
        } else {
            //0-2
            CCIndex sectionCount = capacity / CCPageMaxCountPerGroup;
            CCIndex sectionCapacity = CCPowerAlign2(sectionCount);
            CCPagePtr * * content = CCAllocate(sizeof(CCPagePtr *) * sectionCapacity);
            for (CCIndex idx=0; idx<sectionCount; idx++) {
                content[idx] = CCAllocate(sizeof(CCPagePtr) * CCPageMaxCountPerGroup);
            }
            
            table->sections.capacity = sectionCapacity;
            table->sections.offset = 0;
            table->sections.content = content;
            
            table->_capacity = capacity;
            table->_offset = 0;
        }
    } else if (table->_capacity <= CCPageMaxCountPerGroup) {
        //1->0
        if (capacity == 0) {
            //缩容
            CCPagePtr * old = table->sections.content;

            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = NULL;

            table->_capacity = capacity;
            table->_offset = 0;
            
            CCDeallocate(old);
        } else if (capacity <= CCPageMaxCountPerGroup) {
            //1->1
            CCPagePtr * content = CCAllocate(sizeof(CCPagePtr) * capacity);
            CCPagePtr * old = table->sections.content;
            CCIndex iter = 0;
            for (CCIndex idx=table->_offset; idx<table->_capacity && iter<table->_count; idx++, iter++) {
                content[iter] = old[idx];
            }
            for (CCIndex idx=0; idx<table->_offset && iter<table->_count; idx++, iter++) {
                content[iter] = old[idx];
            }
            
            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = content;
            table->_capacity = capacity;
            table->_offset = 0;
            
            CCDeallocate(old);
        } else {
            //1->2
            
            CCIndex sectionCount = capacity / CCPageMaxCountPerGroup;
            CCIndex sectionCapacity = CCPowerAlign2(sectionCount);
            CCPagePtr * * content = CCAllocate(sizeof(CCPagePtr *) * sectionCapacity);
            for (CCIndex idx=0; idx<sectionCount; idx++) {
                content[idx] = CCAllocate(sizeof(CCPagePtr) * CCPageMaxCountPerGroup);
            }
            
            CCPagePtr * target = content[0];
            CCPagePtr * old = table->sections.content;
            CCIndex iter = 0;
            for (CCIndex idx=table->_offset; idx<table->_capacity && iter<table->_count; idx++, iter++) {
                target[iter] = old[idx];
            }
            for (CCIndex idx=0; idx<table->_offset && iter<table->_count; idx++, iter++) {
                target[iter] = old[idx];
            }
            
            table->sections.capacity = sectionCapacity;
            table->sections.offset = 0;
            table->sections.content = content;
            
            table->_capacity = capacity;
            table->_offset = 0;
            
            CCDeallocate(old);
        }
    } else {
        //2维
        CCIndex oldSectionCount = __CCPageTableGetSectionCount(table);
        CCPageSectionBuffer_s sections = table->sections;

        if (capacity == 0) {
            //2->0
            CCPagePtr **  old = sections.content;

            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = NULL;

            table->_capacity = capacity;
            table->_offset = 0;
            
            CCIndex iter = 0;
            for (CCIndex idx=sections.offset; idx<sections.capacity && iter<oldSectionCount; idx++, iter++) {
                CCDeallocate(old[idx]);
            }
            for (CCIndex idx=0; idx<sections.offset && iter<oldSectionCount; idx++, iter++) {
                CCDeallocate(old[idx]);
            }
            CCDeallocate(old);
        } else if (capacity <= CCPageMaxCountPerGroup) {
            //2->1
            CCPagePtr **  old = sections.content;
            
            CCPagePtr * content = CCAllocate(sizeof(CCPagePtr) * capacity);

            CCIndex idx = 0;
            CCIndex length = 0;
            if (1) {
                CCPagePtr * pages = __CCPageTableGetPages(table, idx, &length);
                memcpy(content, pages, sizeof(CCPagePtr) * length);
                idx += length;
            }
            
            if (idx < table->_count) {
                CCPagePtr * pages = __CCPageTableGetPages(table, idx, &length);
                memcpy(content + idx, pages, sizeof(CCPagePtr) * length);
                idx += length;
            }
            assert(idx == table->_count);

            
            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = content;
            table->_capacity = capacity;
            table->_offset = 0;
            
            CCIndex iter = 0;
            for (CCIndex idx=sections.offset; idx<sections.capacity && iter<oldSectionCount; idx++, iter++) {
                CCDeallocate(old[idx]);
            }
            for (CCIndex idx=0; idx<sections.offset && iter<oldSectionCount; idx++, iter++) {
                CCDeallocate(old[idx]);
            }
            CCDeallocate(old);
        } else {
            //2->2
            
            
        }
    }
}


static inline void __CCPageTableDeallocate(CCPageTablePtr _Nonnull table) {
    __CCPageTableResize(table, 0);
    CCDeallocate(table);
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
    
    table->sections.capacity = 0;
    table->sections.offset = 0;
    table->sections.content = NULL;

    __CCPageTableResize(table, capacity);
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
