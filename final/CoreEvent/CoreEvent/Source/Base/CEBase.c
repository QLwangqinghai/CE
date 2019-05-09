//
//  CEBase.c
//  CoreEvent
//
//  Created by CaiLianfeng on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#include "CEBase.h"
#include "CEMemory.h"



void * _Nonnull __CETypeMateAllocate(CETypeBase_s const * _Nonnull type, size_t size) {
    CETypeBase_s * ptr = CEAllocateClear(size);
    *(CETypeBase_s const **)(&(ptr->type)) = type;
    return ptr;
}
void __CETypeMateDeallocate(CETypeBase_s const * _Nonnull type, void * _Nonnull ptr, size_t size) {
    CEDeallocate(ptr);
}

CEAlloctor_s __CETypeMateAlloctor = {
    .allocate = __CETypeMateAllocate,
    .deallocate = __CETypeMateDeallocate,
    .context = NULL,
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

CERef _Nonnull __CETypeMateRetain(CERef _Nonnull object) {
    return object;
}
CERef _Nullable __CETypeMateTryRetain(CERef _Nonnull object) {
    return object;
}
void __CETypeMateRelease(CERef _Nonnull object) {
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
    
    .retain = &__CETypeMateRetain,
    .tryRetain = __CETypeMateTryRetain,
    .release = __CETypeMateRelease,
};


CERef _Nonnull CERetain(CERef _Nonnull object) {
    assert(object);
    return ((CEObjectRuntimeBase_t *)object)->type->retain(object);
}
CERef _Nullable CETryRetain(CERef _Nonnull object) {
    assert(object);
    return ((CEObjectRuntimeBase_t *)object)->type->tryRetain(object);
}
void CERelease(CERef _Nonnull object) {
    assert(object);
    return ((CEObjectRuntimeBase_t *)object)->type->release(object);
}
