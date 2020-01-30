//
//  CCClosur.c
//  CC
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CCClosure.h"

typedef struct {
    CCClosureExecute_f _Nonnull execute;
    CCClosureClearContext_f _Nullable clear;
    uintptr_t context;
} CCClosure;


CCClosureRef _Nonnull CCClosureCreate(CCClosureExecute_f _Nonnull execute,
                                      CCClosureClearContext_f _Nullable clear,
                                      uintptr_t context) {
    assert(execute);
    CCClosureRef ref = CCRefAllocate(CCTypeClosure, sizeof(CCClosure));
    CCClosure * content = CCRefGetContentPtr(ref);
    content->execute = execute;
    content->clear = clear;
    content->context = context;
    return ref;
}

void CCClosurePerform(CCClosureRef _Nonnull closure, const CCPtr _Nullable input, CCPtr _Nullable output) {
    CCClosure * content = CCRefGetContentPtr(closure);
    content->execute(content->context, input, output);
}


void CCClosureRelease(CCClosureRef _Nonnull closure) {
    if (__CCRefRelease(closure)) {
        CCClosure * content = CCRefGetContentPtr(closure);
        if (content->clear) {
            content->clear(content->context);
        }
    }
}
void CCClosureRetain(CCClosureRef _Nonnull closure) {
    __CCRefRetain(closure);
}

