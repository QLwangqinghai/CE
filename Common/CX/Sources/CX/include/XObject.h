//
//  XObject.h
//  
//
//  Created by vector on 2020/4/4.
//

#ifndef XObject_h
#define XObject_h


#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"


#pragma mark - equalTo

extern XBool XObjectEqual(XRef _Nonnull lhs, XRef _Nonnull rhs);

#pragma mark - hash

extern XHashCode XObjectHash(XRef _Nonnull ref);





extern XPtr _Nonnull XObjectGetContent(XObject _Nonnull obj);


#if defined(__cplusplus)

}  // extern C
#endif

#endif /* XObject_h */
