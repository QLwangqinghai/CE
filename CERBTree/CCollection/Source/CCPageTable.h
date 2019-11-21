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


#pragma mark - Base


#pragma mark - CCPageSectionCollection

typedef struct {
    CCPagePtr _Nonnull * _Nonnull content[3];
} CCPageSection_s;

typedef struct {
    CCIndex count;
    CCPagePtr _Nonnull * _Nonnull content[2];
} CCPageSectionCollection0_s;

typedef struct {
    CCIndex count;
    CCIndex offset;
    CCPagePtr _Nonnull * _Nonnull * _Nonnull content;
} CCPageSectionCollection_s;

typedef struct {
    CCIndex count;
    uintptr_t _load0;
    uintptr_t _load1;
} CCPageSectionCollection2_s;

typedef struct {
    CCPageSectionCollection0_s sections0;
    CCPageSectionCollection_s sections;
} CCPageSectionCollection_u;



typedef CCPageSectionCollection_s * CCPageSectionCollectionPtr;

typedef struct {
    CCPageSectionCollection0_s sections0;
    CCPageSectionCollection_s sections;
} CCPageTableContent_u;


static inline CCPagePtr _Nullable * _Nonnull __CCPageSectionCollectionRemoveEmptySection(CCPageSectionCollectionPtr _Nonnull sections) {
//    assert(table->sections.capacity >= 2);
    
    
    return NULL;
}
static inline void __CCPageSectionCollectionInsertEmptySection(CCPageSectionCollectionPtr _Nonnull sections, CCPagePtr _Nullable * _Nonnull page) {


}

//static inline CCIndex __CCPageSectionCollectionIndexToLoaction(CCPageSectionCollectionPtr _Nonnull sections, CCIndex index) {
//    CCIndex r = sections->capacity - sections->offset;
//    if (index >= r) {
//        return index - r;
//    } else {
//        return index + sections->capacity;
//    }
//}
//static inline CCIndex __CCPageSectionCollectionLoactionToIndex(CCPageSectionCollectionPtr _Nonnull sections, CCIndex location) {
//    if (location >= sections->offset) {
//        return location - sections->offset;
//    } else {
//        return sections->capacity - (sections->offset - location);
//    }
//}


#pragma mark - CCPageTable

typedef struct {
    CCIndex _mutable: 1;
    CCIndex capacityMutable: 1;
    CCIndex __: 24;
    CCIndex _sectionIndexShift: 6;
    
    CCIndex _indexInSectionMask: 32;
    CCIndex pageCountPerSection: 32;
    
    CCIndex offset;
    CCIndex count;
    CCIndex capacity;

    CCPageTableContent_u content;
//    CCPageSectionCollection_s sections;
} CCPageTable_s;



typedef CCPageTable_s * CCPageTablePtr;

static inline CCIndex __CCPageTableGetSectionCount(CCPageTablePtr _Nonnull table) {
    if (table->capacity == 0) {
        return 0;
    } else if (table->capacity <= 3 << table->_sectionIndexShift) {
        return table->capacity >> table->_sectionIndexShift;
    } else {
        return table->content.sections.count;
    }
}

static inline CCIndex __CCPageTableGetCapacity(CCPageTablePtr _Nonnull table) {
    return table->capacity;
}

static inline CCIndex __CCPageTableIndexToLoaction(CCPageTablePtr _Nonnull table, CCIndex index) {
    CCIndex capacity = __CCPageTableGetCapacity(table);
    CCIndex r = capacity - table->offset;
    if (index >= r) {
        return index - r;
    } else {
        return index + table->offset;
    }
}

static inline CCIndex __CCPageTableLoactionToIndex(CCPageTablePtr _Nonnull table, CCIndex location) {
    CCIndex capacity = __CCPageTableGetCapacity(table);
    if (location >= table->offset) {
        return location - table->offset;
    } else {
        return capacity - (table->offset - location);
    }
}

