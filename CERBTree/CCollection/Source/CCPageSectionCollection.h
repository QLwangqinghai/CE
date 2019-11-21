//
//  CCPageSectionCollection.h
//  CCollection
//
//  Created by vector on 2019/11/21.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCPageSectionCollection_h
#define CCPageSectionCollection_h

#include "CInteger.h"
#include "CType.h"
#include "CCBase.h"
#include "CCAtomic.h"

typedef struct {
    CCIndex count;
    uintptr_t _load[3];
} CCPageSectionCollection_s;

#define _CCPageSectionCollectionCapacityIndex 0
#define _CCPageSectionCollectionOffsetIndex 1
#define _CCPageSectionCollectionContentIndex 2

typedef CCPageSectionCollection_s * CCPageSectionCollectionPtr;

static inline CCPagePtr _Nullable * _Nonnull __CCPageSectionCollectionGetSection(const CCPageSectionCollectionPtr _Nonnull collection, CCIndex index) {
    assert(index < collection->count);
    if (collection->count <= 3) {
        return (void *)(collection->_load[index]);
    } else {
        CCIndex capacity = collection->_load[_CCPageSectionCollectionCapacityIndex];
        CCIndex offset = collection->_load[_CCPageSectionCollectionOffsetIndex];

        CCIndex location = 0;
        CCIndex r = capacity - offset;
        if (index >= r) {
            location = index - r;
        } else {
            location = index + offset;
        }
        CCPagePtr * * storage = (void *)(collection->_load[_CCPageSectionCollectionContentIndex]);;
        CCPagePtr * result = storage[location];
        assert(result);
        return result;
    }
}

static inline void __CCPageSectionCollectionPrepend(CCPageSectionCollectionPtr _Nonnull collection, CCPagePtr _Nullable * _Nonnull section) {


}

static inline void __CCPageSectionCollectionAppend(CCPageSectionCollectionPtr _Nonnull collection, CCPagePtr _Nullable * _Nonnull section) {


}
static inline CCPagePtr _Nullable * _Nonnull __CCPageSectionCollectionRemoveFirst(CCPageSectionCollectionPtr _Nonnull collection) {
//    assert(table->sections.capacity >= 2);
    
    
    return NULL;
}


static inline CCPagePtr _Nullable * _Nonnull __CCPageSectionCollectionRemoveLast(CCPageSectionCollectionPtr _Nonnull collection) {
//    assert(table->sections.capacity >= 2);
    
    
    return NULL;
}





#endif /* CCPageSectionCollection_h */
