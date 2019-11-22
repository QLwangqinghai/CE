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
#include "CCPageSectionCollection.h"



#pragma mark - Base



//static inline CCPagePtr _Nullable * _Nonnull __CCPageSectionCollectionRemoveEmptySection(CCPageSectionCollectionPtr _Nonnull sections) {
////    assert(table->sections.capacity >= 2);
//
//
//    return NULL;
//}
//static inline void __CCPageSectionCollectionInsertEmptySection(CCPageSectionCollectionPtr _Nonnull sections, CCPagePtr _Nullable * _Nonnull page) {
//
//
//}

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

    CCPageSectionCollection_s sections;
} CCPageTable_s;



typedef CCPageTable_s * CCPageTablePtr;

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

static inline CCPagePtr _Nonnull __CCPageTableGetPage(CCPageTablePtr _Nonnull table, CCIndex pageIndex) {
    assert(pageIndex < table->count);
    CCIndex location = __CCPageTableIndexToLoaction(table, pageIndex);
    CCPageSectionCollection_s * sections = &(table->sections);
    CCPagePtr * section = __CCPageSectionCollectionGetSection(sections, location >> table->_sectionIndexShift);
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
    
    CCPageSectionCollection_s * sections = &(table->sections);
    CCPagePtr * section = __CCPageSectionCollectionGetSection(sections, location >> table->_sectionIndexShift);
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
    assert(table->count <= capacity);
    
    if (oldCapacity == capacity) {
        return;
    }
    CCPageSectionCollection_s * sections = &(table->sections);

    

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
            memset(sections, 0, sizeof(CCPageSectionCollection_s));
            table->offset = 0;
        } else if (capacity <= table->pageCountPerSection) {
            //0->1
            __CCPageSectionCollectionResize(sections, 1);
            CCPagePtr * section = CCAllocate(sizeof(CCPagePtr *) * capacity);
            __CCPageSectionCollectionAppend(sections, section);
            table->capacity = capacity;
            table->offset = 0;
        } else {
            //0-2
            CCIndex sectionCount = capacity >> table->_sectionIndexShift;
            __CCPageSectionCollectionResize(sections, sectionCount);
            for (CCIndex idx=0; idx<sectionCount; idx++) {
                CCPagePtr * section = CCAllocate(sizeof(CCPagePtr *) * table->pageCountPerSection);
                __CCPageSectionCollectionAppend(sections, section);
            }
            table->capacity = capacity;
            table->offset = 0;
        }
    } else if (table->capacity <= table->pageCountPerSection) {
        //1->0
        if (capacity == 0) {
            CCPagePtr * oldSection = __CCPageSectionCollectionRemoveLast(sections);
            CCDeallocate(oldSection);
        } else if (capacity <= table->pageCountPerSection) {
            //1->1
            CCPagePtr * oldSection = __CCPageSectionCollectionRemoveLast(sections);
            CCPagePtr * section = CCAllocate(sizeof(CCPagePtr) * capacity);
            CCIndex iter = 0;
            for (CCIndex idx=table->offset; idx<table->capacity && iter<table->count; idx++, iter++) {
                section[iter] = oldSection[idx];
            }
            for (CCIndex idx=0; idx<table->offset && iter<table->count; idx++, iter++) {
                section[iter] = oldSection[idx];
            }
            __CCPageSectionCollectionAppend(sections, section);
            table->capacity = capacity;
            table->offset = 0;
            CCDeallocate(oldSection);
        } else {
            //1->2
            CCPagePtr * oldSection = __CCPageSectionCollectionRemoveLast(sections);

            CCIndex sectionCount = capacity >> table->_sectionIndexShift;
            __CCPageSectionCollectionResize(sections, sectionCount);

            if (1) {
                CCPagePtr * section = CCAllocate(sizeof(CCPagePtr) * table->pageCountPerSection);
                CCIndex iter = 0;
                for (CCIndex idx=table->offset; idx<table->capacity && iter<table->count; idx++, iter++) {
                    section[iter] = oldSection[idx];
                }
                for (CCIndex idx=0; idx<table->offset && iter<table->count; idx++, iter++) {
                    section[iter] = oldSection[idx];
                }
                __CCPageSectionCollectionAppend(sections, section);
            }

            while (sections->count < sectionCount) {
                CCPagePtr * section = CCAllocate(sizeof(CCPagePtr) * table->pageCountPerSection);
                __CCPageSectionCollectionAppend(sections, section);
            }
            
            table->capacity = capacity;
            table->offset = 0;
            CCDeallocate(oldSection);
        }
    } else {
        //2维
        if (capacity == 0) {
            //2->0
            while (sections->count > 0) {
                CCPagePtr * oldSection = __CCPageSectionCollectionRemoveLast(sections);
                CCDeallocate(oldSection);
            }
        } else if (capacity <= table->pageCountPerSection) {
            //2->1
            CCPagePtr * section = CCAllocate(sizeof(CCPagePtr) * capacity);
            CCIndex idx = 0;
            CCIndex length = 0;
            if (1) {
                CCPagePtr * pages = __CCPageTableGetPages(table, idx, &length);
                memcpy(section, pages, sizeof(CCPagePtr) * length);
                idx += length;
            }
            if (idx < table->count) {
                CCPagePtr * pages = __CCPageTableGetPages(table, idx, &length);
                memcpy(section + idx, pages, sizeof(CCPagePtr) * length);
                idx += length;
            }
            assert(idx == table->count);
            while (sections->count > 0) {
                CCPagePtr * oldSection = __CCPageSectionCollectionRemoveLast(sections);
                CCDeallocate(oldSection);
            }
            __CCPageSectionCollectionAppend(sections, section);
            
            table->capacity = capacity;
            table->offset = 0;
        } else {
            //2->2
            CCIndex sectionCount = capacity >> table->_sectionIndexShift;
            if (sectionCount == sections->count) {
                abort();
            } else if (sectionCount > sections->count) {
                //拓容
                __CCPageSectionCollectionResize(sections, sectionCount);
                CCIndex offset2 = (table->offset & table->_indexInSectionMask);

                if (offset2 != 0 &&  table->capacity - table->count < table->pageCountPerSection) {
                    //section首尾分离
                    CCPagePtr * section = __CCPageSectionCollectionGetSection(sections, location >> table->_sectionIndexShift);

                    CCIndex length = 0;
                    if (1) {
                        
                        CCPagePtr * pages = __CCPageTableGetPages(table, idx, &length);

                    __CCPageSectionCollectionGetSection(sections, table->count - 1);

                }
                while (sections->count < sectionCount) {
//            应该插入空白页
                    CCPagePtr * section = CCAllocate(sizeof(CCPagePtr) * table->pageCountPerSection);
                    __CCPageSectionCollectionAppend(sections, section);
                }

            } else {
                //缩容
                while (sections->count > sectionCount) {
//            应该删除空白页
                    CCPagePtr * oldSection = __CCPageSectionCollectionRemoveLast(sections);
                    CCDeallocate(oldSection);
                }
                if ((table->offset & table->_indexInSectionMask) != 0 &&  table->capacity - table->count < table->pageCountPerSection) {
                    //section首尾合并


                }
                __CCPageSectionCollectionResize(sections, sectionCount);
            }
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
