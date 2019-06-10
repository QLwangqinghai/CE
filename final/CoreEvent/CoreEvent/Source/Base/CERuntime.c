//
//  CERuntime.c
//  CoreEvent
//
//  Created by vector on 2019/5/11.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CERuntime.h"
#include "CEMemory.h"


void CETypeDefaultDescript(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript) {
    CETypeRef type = CERefGetType(object);
    
    char buffer[32] = {};
    descript(handler, "<");
    
    descript(handler, type->specific->name);
    
    snprintf(buffer, 31, ":%p>", object);
    descript(handler, buffer);
}
void CERuntimeDefaultDeinit(CERef _Nonnull object) {}

void __CETypeMateDescript(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript) {
    CETypeDefaultDescript(object, handler, descript);
}

static const CETypeSpecific_s __CETypeMateSpecific = {
    .name = "Type",
    .descript = __CETypeMateDescript,
    .deinit = CERuntimeDefaultDeinit,
};

const CEAlloctor_s CERuntimeDefaultAlloctor = {
    .context = NULL,
    .allocate = CERuntimeDefaultAllocate,
    .deallocate = CERuntimeDefaultDeallocate,
    .destroyWeakRefrences = CERuntimeDefaultDestroyWeakRefrences,
    .retain = CERuntimeDefaultRetain,
    .tryRetain = CERuntimeDefaultTryRetain,
    .release = CERuntimeDefaultRelease,
};

/*
 CETypeRef const _Nonnull type;
 uint16_t version;
 uint16_t masks;
 uint32_t objectSize;//objectSize 不能为0
 uintptr_t identifier;
 
 CEAlloctorPtr _Nonnull alloctor;
 CETypeDescripterPtr _Nonnull descripter;
 void * _Nullable class;
 */


CEType_s __CETypeMate = {
    .runtime = {.type = &__CETypeMate},
    .version = CERuntimeVersion,
    .masks = CETypeBitHasRc | CETypeBitStatic,
    .objectSize = sizeof(CEType_s),
    .identifier = UINTPTR_MAX,
    
    .alloctor = &CERuntimeDefaultAlloctor,
    .specific = &__CETypeMateSpecific,
    .class = NULL,
};



CERef _Nonnull CERetain(const CERef _Nonnull object) {
    assert(object);
    CETypeRef type = CERefGetType(object);
    return type->alloctor->retain(object);
}
CERef _Nullable CETryRetain(const CERef _Nonnull object) {
    assert(object);
    CETypeRef type = CERefGetType(object);
    return type->alloctor->tryRetain(object);
}
void CERelease(const CERef _Nonnull object) {
    assert(object);
    CETypeRef type = CERefGetType(object);
    return type->alloctor->release(object);
}


void * _Nonnull CERuntimeDefaultAllocate(CETypeRef _Nonnull typeRef, size_t size) {
    CERetain((void *)typeRef);

    void * object = CEAllocateClear(size);
    memcpy(object, &typeRef, sizeof(void *));
    CEType_s * ptr = object;
    
    if (__builtin_expect(((typeRef->masks & CETypeBitHasRc) == CETypeBitHasRc), true)) {
        CERuntimeAtomicRcBase_s * header = object;
        if ((typeRef->masks & CETypeBitStatic) == CETypeBitStatic) {
#if CEBuild64Bit
            uint_fast64_t rcInfo = CERuntimeRcStatic;
            _Atomic(uint_fast64_t) * rcInfoPtr = &(header->rcInfo);
#else
            uint_fast32_t rcInfo = CERuntimeRcStatic;
            _Atomic(uint_fast32_t) * rcInfoPtr = &(header->rcInfo);
#endif
            atomic_store(rcInfoPtr, rcInfo);
        } else {
#if CEBuild64Bit
            uint_fast64_t rcInfo = 1;
            _Atomic(uint_fast64_t) * rcInfoPtr = &(header->rcInfo);
#else
            uint_fast32_t rcInfo = 1;
            _Atomic(uint_fast32_t) * rcInfoPtr = &(header->rcInfo);
#endif
            atomic_store(rcInfoPtr, rcInfo);
        }
    }
    
    return ptr;
}
void CERuntimeDefaultDeallocate(CETypeRef _Nonnull type, void * _Nonnull ptr) {
    CEDeallocate(ptr);
}

void CERuntimeDefaultDestroyWeakRefrences(CERef _Nonnull object) {
    assert(object);
}

