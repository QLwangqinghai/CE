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

typedef void (*CCClosureClearContext_f)(CCPtr _Nonnull contextBuffer, size_t contextBuffertSize);
typedef void (*CCClosureExecute_f)(const CCPtr _Nullable input, CCPtr _Nullable output, CCPtr _Nullable contextBuffer, size_t contextBuffertSize);


/*
 内存布局
 CCRef header ;1 word
 CCClosureExecute_f _Nonnull ; 1word
 size_t contextSize; 1word
 
 
 // contextSize <= 0 时以下没有内容
 CCClosureClearContext_f _Nullable clear; optional
 uint8_t context[0]; contextSize
 */


CCClosureRef _Nonnull CCClosureCreate(CCClosureExecute_f _Nonnull execute);

CCClosureRef _Nonnull CCClosureCreateWithContext(CCClosureExecute_f _Nonnull execute,
                                                 CCClosureClearContext_f _Nullable clear,
                                                 CCPtr _Nonnull contextBuffer,
                                                 size_t contextBuffertSize);

void CCClosurePerform(CCClosureRef _Nonnull closure, const CCPtr _Nullable input, CCPtr _Nullable output);




void _CCClosureDeinit(CCClosureRef _Nonnull closure);



#endif /* CEClosur_h */
