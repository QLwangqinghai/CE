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
    descript(handler, type->descripter->name);
    
    snprintf(buffer, 31, ":%p>", object);
    descript(handler, buffer);
}
void CETypeDefaultDeinit(CERef _Nonnull object) {}


void * _Nonnull __CETypeMateAllocate(CETypeRef _Nonnull type, size_t size) {
    void * ptr = CEAllocateClear(size);
    memcpy(ptr, &type, sizeof(CERef));
    return ptr;
}
void __CETypeMateDeallocate(CETypeRef _Nonnull type, void * _Nonnull ptr) {
    CEDeallocate(ptr);
}


void __CETypeMateDestroyWeakRefrences(CERef _Nonnull object) {
    assert(object);
}
CERef _Nonnull __CETypeMateRetain(CERef _Nonnull object) {
    assert(object);
    return object;
}
CERef _Nullable __CETypeMateTryRetain(CERef _Nonnull object) {
    assert(object);
    return object;
}
void __CETypeMateRelease(CERef _Nonnull object) {
    assert(object);
}

static const CEAlloctor_s __CETypeMateAlloctor = {
    .context = NULL,
    .allocate = __CETypeMateAllocate,
    .deallocate = __CETypeMateDeallocate,
    .destroyWeakRefrences = __CETypeMateDestroyWeakRefrences,
    .retain = __CETypeMateRetain,
    .tryRetain = __CETypeMateTryRetain,
    .release = __CETypeMateRelease,
};


size_t __CETypeMateGetSize(CERef _Nonnull object) {
    CETypeRef type = ((CERuntimeBase_t *)object)->type;
    return type->objectSize;
}
void __CETypeMateInit(CERef _Nonnull object) {
}
void __CETypeMateDeinit(CERef _Nonnull object) {
}

void __CETypeMateDescript(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript) {
    CETypeDefaultDescript(object, handler, descript);
}

static const CETypeDescripter_s __CETypeMateDescripter = {
    .name = "Type",
    .descript = __CETypeMateDescript,
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
    .type = &__CETypeMate,
    .version = CERuntimeVersion,
    .masks = CETypeMaskNoRc,
    .objectSize = CETypeBaseLayoutSize,
    .identifier = UINTPTR_MAX,
    
    .alloctor = &__CETypeMateAlloctor,
    .descripter = &__CETypeMateDescripter,
    .class = NULL,
};



CERef _Nonnull CERetain(CERef _Nonnull object) {
    assert(object);
    return ((CERuntimeBase_t *)object)->type->alloctor->retain(object);
}
CERef _Nullable CETryRetain(CERef _Nonnull object) {
    assert(object);
    return ((CERuntimeBase_t *)object)->type->alloctor->tryRetain(object);
}
void CERelease(CERef _Nonnull object) {
    assert(object);
    return ((CERuntimeBase_t *)object)->type->alloctor->release(object);
}


void * _Nonnull __CETypeAllocate(CETypeRef _Nonnull type, size_t size) {
    void * object = CEAllocateClear(size);
    memcpy(object, &type, sizeof(void *));
    CEType_s * ptr = object;
    
    if (__builtin_expect(((type->masks & CETypeBitHasRc) == CETypeBitHasRc), true)) {
        if ((type->masks & CETypeBitRcAtomic) == CETypeBitRcAtomic) {
            CERuntimeAtomicRcBase_t * header = object;
#if CEBuild64Bit
            uint_fast64_t rcInfo = 1;
            _Atomic(uint_fast64_t) * rcInfoPtr = &(header->rcInfo);
#else
            uint_fast32_t rcInfo = 1;
            _Atomic(uint_fast32_t) * rcInfoPtr = &(header->rcInfo);
#endif
            atomic_store(rcInfoPtr, rcInfo);
        } else {
            CERuntimeUnsafeRcBase_t * header = object;
            header->rcInfo = 1;
        }
    }
    
    return ptr;
}
void __CETypeDeallocate(CETypeRef _Nonnull type, void * _Nonnull ptr) {
    CEDeallocate(ptr);
}

void __CETypeDestroyWeakRefrences(CERef _Nonnull object) {
    assert(object);
}

static inline CERef _Nullable ___CETypeRetain(CERef _Nonnull object, _Bool tryR) {
    assert(object);
    CETypeRef type = ((CERuntimeBase_t *)object)->type;
    assert(type);
    
    if (__builtin_expect(((type->masks & CETypeBitHasRc) == CETypeBitHasRc), true)) {
        if ((type->masks & CETypeBitRcAtomic) == CETypeBitRcAtomic) {
            CERuntimeAtomicRcBase_t * header = object;
            
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
        } else {
            CERuntimeUnsafeRcBase_t * header = object;
            
            CERcType_t rcInfo = header->rcInfo;
            
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
            CERcType_t newRcInfo = weakBit | rc;
            header->rcInfo = newRcInfo;
        }
    }
    return object;
}
CERef _Nonnull __CETypeRetain(CERef _Nonnull object) {
    assert(object);
    
    return ___CETypeRetain(object, false);
}
CERef _Nullable __CETypeTryRetain(CERef _Nonnull object) {
    if (NULL == object) {
        return NULL;
    }
    return ___CETypeRetain(object, true);
}

void __CETypeRelease(CERef _Nonnull object) {
    assert(object);
    CETypeRef type = ((CERuntimeBase_t *)object)->type;
    assert(type);
    
    if (__builtin_expect(((type->masks & CETypeBitHasRc) == CETypeBitHasRc), true)) {
        if ((type->masks & CETypeBitRcAtomic) == CETypeBitRcAtomic) {
            CERuntimeAtomicRcBase_t * header = object;
            
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
                type->alloctor->deinit(object);
                //回收内存
                type->alloctor->deallocate(type, object);
            }
        } else {
            CERuntimeUnsafeRcBase_t * header = object;
            
            CERcType_t rcInfo = header->rcInfo;
            CERcType_t newRcInfo = 0;
            CERcType_t rc = rcInfo & CERuntimeRcMask;
            CERcType_t weakBit = 0;

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
            header->rcInfo = newRcInfo;
            if (0 == rc) {
                rc = CERuntimeRcDeallocing;
                newRcInfo = weakBit | rc;
                header->rcInfo = newRcInfo;

                //清除weak
                type->alloctor->destroyWeakRefrences(object);
                //deinit
                type->alloctor->deinit(object);
                //回收内存
                type->alloctor->deallocate(type, object);
            }
        }
    }
}


const CEAlloctor_s __CETypeDefaultAlloctor = {
    .context = NULL,
    .allocate = __CETypeAllocate,
    .deallocate = __CETypeDeallocate,
    .destroyWeakRefrences = __CETypeDestroyWeakRefrences,
    .retain = __CETypeMateRetain,
    .tryRetain = __CETypeMateTryRetain,
    .release = __CETypeMateRelease,
};

_Bool CETypeIsEqual(CETypeRef _Nonnull type0, CETypeRef _Nonnull type1) {
    assert(type0);
    assert(type1);
    return type0->identifier == type1->identifier;
}
CETypeRef _Nonnull CERefGetType(CERef _Nonnull ref) {
    CERuntimeBase_t * base = ref;
    CETypeRef type = base->type;
    assert(type);
    return type;
}
