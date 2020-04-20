//
//  CCPageSectionCollection.h
//  CCollection
//
//  Created by vector on 2019/11/21.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCPageSectionCollection_h
#define CCPageSectionCollection_h

#include "CCBase.h"

typedef struct {
    CCIndex count;
    CCIndex capacity;
    uintptr_t _load[2];
} CCPageSectionCollection_s;

#define _CCPageSectionCollectionOffsetIndex 0
#define _CCPageSectionCollectionContentIndex 1

typedef CCPageSectionCollection_s * CCPageSectionCollectionPtr;

static inline CCPagePtr _Nullable * _Nonnull __CCPageSectionCollectionGetSection(const CCPageSectionCollectionPtr _Nonnull collection, CCIndex index) {
    assert(index < collection->count);
    if (collection->count <= 2) {
        return (void *)(collection->_load[index]);
    } else {
        CCIndex capacity = collection->capacity;
        CCIndex offset = collection->_load[_CCPageSectionCollectionOffsetIndex];
        CCIndex location = 0;
        CCIndex r = capacity - offset;
        if (index >= r) {
            location = index - r;
        } else {
            location = index + offset;
        }
        CCPagePtr * * storage = (void *)(collection->_load[_CCPageSectionCollectionContentIndex]);
        CCPagePtr * result = storage[location];
        assert(result);
        return result;
    }
}

static inline CCPagePtr _Nullable * _Nonnull __CCPageSectionCollectionReplaceSection(const CCPageSectionCollectionPtr _Nonnull collection, CCIndex index, CCPagePtr _Nullable * _Nonnull section) {
    assert(index < collection->count);
    if (collection->count <= 2) {
        CCPagePtr * old = (void *)(collection->_load[index]);
        collection->_load[index] = (uintptr_t)(section);
        return old;
    } else {
        CCIndex capacity = collection->capacity;
        CCIndex offset = collection->_load[_CCPageSectionCollectionOffsetIndex];
        CCIndex location = 0;
        CCIndex r = capacity - offset;
        if (index >= r) {
            location = index - r;
        } else {
            location = index + offset;
        }
        CCPagePtr * * storage = (void *)(collection->_load[_CCPageSectionCollectionContentIndex]);
        CCPagePtr * result = storage[location];
        storage[location] = section;
        assert(result);
        return result;
    }
}

static inline void __CCPageSectionCollectionPrepend(CCPageSectionCollectionPtr _Nonnull collection, CCPagePtr _Nullable * _Nonnull section) {
    assert(collection->count < collection->capacity);
    if (collection->capacity <= 2) {
        collection->_load[1] = collection->_load[0];
        collection->_load[0] = (uintptr_t)section;
        collection->count += 1;
    } else {
        CCIndex offset = collection->_load[_CCPageSectionCollectionOffsetIndex];
        CCPagePtr * * storage = (void *)(collection->_load[_CCPageSectionCollectionContentIndex]);
        
        CCIndex location = 0;
        if (offset == 0) {
            location = collection->capacity - 1;
        } else {
            location = offset - 1;
        }
        storage[location] = section;
        collection->count += 1;
        collection->_load[_CCPageSectionCollectionOffsetIndex] = location;
    }
}

static inline void __CCPageSectionCollectionAppend(CCPageSectionCollectionPtr _Nonnull collection, CCPagePtr _Nullable * _Nonnull section) {
    assert(collection->count < collection->capacity);
    if (collection->capacity <= 2) {
        collection->_load[collection->count] = (uintptr_t)section;
        collection->count += 1;
    } else {
        CCIndex offset = collection->_load[_CCPageSectionCollectionOffsetIndex];
        CCPagePtr * * storage = (void *)(collection->_load[_CCPageSectionCollectionContentIndex]);
        CCIndex index = collection->count;
        CCIndex location = 0;
        CCIndex r = collection->capacity - offset;
        if (index >= r) {
            location = index - r;
        } else {
            location = index + offset;
        }
        storage[location] = section;
        collection->count += 1;
    }
}
static inline CCPagePtr _Nullable * _Nonnull __CCPageSectionCollectionRemoveFirst(CCPageSectionCollectionPtr _Nonnull collection) {
    assert(collection->count > 0);
    if (collection->capacity <= 2) {
        CCPagePtr * first = (CCPagePtr *)(collection->_load[0]);
        collection->_load[0] = collection->_load[1];
        collection->_load[1] = (uintptr_t)NULL;
        collection->count -= 1;
        return first;
    } else {
        CCIndex offset = collection->_load[_CCPageSectionCollectionOffsetIndex];
        CCPagePtr * * storage = (void *)(collection->_load[_CCPageSectionCollectionContentIndex]);
        
        CCPagePtr * first = storage[offset];
        storage[offset] = NULL;
        offset += 1;
        if (offset == collection->capacity) {
            offset = 0;
        } else {
            assert(offset < collection->capacity);
        }
        collection->_load[_CCPageSectionCollectionOffsetIndex] = offset;
        collection->count -= 1;
        return first;
    }
}


