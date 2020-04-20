//
//  XAllocator.c
//  
//
//  Created by vector on 2020/4/4.
//

#include "internal/XAllocator.h"


XPtr _Nonnull _XAllocatorDefaultAllocate(_XAllocatorPtr _Nonnull allocator, XSize size) {
    assert(allocator);
    
    return NULL;
}
void _XAllocatorDefaultDeallocate(_XAllocatorPtr _Nonnull allocator, XPtr _Nonnull ptr) {
    assert(allocator);
    
}

XObject _Nonnull _XAllocatorDefaultObjectAllocate(_XAllocatorPtr _Nonnull allocator, XClass _Nonnull cls, XSize contentSize, XUInt32 flag) {
    
    return NULL;
}
void _XAllocatorDefaultObjectDeallocate(_XAllocatorPtr _Nonnull allocator, XObject _Nonnull obj) {
    
    
}

const _XAllocator_s _XObjectAllocator = {
    .context = NULL,
    .allocate = _XAllocatorDefaultAllocate,
    .deallocate = _XAllocatorDefaultDeallocate,
    .allocateObject = _XAllocatorDefaultObjectAllocate,
    .deallocateObject = _XAllocatorDefaultObjectDeallocate,
};


const _XAllocator_s _XClassAllocator = {
    .context = NULL,
    .allocate = _XAllocatorDefaultAllocate,
    .deallocate = _XAllocatorDefaultDeallocate,
    .allocateObject = _XAllocatorDefaultObjectAllocate,
    .deallocateObject = _XAllocatorDefaultObjectDeallocate,
};





XPtr _Nonnull _XAllocatorConstantAllocate(_XAllocatorPtr _Nonnull allocator, XSize size) {
    abort();
    return NULL;
}
void _XAllocatorConstantDeallocate(_XAllocatorPtr _Nonnull allocator, XPtr _Nonnull ptr) {
    abort();
}

XObject _Nonnull _XAllocatorConstantObjectAllocate(_XAllocatorPtr _Nonnull allocator, XClass _Nonnull cls, XSize contentSize, XUInt32 flag) {
    abort();
}
void _XAllocatorConstantObjectDeallocate(_XAllocatorPtr _Nonnull allocator, XObject _Nonnull obj) {
    abort();
}

const _XAllocator_s _XConstantAllocator = {
    .context = NULL,
    .allocate = _XAllocatorDefaultAllocate,
    .deallocate = _XAllocatorDefaultDeallocate,
    .allocateObject = _XAllocatorDefaultObjectAllocate,
    .deallocateObject = _XAllocatorDefaultObjectDeallocate,
};
