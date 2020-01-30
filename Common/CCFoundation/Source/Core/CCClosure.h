//
//  CCClosur.h
//  CC
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CCClosure_h
#define CCClosure_h

#include "CCReferenceBase.h"

typedef CCRef CCClosureRef;

typedef void (*CCClosureExecute_f)(uintptr_t context, const CCPtr _Nullable input, CCPtr _Nullable output);
typedef void (*CCClosureClearContext_f)(uintptr_t context);

CCClosureRef _Nonnull CCClosureCreate(CCClosureExecute_f _Nonnull execute,
                                      CCClosureClearContext_f _Nullable clear,
                                      uintptr_t context);

void CCClosurePerform(CCClosureRef _Nonnull closure, const CCPtr _Nullable input, CCPtr _Nullable output);




void CCClosureRelease(CCClosureRef _Nonnull closure);
void CCClosureRetain(CCClosureRef _Nonnull closure);




#endif /* CEClosur_h */
