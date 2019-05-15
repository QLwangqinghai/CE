//
//  CERuntime.c
//  CoreEvent
//
//  Created by vector on 2019/5/11.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CERuntime.h"
#include "CEMemory.h"



void * _Nonnull __CETypeMateAllocate(CETypeBase_s const * _Nonnull type, size_t size) {
    CETypeBase_s * ptr = CEAllocateClear(size);
    *(CETypeBase_s const **)(&(ptr->type)) = type;
    return ptr;
}
void __CETypeMateDeallocate(CETypeBase_s const * _Nonnull type, void * _Nonnull ptr, size_t size) {
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


size_t __CETypeMateGetSize(CETypeBase_s const * _Nonnull type, CERef _Nonnull object) {
    return type->objectSize;
}
void __CETypeMateInit(CERef _Nonnull object) {
}
void __CETypeMateDeinit(CERef _Nonnull object) {
}

void __CETypeMateDescript(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript) {
    char buffer[1024] = {};
    snprintf(buffer, 1023, "<%s:%p>", ((CETypeBase_s *)object)->name, object);
    descript(handler, buffer);
}



CETypeBase_s __CETypeMate = {
    .type = &__CETypeMate,
    .version = 1,
    .masks = 0,
    .objectSize = CETypeBaseLayoutSize,
    .getSize = __CETypeMateGetSize,
    
    .alloctor = &__CETypeMateAlloctor,
    .deinit = __CETypeMateDeinit,
    
    .name = "CEType",
    .descript = __CETypeMateDescript,
    
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


void * _Nonnull __CETypeAllocate(CETypeBase_s const * _Nonnull type, size_t size) {
    void * object = CEAllocateClear(size);
    CETypeBase_s * ptr = object;
    *(CETypeBase_s const **)(&(ptr->type)) = type;
    
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
void __CETypeDeallocate(CETypeBase_s const * _Nonnull type, void * _Nonnull ptr, size_t size) {
    CEDeallocate(ptr);
}

void __CETypeDestroyWeakRefrences(CERef _Nonnull object) {
    assert(object);
}

static inline CERef _Nullable ___CETypeRetain(CERef _Nonnull object, _Bool tryR) {
    assert(object);
    CETypeBaseRef type = ((CERuntimeBase_t *)object)->type;
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
    CETypeBaseRef type = ((CERuntimeBase_t *)object)->type;
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
                size_t size = type->getSize(type, object);
                //清除weak
                type->alloctor->destroyWeakRefrences(object);
                //deinit
                type->deinit(object);
                //回收内存
                type->alloctor->deallocate(type, object, size);
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

                size_t size = type->getSize(type, object);
                //清除weak
                type->alloctor->destroyWeakRefrences(object);
                //deinit
                type->deinit(object);
                //回收内存
                type->alloctor->deallocate(type, object, size);
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