static inline CERef _Nullable ___CERuntimeRetain(CERef _Nonnull object, _Bool tryR) {
    assert(object);
    CETypeRef type = ((CERuntimeBase_s *)object)->type;
    assert(type);

    
    if (__builtin_expect(((type->masks & CETypeBitHasRc) == CETypeBitHasRc), true)) {
        if ((type->masks & CERuntimeRcStatic) != CERuntimeRcStatic) {
            CERuntimeAtomicRcBase_s * header = object;
            
#if CEBuild64Bit
            uint_fast64_t rcInfo = 0;
            uint_fast64_t newRcInfo = 0;
            _Atomic(uint_fast64_t) * rcInfoPtr = &(header->rcInfo);
#else
            uint_fast32_t rcInfo = 0;
            uint_fast32_t newRcInfo = 0;
            _Atomic(uint_fast32_t) * rcInfoPtr = &(header->rcInfo);
#endif
            do {
                rcInfo = atomic_load(rcInfoPtr);
                CERcType_t rc = rcInfo & CERuntimeRcMask;

                if (__builtin_expect((rc < CERuntimeRcStatic), true)) {
                    rc += 1;
                } else if (rc == CERuntimeRcStatic) {
                    return object;
                } else {
                    if (!tryR) {
                        CELogError("retain a dealloced object");
                        abort();
                    }
                    return NULL;
                }
                CERcType_t weakBit = rcInfo & CERuntimeRcWeakBit;
                newRcInfo = weakBit | rc;
            } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
        }
    }
    return object;
}
CERef _Nonnull CERuntimeDefaultRetain(CERef _Nonnull object) {
    assert(object);
    
    return ___CERuntimeRetain(object, false);
}
CERef _Nullable CERuntimeDefaultTryRetain(CERef _Nonnull object) {
    if (NULL == object) {
        return NULL;
    }
    return ___CERuntimeRetain(object, true);
}

void CERuntimeDefaultRelease(CERef _Nonnull object) {
    assert(object);
    CETypeRef type = ((CERuntimeBase_s *)object)->type;
    assert(type);
    if (__builtin_expect(((type->masks & CETypeBitHasRc) == CETypeBitHasRc), true)) {
        if ((type->masks & CERuntimeRcStatic) != CERuntimeRcStatic) {
            CERuntimeAtomicRcBase_s * header = object;
            
#if CEBuild64Bit
            uint64_t rcInfo = 0;
            uint64_t newRcInfo = 0;
            uint64_t rc = 0;
            uint64_t weakBit = 0;
            _Atomic(uint_fast64_t) * rcInfoPtr = &(header->rcInfo);

#else
            uint_fast32_t rcInfo = 0;
            uint_fast32_t newRcInfo = 0;
            uint_fast32_t rc = 0;
            uint_fast32_t weakBit = 0;
            _Atomic(uint_fast32_t) * rcInfoPtr = &(header->rcInfo);
#endif
            

            do {
                rcInfo = atomic_load(rcInfoPtr);
                rc = rcInfo & CERuntimeRcMask;
                
                if (__builtin_expect((rc != 0), true)) {
                    if (__builtin_expect((rc < CERuntimeRcStatic), true)) {
                        rc -= 1;
                    } else if (rc == CERuntimeRcStatic) {
                        return;
                    } else {
                        CELogError("retain a dealloced object");
                        abort();
                        return;
                    }
                } else {
                    CELogError("release a dealloced object");
                    abort();
                }
                
                weakBit = rcInfo & CERuntimeRcWeakBit;
                newRcInfo = weakBit | rc;
            } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
            if (0 == rc) {
                //标记dealloc 状态
                do {
                    rcInfo = atomic_load(rcInfoPtr);
                    rc = rcInfo & CERuntimeRcMask;
                    if (0 != rc) {
                        return;
                    }
                    rc = CERuntimeRcDeallocing;
                    weakBit = rcInfo & CERuntimeRcWeakBit;
                    newRcInfo = weakBit | rc;
                } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
                //清除weak
                type->alloctor->destroyWeakRefrences(object);
                //deinit
                type->specific->deinit(object);
                //回收内存
                type->alloctor->deallocate(type, object);
            }
        }
    }
}

_Bool CETypeIsEqual(CETypeRef _Nonnull type0, CETypeRef _Nonnull type1) {
    assert(type0);
    assert(type1);
    
    return type0->identifier == type1->identifier;
}
CETypeRef _Nonnull CERefGetType(CERef _Nonnull ref) {
    CERuntimeBase_s * base = ref;
    CETypeRef type = base->type;
    assert(type);
    return type;
}
