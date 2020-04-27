//
//  XByteStorage.h
//  
//
//  Created by vector on 2020/4/4.
//

#ifndef XByteStorage_h
#define XByteStorage_h


#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"
#include "../internal/XRuntimeInternal.h"

#define X_BUILD_ExtendedLayoutInline 0
#define X_BUILD_ExtendedLayoutBuffer 1


extern const _XExtendedLayout _XExtendedLayoutInline;
extern const _XExtendedLayout _XExtendedLayoutBuffer;


#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XByteStorage_h */
