//
//  CCClosur.c
//  CC
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CCClosure.h"

static const CCUInt CCClosureContextSizeMax = CCUInt_MAX >> 1;


typedef struct {
    CCClosureExecute_f _Nonnull execute;
    CCUInt hasClear: 1;
#if CCBuild64Bit
    CCUInt contextSize: 63;
#else
    CCUInt contextSize: 31;
#endif
} CCClosureBase;

typedef struct {
    CCClosureClearContext_f _Nullable clear;
    uint8_t context[0];
} CCClosureContext;


CCClosureRef _Nonnull CCClosureCreate(CCClosureExecute_f _Nonnull execute) {
    assert(execute);
    CCClosureRef ref = CCRefAllocate(CCDomain, CCTypeClosure, sizeof(CCClosureBase));
    uint8_t * tmp = CCRefGetContentPtr(ref);
    CCClosureBase * base = (CCClosureBase *)tmp;
    base->execute = execute;
    base->hasClear = 0;
    base->contextSize = 0;
    return ref;
}

CCClosureRef _Nonnull CCClosureCreateWithContext(CCClosureExecute_f _Nonnull execute,
                                                 CCClosureClearContext_f _Nullable clear,
                                                 CCPtr _Nonnull context,
                                                 size_t contextSize) {
    assert(contextSize > 0);
    assert(contextSize <= CCClosureContextSizeMax);
    assert(context);
    assert(execute);
    size_t size = sizeof(CCClosureBase);
    if (NULL != clear) {
        size += sizeof(CCClosureClearContext_f);
    }
    
    CCClosureRef ref = CCRefAllocate(CCDomain, CCTypeClosure, sizeof(CCClosureBase));
    uint8_t * tmp = CCRefGetContentPtr(ref);
    
    CCClosureBase * base = (CCClosureBase *)tmp;
    tmp += sizeof(CCClosureBase);
    base->execute = execute;
    base->contextSize = contextSize;
    if (NULL != clear) {
        void ** clearPtr = (void **)tmp;
        tmp += sizeof(void *);
        *clearPtr = clear;
        base->hasClear = 1;
    } else {
        base->hasClear = 0;
    }
    
    memcpy(tmp, context, contextSize);
    return ref;
}

void CCClosurePerform(CCClosureRef _Nonnull closure, const CCPtr _Nullable input, CCPtr _Nullable output) {
    assert(closure);
    CCRefRetain(closure);
    uint8_t * tmp = CCRefGetContentPtr(closure);
    CCClosureBase * base = (CCClosureBase *)tmp;
    tmp += sizeof(CCClosureBase);
    if (base->contextSize == 0) {
        base->execute(input, output, NULL, 0);
    } else {
        if (base->hasClear != 0) {
            tmp += sizeof(void *);
        }
        base->execute(input, output, tmp, base->contextSize);
    }
    CCRefRelease(closure);
}

void _CCClosureDeinit(CCClosureRef _Nonnull closure) {
    uint8_t * tmp = CCRefGetContentPtr(closure);
    CCClosureBase * base = (CCClosureBase *)tmp;
    if (base->contextSize > 0) {
        tmp += sizeof(CCClosureBase);
        if (base->hasClear != 0) {
            CCClosureClearContext_f clear = (CCClosureClearContext_f)tmp;
            tmp += sizeof(void *);
            clear(tmp, base->contextSize);
        }
    }
}