static inline CCPagePtr _Nullable * _Nonnull __CCPageTableGetSection(CCPageTablePtr _Nonnull table, CCIndex sectionIndex) {
    CCIndex sectionCount = __CCPageTableGetSectionCount(table);
    assert(sectionIndex < sectionCount);
    if (0 == table->isSections) {
        CCPageSection_s section = table->content.section;
        assert(section.content);
        return section.content;
    } else {
        CCPageSectionCollectionPtr sections = &(table->content.sections);
        CCIndex location = __CCPageSectionCollectionIndexToLoaction(sections, sectionIndex);
        //2维
        CCPagePtr * * storage = sections->content;
        CCPagePtr * result = storage[location];
        assert(result);
        return result;
    }
}
static inline CCPagePtr _Nonnull __CCPageTableGetPage(CCPageTablePtr _Nonnull table, CCIndex pageIndex) {
    assert(pageIndex < table->count);
    CCIndex location = __CCPageTableIndexToLoaction(table, pageIndex);
    CCPagePtr * section = __CCPageTableGetSection(table, location >> table->_sectionIndexShift);
    CCPagePtr page = section[location & table->_indexInSectionMask];
    assert(page);
    return page;
}

static inline CCPagePtr _Nonnull * _Nonnull __CCPageTableGetPages(CCPageTablePtr _Nonnull table, CCIndex beginIndex, CCIndex * _Nonnull lengthPtr) {
    assert(beginIndex < table->count);
    assert(table);
    assert(lengthPtr);
    CCIndex rLength = table->count - beginIndex;
    CCIndex location = __CCPageTableIndexToLoaction(table, beginIndex);
    
    CCPagePtr * section = __CCPageTableGetSection(table, location >> table->_sectionIndexShift);
    CCIndex offset = location & table->_indexInSectionMask;
    section += offset;
#warning "out bounds sectionCount == 1"
    *lengthPtr = table->pageCountPerSection - offset < rLength ? table->pageCountPerSection - offset : rLength;
    return section;
}

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

//static inline CCPagePtr _Nullable __CCPageTableRemoveFirst(CCPageTablePtr _Nonnull table) {
//
//    return NULL;
//}
//static inline CCPagePtr _Nullable __CCPageTableRemoveLast(CCPageTablePtr _Nonnull table) {
//
//    return NULL;
//}


/*
 remove empty section
 
 insert empty section
 */

