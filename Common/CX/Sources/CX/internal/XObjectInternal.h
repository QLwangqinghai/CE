//
//  XObjectInternal.h
//  
//
//  Created by vector on 2020/4/4.
//

#ifndef XObjectInternal_h
#define XObjectInternal_h


#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"
#include "XObject.h"
#include <stdatomic.h>

#define XFastSInt32 int_fast32_t
#define XFastUInt32 uint_fast32_t


#define XFastSInt64 int_fast64_t
#define XFastUInt64 uint_fast64_t

#if BUILD_TARGET_RT_64_BIT
    #define XFastSInt int_fast64_t
    #define XFastUInt uint_fast64_t

    #define XFastSIntMin INT_FAST64_MIN
    #define XFastSIntMax INT_FAST64_MAX

    #define XFastUIntMax UINT_FAST64_MAX

#elif BUILD_TARGET_RT_32_BIT
    #define XFastSInt int_fast32_t
    #define XFastUInt uint_fast32_t

    #define XFastSIntMin INT_FAST32_MIN
    #define XFastSIntMax INT_FAST32_MAX

    #define XFastUIntMax UINT_FAST32_MAX

#else
    #error unknown rt
#endif


typedef struct {
    _Atomic(XFastUInt32) _refCount;
    XUInt32 size;
    XUInt8 content[0];
} _XBuffer;

extern _XBuffer * _Nonnull _XBufferAllocate(XUInt32 size);
extern XPtr _Nonnull _XBufferGetContent(_XBuffer * _Nonnull buffer);

extern _XBuffer * _Nonnull _XBufferRetain(_XBuffer * _Nonnull buffer);
extern void _XBufferRelease(_XBuffer * _Nonnull buffer);


typedef XPtr _XObject;

XPtr _Nonnull _XObjectGetContent(_XObject _Nonnull obj);

//_XObject _Nonnull _XObjectAllocate(uint32_t domain, CCRefType type, size_t contentSize);
//
//void CCRefRetain(CCRef _Nonnull ref);
//
//CCRef _Nullable CCRefTryRetain(CCRef _Nonnull ref);
//void CCRefRelease(CCRef _Nonnull ref);




#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XObjectInternal_h */
