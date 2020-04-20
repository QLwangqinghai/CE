//
//  XAllocator.h
//  
//
//  Created by vector on 2020/4/4.
//

#ifndef XAllocator_h
#define XAllocator_h


#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"
#include "XRuntime.h"

#pragma pack(push, 1)

typedef struct _XAllocator _XAllocator_s;

typedef XPtr _Nonnull (*_XAllocate_f)(_XAllocatorPtr _Nonnull allocator, XSize size);
typedef void (*_XDeallocate_f)(_XAllocatorPtr _Nonnull allocator, XPtr _Nonnull ptr);

struct _XAllocator {
    void * _Nullable context;
    
    _XAllocate_f _Nonnull allocate;
    _XDeallocate_f _Nonnull deallocate;
    
    XObjectAllocate_f _Nonnull allocateObject;
    XObjectDeallocate_f _Nonnull deallocateObject;
};

#pragma pack(pop)


extern const _XAllocator_s _XObjectAllocator;

extern const _XAllocator_s _XClassAllocator;

extern const _XAllocator_s _XConstantAllocator;



#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XAllocator_h */
