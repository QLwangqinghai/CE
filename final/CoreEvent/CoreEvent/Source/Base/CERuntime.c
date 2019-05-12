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
    .retain = __CETypeMateRetain,
    .tryRetain = __CETypeMateTryRetain,
    .release = __CETypeMateRelease,
};


size_t __CETypeMateGetSize(CETypeBase_s * _Nonnull type, CERef _Nonnull object) {
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
    .init = __CETypeMateInit,
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



//CMInfoStorage_s * _Nullable CMReferenceCountAdd(CMInfoStorage_s * _Nonnull header) {
//    assert(header);
//    CMActiveInfo_s activeInfo = {};
//    CMActiveInfo_s newValue = {};
//
//    do {
//        activeInfo = CMGetActiveInfo(header);
//        if (activeInfo.deallocing == 1) {//dealloc obj
//            printf("CPlus error, retain an dealloc obj.\n");
//            return NULL;
//        } else if (activeInfo.prepareDealloc == 1) {//prepare dealloc obj
//            return NULL;
//        } else if (activeInfo.refrenceCount == CMRefrenceStaticFlag) {//static obj
//            return header;
//        }
//        newValue = activeInfo;
//        newValue.refrenceCount += 1;
//    } while (!CMCASSetActiveInfo(header, activeInfo, newValue));
//    return header;
//}
//
//CMActiveInfo_s CMReferenceCountSubtract(CMInfoStorage_s * _Nonnull header) {
//    assert(header);
//    CMActiveInfo_s activeInfo = {};
//    CMActiveInfo_s newValue = {};
//    do {
//        activeInfo = CMGetActiveInfo(header);
//        if (activeInfo.deallocing == 1) {//dealloc obj
//            printf("CPlus error, release an dealloc obj.\n");
//            abort();
//            return activeInfo;
//        } else if (activeInfo.prepareDealloc == 1) {//prepare dealloc obj
//            printf("CPlus error, release an prepare dealloc obj.\n");
//            abort();
//            return activeInfo;
//        } else if (activeInfo.refrenceCount == CMRefrenceStaticFlag) {//static obj
//            return activeInfo;
//        }
//        if (activeInfo.refrenceCount == 0) {
//            printf("CPlus error, release an error obj.\n");
//            abort();
//            return activeInfo;
//        } else {
//            newValue = activeInfo;
//            newValue.refrenceCount -= 1;
//
//            if (activeInfo.autoDealloc == 1 && newValue.refrenceCount == 0) {
//                newValue.prepareDealloc = 1;
//            }
//        }
//    } while (!CMCASSetActiveInfo(header, activeInfo, newValue));
//    return newValue;
//}

CERef _Nonnull ___CETypeRetain(CERef _Nonnull object, _Bool tryR) {
    assert(object);
    CETypeBaseRef type = ((CERuntimeBase_t *)object)->type;
    assert(type);
    
    if ((type->masks & CETypeBitHasRc) == CETypeBitHasRc) {
        if ((type->masks & CETypeBitRcAtomic) == CETypeBitRcAtomic) {
            CERuntimeAtomicRcBase_t * header = object;
            
            CERcType_t rc = 0;
            CERcType_t newRc = 0;

            do {
                rc = atomic_load(&(header->rcInfo));
                if (__builtin_expect((rc < CERuntimeRcStatic), true)) {
                    newRc = rc + 1;
                    
                } else {
                    if (rc == CERuntimeRcStatic) {
                        return object;
                    } else if (rc == CERuntimeRcDeiniting) {
                    
                    
                    } else if (rc == CERuntimeRcDeallocing) {
                        
                        
                    }
                    if (__builtin_expect((tryR && (rc < CERuntimeRcStatic)), true)) {
                        newRc = rc + 1;
                        
                    } else {
                        
                        
                        
                    }
                    
                    
                }

                
                activeInfo = CMGetActiveInfo(header);
                if (activeInfo.deallocing == 1) {//dealloc obj
                    printf("CPlus error, release an dealloc obj.\n");
                    abort();
                    return activeInfo;
                } else if (activeInfo.prepareDealloc == 1) {//prepare dealloc obj
                    printf("CPlus error, release an prepare dealloc obj.\n");
                    abort();
                    return activeInfo;
                } else if (activeInfo.refrenceCount == CMRefrenceStaticFlag) {//static obj
                    return activeInfo;
                }
                if (activeInfo.refrenceCount == 0) {
                    printf("CPlus error, release an error obj.\n");
                    abort();
                    return activeInfo;
                } else {
                    newValue = activeInfo;
                    newValue.refrenceCount -= 1;
        
                    if (activeInfo.autoDealloc == 1 && newValue.refrenceCount == 0) {
                        newValue.prepareDealloc = 1;
                    }
                }
            } while (!CMCASSetActiveInfo(header, activeInfo, newValue));
                
            } else {
                
            }
            
            
#if CEBuild64Bit
            uint64_t rc =
            
#define CERuntimeRcStatic 0xFFFFFFFDull
#define CERuntimeRcDeiniting 0xFFFFFFFEull
#define CERuntimeRcDeallocing 0xFFFFFFFFull
            
#else
            uint32_t rc = atomic_load(&(object->rcInfo));
            return CMActiveInfoBridgeFrom(content);
#endif
        } else {
            
        }
    }
    
    return object;
}


CERef _Nonnull __CETypeRetain(CERef _Nonnull object) {
    assert(object);
    CETypeBaseRef type = ((CERuntimeBase_t *)object)->type;
    assert(type);
    
    if ((type->masks & CETypeBitHasRc) == CETypeBitHasRc) {
        if ((type->masks & CETypeBitRcAtomic) == CETypeBitRcAtomic) {
            CERuntimeAtomicRcBase_t * header = object;
            
            CERcType_t rc = atomic_load(&(header->rcInfo));
            
            if (rc < CERuntimeRcStatic) {
                
                
            } else {
                
            }

            
#if CEBuild64Bit
            uint64_t rc =
            
#define CERuntimeRcStatic 0xFFFFFFFDull
#define CERuntimeRcDeiniting 0xFFFFFFFEull
#define CERuntimeRcDeallocing 0xFFFFFFFFull

#else
            uint32_t rc = atomic_load(&(object->rcInfo));
            return CMActiveInfoBridgeFrom(content);
#endif
        } else {
            
        }
    }
    
    return object;
}
CERef _Nullable __CETypeTryRetain(CERef _Nonnull object) {
    assert(object);
    return object;
}
void __CETypeRelease(CERef _Nonnull object) {
    assert(object);
}


const CEAlloctor_s __CETypeDefaultAlloctor = {
    .context = NULL,
    .allocate = __CETypeMateAllocate,
    .deallocate = __CETypeMateDeallocate,
    .retain = __CETypeMateRetain,
    .tryRetain = __CETypeMateTryRetain,
    .release = __CETypeMateRelease,
};