static inline void __CCPageTableResize(CCPageTablePtr _Nonnull table, CCIndex capacity) {
    assert(table);
    capacity = __CCPageTableAlignCapacity(capacity);
    CCIndex oldCapacity = __CCPageTableGetCapacity(table);

    if (oldCapacity == capacity) {
        return;
    }
    assert(table->count <= capacity);
    
    CCPageSectionCollection_s * oldSections = &(table->content.sections);
    CCIndex oldIsSections = table->isSections;
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
    if (oldCapacity == 0) {
        if (capacity == 0) {
            //0->0
            memset(&(table->content), 0, sizeof(CCPageTableContent_u));
            table->offset = 0;
        } else if (capacity <= table->pageCountPerSection) {
            //0->1
            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = CCAllocate(sizeof(CCPagePtr) * capacity);
            
            table->capacity = capacity;
            table->offset = 0;
        } else {
            //0-2
            CCIndex sectionCount = capacity >> table->_sectionIndexShift;
            CCIndex sectionCapacity = CCPowerAlign2(sectionCount);
            CCPagePtr * * content = CCAllocate(sizeof(CCPagePtr *) * sectionCapacity);
            for (CCIndex idx=0; idx<sectionCount; idx++) {
                content[idx] = CCAllocate(sizeof(CCPagePtr) * table->pageCountPerSection);
            }
            
            table->sections.capacity = sectionCapacity;
            table->sections.offset = 0;
            table->sections.content = content;
            
            table->capacity = capacity;
            table->offset = 0;
        }
    } else if (table->capacity <= table->pageCountPerSection) {
        //1->0
        if (capacity == 0) {
            //缩容
            CCPagePtr * old = table->sections.content;

            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = NULL;

            table->capacity = capacity;
            table->offset = 0;
            
            CCDeallocate(old);
        } else if (capacity <= table->pageCountPerSection) {
            //1->1
            CCPagePtr * content = CCAllocate(sizeof(CCPagePtr) * capacity);
            CCPagePtr * old = table->sections.content;
            CCIndex iter = 0;
            for (CCIndex idx=table->offset; idx<table->capacity && iter<table->count; idx++, iter++) {
                content[iter] = old[idx];
            }
            for (CCIndex idx=0; idx<table->offset && iter<table->count; idx++, iter++) {
                content[iter] = old[idx];
            }
            
            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = content;
            table->capacity = capacity;
            table->offset = 0;
            
            CCDeallocate(old);
        } else {
            //1->2
            
            CCIndex sectionCount = capacity >> table->_sectionIndexShift;
            CCIndex sectionCapacity = CCPowerAlign2(sectionCount);
            CCPagePtr * * content = CCAllocate(sizeof(CCPagePtr *) * sectionCapacity);
            for (CCIndex idx=0; idx<sectionCount; idx++) {
                content[idx] = CCAllocate(sizeof(CCPagePtr) * table->pageCountPerSection);
            }
            
            CCPagePtr * target = content[0];
            CCPagePtr * old = table->sections.content;
            CCIndex iter = 0;
            for (CCIndex idx=table->offset; idx<table->capacity && iter<table->count; idx++, iter++) {
                target[iter] = old[idx];
            }
            for (CCIndex idx=0; idx<table->offset && iter<table->count; idx++, iter++) {
                target[iter] = old[idx];
            }
            
            table->sections.capacity = sectionCapacity;
            table->sections.offset = 0;
            table->sections.content = content;
            
            table->capacity = capacity;
            table->offset = 0;
            
            CCDeallocate(old);
        }
    } else {
        //2维
        CCIndex oldSectionCount = __CCPageTableGetSectionCount(table);
        CCPageSectionCollection_s sections = table->sections;

        if (capacity == 0) {
            //2->0
            CCPagePtr **  old = sections.content;

            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = NULL;

            table->capacity = capacity;
            table->offset = 0;
            
            CCIndex iter = 0;
            for (CCIndex idx=sections.offset; idx<sections.capacity && iter<oldSectionCount; idx++, iter++) {
                CCDeallocate(old[idx]);
            }
            for (CCIndex idx=0; idx<sections.offset && iter<oldSectionCount; idx++, iter++) {
                CCDeallocate(old[idx]);
            }
            CCDeallocate(old);
        } else if (capacity <= table->pageCountPerSection) {
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
            
            if (idx < table->count) {
                CCPagePtr * pages = __CCPageTableGetPages(table, idx, &length);
                memcpy(content + idx, pages, sizeof(CCPagePtr) * length);
                idx += length;
            }
            assert(idx == table->count);
            
            table->sections.capacity = 0;
            table->sections.offset = 0;
            table->sections.content = content;
            
            table->capacity = capacity;
            table->offset = 0;
            
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
            CCPagePtr **  old = sections.content;

            CCIndex sectionCount = capacity >> table->_sectionIndexShift;
            CCIndex sectionCapacity = CCPowerAlign2(sectionCount);
            CCPagePtr * * content = CCAllocate(sizeof(CCPagePtr *) * sectionCapacity);
            CCIndex iter = 0;
            for (CCIndex idx=sections.offset; idx<sections.capacity && iter<oldSectionCount; idx++, iter++) {
                if (iter < sectionCount) {
                    content[iter] = old[idx];
                } else {
                    CCDeallocate(old[idx]);
                    //剩余数据
                }
            }
            for (CCIndex idx=0; idx<sections.offset && iter<oldSectionCount; idx++, iter++) {
                if (iter < sectionCount) {
                    content[iter] = old[idx];
                } else {
                    CCDeallocate(old[idx]);
                    //剩余数据
                }
            }
            table->sections.capacity = sectionCapacity;
            table->sections.offset = 0;
            table->sections.content = content;
            
            table->capacity = capacity;
            //不修改offset
            CCDeallocate(old);
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
     CCIndex capacityMutable: 1;
     CCIndex __: 24;
     CCIndex _sectionIndexShift: 6;
     CCIndex pageCountPerSection: 32;
     CCIndex _sectionOffset: 32;
     CCIndex _sectionCapacity: 32;
     
     CCIndex offset;
     CCIndex capacity;
     CCIndex count;

     void * _Nonnull _content;
 } CCPageTable_s;

 */

static inline CCPageTablePtr _Nonnull __CCPageTableAllocate(CCIndex capacity) {
    CCPageTablePtr table = CCAllocate(sizeof(CCPageTable_s));
    table->capacity = capacity;
    table->count = 0;
    table->offset = 0;
    
    table->sections.capacity = 0;
    table->sections.offset = 0;
    table->sections.content = NULL;

    __CCPageTableResize(table, capacity);
    return table;
}

//
//typedef void (*CCCircularBufferEnumerateCallBack_f)(void * _Nullable context, CCRange range, size_t elementSize, const void * _Nonnull values);
//
//static inline void __CCPageTableEnumerate(CCPageTablePtr _Nonnull table, CCRange range, void * _Nullable context, CCCircularBufferEnumerateCallBack_f _Nonnull func) {
//    assert(table);
//    assert(func);
//    if (range.length == 0) {
//        return;
//    }
//    assert(range.location + range.length >= range.location);
//    assert(range.location + range.length <= table->count);
//    CCRange remain = range;
//
//    if (table->_sectionCapacity > 0) {
//        CCIndex pageIndex = 0;
//        do {
//            CCIndex location = __CCPageTableLoactionToIndex(table, remain.location);
//            CCPagePtr * storage = __CCPageTableGetPage(table, pageIndex);
//            CCIndex rLength = bufferElementCapacity - path.index;
//            if (rLength > remain.length) {
//                rLength = remain.length;
//            }
//            assert(rLength > 0);
//            func(context, CCRangeMake(remain.location, rLength), elementSize, storage + buffer->_indexOffset * elementSize);
//            remain.location += rLength;
//            remain.length -= rLength;
//            pageIndex += 1;
//            if (pageIndex >= table->count) {
//                pageIndex = 0;
//            }
//        } while (0);
//        while (remain.length > 0) {
//            uint8_t * storage = __CCCircularBufferGetPage(buffer, pageIndex);
//            CCIndex rLength = bufferElementCapacity;
//            if (rLength > remain.length) {
//                rLength = remain.length;
//            }
//            assert(rLength > 0);
//            func(context, CCRangeMake(remain.location, rLength), elementSize, storage + buffer->_indexOffset * elementSize);
//            remain.location += rLength;
//            remain.length -= rLength;
//            pageIndex += 1;
//            if (pageIndex >= table->count) {
//                pageIndex = 0;
//            }
//        }
//    } else {
//        CCIndex location = __CCCircularBufferIndexToLoaction(buffer, remain.location);
//        CCCircularBufferLocationPath path = __CCCircularBufferGetLocationPath(buffer, location);
//        uint8_t * storage = __CCCircularBufferGetPage(buffer, path.pageIndex);
//        CCIndex rLength = buffer->capacity - location;
//        if (rLength > range.length) {
//            rLength = range.length;
//        }
//        assert(rLength > 0);
//        func(context, CCRangeMake(remain.location, rLength), elementSize, storage + location * elementSize);
//        remain.location += rLength;
//        remain.length -= rLength;
//        if (rLength < range.length) {
//            func(context, remain, elementSize, storage);
//        }
//    }
//}














#endif /* CCPageTable_h */