static inline CCPagePtr _Nullable * _Nonnull __CCPageSectionCollectionRemoveLast(CCPageSectionCollectionPtr _Nonnull collection) {
    assert(collection->count > 0);
    if (collection->capacity <= 2) {
        CCPagePtr * last = (CCPagePtr *)(collection->_load[collection->count-1]);
        collection->_load[collection->count-1] = (uintptr_t)NULL;
        collection->count -= 1;
        return last;
    } else {
        CCIndex offset = collection->_load[_CCPageSectionCollectionOffsetIndex];
        CCPagePtr * * storage = (void *)(collection->_load[_CCPageSectionCollectionContentIndex]);
        
        CCIndex index = collection->count - 1;
        CCIndex location = 0;
        CCIndex r = collection->capacity - offset;
        if (index >= r) {
            location = index - r;
        } else {
            location = index + offset;
        }
        
        CCPagePtr * last = storage[location];
        storage[location] = NULL;
        collection->count -= 1;
        return last;
    }
}



static inline void __CCPageSectionCollectionResize(CCPageSectionCollectionPtr _Nonnull collection, CCIndex capacity) {
    assert(collection);
    capacity = CCPowerAlign2(capacity);
    if (capacity < 2) {
        capacity = 2;
    }
    CCIndex oldCapacity = collection->capacity;

    assert(collection->count <= capacity);
    /*
     2-> >2
     >2 -> 2
     */
    if (oldCapacity == capacity) {
        return;
    } else if (oldCapacity <= 2 && capacity > 2) {
        void * * content = CCAllocate(sizeof(void * *) * capacity);
        for (CCIndex idx=0; idx<oldCapacity; idx++) {
            content[idx] = (void *)(collection->_load[idx]);
        }
        collection->_load[_CCPageSectionCollectionOffsetIndex] = 0;
        collection->_load[_CCPageSectionCollectionContentIndex] = (uintptr_t)content;
        collection->capacity = capacity;
    } else if (oldCapacity > 2 && capacity <= 2) {
        CCIndex capacity = collection->capacity;
        CCIndex offset = collection->_load[_CCPageSectionCollectionOffsetIndex];
        CCPagePtr * * storage = (void *)(collection->_load[_CCPageSectionCollectionContentIndex]);
        for (CCIndex index=0; index<collection->count; index++) {
            CCIndex location = 0;
            CCIndex r = capacity - offset;
            if (index >= r) {
                location = index - r;
            } else {
                location = index + offset;
            }
            collection->_load[index] = (uintptr_t)(storage[location]);
        }
        collection->capacity = capacity;
        CCDeallocate(storage);
    } else {
        void * * content = CCAllocate(sizeof(void * *) * capacity);
        CCIndex capacity = collection->capacity;
        CCIndex offset = collection->_load[_CCPageSectionCollectionOffsetIndex];
        CCPagePtr * * oldContent = (void *)(collection->_load[_CCPageSectionCollectionContentIndex]);
        
        CCIndex iter = 0;
        for (CCIndex idx=offset; idx<capacity && iter<collection->count; idx++, iter++) {
            content[iter] = oldContent[idx];
        }
        for (CCIndex idx=0; idx<offset && iter<collection->count; idx++, iter++) {
            content[iter] = oldContent[idx];
        }
        CCDeallocate(oldContent);
        collection->_load[_CCPageSectionCollectionOffsetIndex] = 0;
        collection->_load[_CCPageSectionCollectionContentIndex] = (uintptr_t)content;
        collection->capacity = capacity;
    }
}



#endif /* CCPageSectionCollection_h */
