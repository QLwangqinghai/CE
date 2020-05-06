//
//  XAllocator.c
//  
//
//  Created by vector on 2020/4/4.
//

#include "internal/XAllocator.h"
#include "internal/XRuntimeInternal.h"



XPtr _Nonnull _XAllocatorConstantAllocate(_XAllocatorPtr _Nonnull allocator, XSize size) {
    abort();
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


XPtr _Nonnull _XAllocatorCompressedAllocate(_XAllocatorPtr _Nonnull allocator, XSize size) {
    assert(allocator);
    
    return NULL;
}
void _XAllocatorCompressedDeallocate(_XAllocatorPtr _Nonnull allocator, XPtr _Nonnull ptr) {
    assert(allocator);
    
}

XObject _Nonnull _XAllocatorCompressedObjectAllocate(_XAllocatorPtr _Nonnull allocator, XClass _Nonnull cls, XSize contentSize, XUInt32 flag) {
    
    return NULL;
}
void _XAllocatorCompressedObjectDeallocate(_XAllocatorPtr _Nonnull allocator, XObject _Nonnull obj) {
    
    
}




const _XAllocator_s _XCompressedObjectAllocator = {
    .context = NULL,
    .headerSize = sizeof(_XObjectCompressedBase),
    .allocate = _XAllocatorCompressedAllocate,
    .deallocate = _XAllocatorCompressedDeallocate,
    .allocateRef = _XAllocatorCompressedObjectAllocate,
    .deallocateRef = _XAllocatorCompressedObjectDeallocate,
};


const _XAllocator_s _XObjectAllocator = {
    .context = NULL,
    .headerSize = sizeof(_XObjectBase),
    .allocate = _XAllocatorDefaultAllocate,
    .deallocate = _XAllocatorDefaultDeallocate,
    .allocateRef = _XAllocatorDefaultObjectAllocate,
    .deallocateRef = _XAllocatorDefaultObjectDeallocate,
};


const _XAllocator_s _XClassAllocator = {
    .context = NULL,
    .headerSize = sizeof(_XObjectBase),
    .allocate = _XAllocatorDefaultAllocate,
    .deallocate = _XAllocatorDefaultDeallocate,
    .allocateRef = _XAllocatorDefaultObjectAllocate,
    .deallocateRef = _XAllocatorDefaultObjectDeallocate,
};

const _XAllocator_s _XConstantClassAllocator = {
    .context = NULL,
    .headerSize = sizeof(_XObjectBase),
    .allocate = _XAllocatorConstantAllocate,
    .deallocate = _XAllocatorConstantDeallocate,
    .allocateRef = _XAllocatorConstantObjectAllocate,
    .deallocateRef = _XAllocatorConstantObjectDeallocate,
};

const _XAllocator_s _XConstantAllocator = {
    .context = NULL,
    .headerSize = sizeof(_XObjectBase),
    .allocate = _XAllocatorConstantAllocate,
    .deallocate = _XAllocatorConstantDeallocate,
    .allocateRef = _XAllocatorConstantObjectAllocate,
    .deallocateRef = _XAllocatorConstantObjectDeallocate,
};
