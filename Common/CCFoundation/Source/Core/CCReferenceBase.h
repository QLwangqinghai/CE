//
//  CCReferenceBase.h
//  SITP
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 angfung. All rights reserved.
//

#ifndef CCReferenceBase_h
#define CCReferenceBase_h

#include "CCBase.h"

typedef CCPtr CCRef;

typedef CCPtr CCRefWeakContainerPtr;

typedef uintptr_t CCRefType;


typedef void (*CCRefDeinit_f)(CCRef _Nonnull ref);
typedef void (*CCRefDomainDeinitCallback_f)(CCPtr _Nonnull context, CCRef _Nonnull ref);


extern const CCRefType CCTypeClosure;
extern const uint32_t CCDomain;

CCPtr _Nonnull CCRefGetContentPtr(CCRef _Nonnull ref);

CCRef _Nonnull CCRefAllocate(uint32_t domain, CCRefType type, size_t contentSize);

void CCRefRetain(CCRef _Nonnull ref);

CCRef _Nullable CCRefTryRetain(CCRef _Nonnull ref);
void CCRefRelease(CCRef _Nonnull ref);

//CCPtr CCRefAssociateManagerShared(void);




void CCRefWeakContainerDestroy(CCRefWeakContainerPtr _Nonnull container);



uint32_t CCDomainRefTypeTableRegister(const char * _Nonnull name, CCRefDeinit_f _Nonnull deinit);


#endif /* CCReferenceBase_h */
