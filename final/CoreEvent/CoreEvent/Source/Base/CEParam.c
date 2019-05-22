//
//  CEParam.c
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEParam.h"

#include "CEMemory.h"


#pragma pack(push)
#pragma pack(1)

typedef struct _CEParamItem {
    uint32_t type: 6;
    uint32_t location: 15;
    uint32_t length: 11;
} CEParamItem_s;

#pragma pack(pop)


#pragma mark - private types

size_t _CEStackParamGetSize(CERef _Nonnull p);
size_t _CEHeapParamGetSize(CERef _Nonnull p);
void _CEHeapParamDeinit(CERef _Nonnull heapParam);


const CEType_s __CETypeStackParam = {
    .type = CETypeMate,
    .version = CERuntimeVersion,
    .masks = CETypeMaskNoRc,
    .objectSize = 0,
    .name = "CEStackParam",
    .identifier = (uintptr_t)(&__CETypeStackParam),
    .alloctor = &__CETypeDefaultAlloctor,
    
    .getSize = _CEStackParamGetSize,
    .deinit = CETypeDefaultDeinit,
    .descript = CETypeDefaultDescript,
};

const CEType_s __CETypeHeapParam = {
    .type = CETypeMate,
    .version = CERuntimeVersion,
    .masks = CETypeMaskRcAtomic,
    .objectSize = 0,
    .name = "CEHeapParam",
    .identifier = (uintptr_t)(&__CETypeHeapParam),
    .alloctor = &__CETypeDefaultAlloctor,
    
    .getSize = _CEHeapParamGetSize,
    .deinit = _CEHeapParamDeinit,
    .descript = CETypeDefaultDescript,
};


CETypeRef _Nonnull CETypeStackParam = &__CETypeStackParam;
CETypeRef _Nonnull CETypeHeapParam = &__CETypeHeapParam;


#pragma pack(push)
#pragma pack(1)

typedef struct _CEParamBase {
    uint32_t capacity: 6;
    uint32_t count: 6;
    uint32_t contentSize: 20;
    uint32_t error: 12;
    uint32_t contentUsedSize: 20;
    CERuntimeRetain_f _Nonnull refRetain;
    CERuntimeRelease_f _Nonnull refRelease;
} CEParamBase_t;

#pragma pack(pop)

typedef struct _CEStackParam {
    CERuntimeBase_t runtime;
    CEParamBase_t base;
    uint8_t itemsAndExt[0];
} CEStackParam_s;

#define CEStackParamCheck(param, type)     assert(param);\
assert(type);\
assert(CETypeIsEqual(type, CETypeStackParam));


#define CEStackParamCheckCapacity(param)     assert(param);\
if (param->base.capacity >= param->base.count) {\
return false;\
}


typedef struct _CEHeapParam {
    CERuntimeAtomicRcBase_t runtime;
    CEParamBase_t base;
    uint8_t itemsAndExt[0];//    CEParamItem_s items[base.bufferSize]; uint8_t content[];
} CEHeapParam_s;

#define CEHeapParamCheck(param, type)     assert(param);\
assert(type);\
assert(CETypeIsEqual(type, CETypeHeapParam));

#define CEHeapParamAppendCheck CEHeapParam_s * heapParam = paramRef;\
assert(heapParam);\
CETypeRef type = heapParam->runtime.type;\
assert(type);\
assert(CETypeIsEqual(type, CETypeHeapParam));\
if (heapParam->base.capacity >= heapParam->base.count) {\
    return false;\
}

#define CEHeapParamCheckCapacity(param)     assert(param);\
if (param->base.capacity >= param->base.count) {\
return false;\
}




#pragma mark - private methods

_Bool _CEStackParamAppendContentValue(CEStackParamRef _Nonnull stackParam, CEParamType_e t, void * _Nonnull value, size_t valueSize) {
    assert(valueSize >= 1);
    assert(t <= CEParamTypeMax);
    if (valueSize > CEParamBufferItemSizeMax) {
        return false;
    }
    
    CEStackParam_s * param = stackParam;
    CEStackParamCheckCapacity(param);
    
    uint32_t size = (uint32_t)valueSize;
    
    uint32_t offset = param->base.contentUsedSize;
    uint8_t * content = &(param->itemsAndExt[offset]);
    
    if (offset + size > param->base.contentSize) {
        return false;
    }
    
    CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
    CEParamItem_s * item = items + param->base.count;

    memcpy(content, value, size);
    item->type = t;
    item->length = size;
    item->location = offset;

    uint32_t goodSizeMask = ~(3u);
    uint32_t goodSize = (uint32_t)size + 3;
    goodSize = goodSize & goodSizeMask;
    
    param->base.contentUsedSize += goodSize;
    param->base.count += 1;
    return true;
}

_Bool _CEStackParamGetContentValue(CEStackParamRef _Nonnull stackParam, uint32_t index, CEParamType_e t, void * _Nonnull valuePtr, size_t size) {
    CEStackParam_s * param = stackParam;
    if (index < param->base.count) {
        CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
        CEParamItem_s item = items[index];
        if (item.type != t) {
            return false;
        } else {
            if (item.length != size) {
                return false;
            }
            uint8_t * target = &(param->itemsAndExt[item.location]);
            memcpy(valuePtr, target, item.length);
            return true;
        }
    } else {
        return false;
    }
}

_Bool _CEHeapParamAppendContentValue(CEHeapParamRef _Nonnull heapParam, CEParamType_e t, void * _Nonnull value, size_t valueSize) {
    assert(valueSize >= 1);
    assert(t <= CEParamTypeMax);
    if (valueSize > CEParamBufferItemSizeMax) {
        return false;
    }
    
    CEHeapParam_s * param = heapParam;
    CEHeapParamCheckCapacity(param);
    
    uint32_t size = (uint32_t)valueSize;
    
    uint32_t offset = param->base.contentUsedSize;
    uint8_t * content = &(param->itemsAndExt[offset]);
    
    if (offset + size > param->base.contentSize) {
        return false;
    }
    
    CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
    CEParamItem_s * item = items + param->base.count;
    
    memcpy(content, value, size);
    item->type = t;
    item->length = size;
    item->location = offset;
    
    uint32_t goodSizeMask = ~(3u);
    uint32_t goodSize = (uint32_t)size + 3;
    goodSize = goodSize & goodSizeMask;
    
    param->base.contentUsedSize += goodSize;
    param->base.count += 1;
    return true;
}


_Bool _CEHeapParamGetContentValue(CEHeapParamRef _Nonnull heapParam, uint32_t index, CEParamType_e t, void * _Nonnull valuePtr, size_t size) {
    CEHeapParam_s * param = heapParam;
    if (index < param->base.count) {
        CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
        CEParamItem_s item = items[index];
        if (item.type != t) {
            return false;
        } else {
            if (item.length != size) {
                return false;
            }
            uint8_t * target = &(param->itemsAndExt[item.location]);
            memcpy(valuePtr, target, item.length);
            return true;
        }
    } else {
        return false;
    }
}

size_t _CEStackParamGetSize(CERef _Nonnull p) {
    CEStackParam_s * param = p;
    CETypeRef type = param->runtime.type;
    CEStackParamCheck(param, type);
    CEStackParamCheckCapacity(param);
    
    return sizeof(CEStackParam_s) + param->base.contentSize;
}

void _CEHeapParamDeinit(CERef _Nonnull heapParam) {
    assert(heapParam);
    CERuntimeBase_t * base = heapParam;
    CETypeRef type = base->type;
    assert(type);
    assert(CETypeIsEqual(type, CETypeHeapParam));
    
    CEHeapParam_s * param = heapParam;
    CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
    CERuntimeRelease_f release = param->base.refRelease;
    
    if (release) {
        for (uint32_t index=0; index<param->base.count; index++) {
            CEParamItem_s item = items[index];
            if (item.type != CEParamTypeRef) {
                continue;
            }
            assert(item.length == item.length);
            CERef ref = NULL;
            memcpy(&ref, &(param->itemsAndExt[item.location]), sizeof(CERef));
            release(ref);
        }
    }
}


size_t _CEHeapParamGetSize(CERef _Nonnull p) {
    CEHeapParam_s * param = p;
    CETypeRef type = param->runtime.type;
    CEHeapParamCheck(param, type);
    CEHeapParamCheckCapacity(param);
    
    return sizeof(CEStackParam_s) + param->base.contentSize;
}

#pragma mark - CEStackParam public api

CEStackParamRef _Nullable CEStackParamInit(void * _Nonnull ptr, size_t size, uint32_t capacity) {
    if (NULL == ptr) {
        return NULL;
    }
    if (capacity > CEParamItemMaxCount) {
        return NULL;
    }
    
    size_t baseSize = sizeof(CEStackParam_s) + capacity * sizeof(CEParamItem_s);
    if (baseSize < sizeof(CEStackParam_s)) {
        return NULL;
    }
    memcpy(ptr, &CETypeStackParam, sizeof(void *));
    
    CEStackParam_s * result = ptr;
    result->base.capacity = capacity;
    result->base.count = 0;
    result->base.error = 0;
    
    size_t contentSize = size - sizeof(CEStackParam_s);
    if (contentSize > 0xFFFFFu) {
        result->base.contentSize = 0xFFFFFu;
    } else {
        result->base.contentSize = (uint32_t)contentSize;
    }
    result->base.contentUsedSize = capacity * sizeof(CEParamItem_s);
    return result;
}

#pragma mark - CEHeapParam public api


CEHeapParamRef _Nonnull CEHeapParamCreate(uint32_t capacity, size_t bufferItemsTotalSize) {
    assert(bufferItemsTotalSize <= CEParamBufferItemsTotalSizeMax);
    
    if (0 == bufferItemsTotalSize) {
        bufferItemsTotalSize = 64;
    }
    
    size_t size = sizeof(CEHeapParam_s) + (sizeof(CEParamItem_s) + sizeof(void *) + sizeof(double)) * capacity + bufferItemsTotalSize;
    size = (size + sizeof(void *) - 1) / sizeof(void *) * sizeof(void *);
    
    CEHeapParamRef ptr = CETypeHeapParam->alloctor->allocate(CETypeHeapParam, size);
    
    CEHeapParam_s * result = ptr;
    result->base.capacity = capacity;
    
    size_t contentSize = size - sizeof(CEStackParam_s);
    if (contentSize > 0xFFFFFFu) {
        result->base.contentSize = 0xFFFFFFu;
    } else {
        result->base.contentSize = (uint32_t)contentSize;
    }
    result->base.contentUsedSize = capacity * sizeof(CEParamItem_s);
    
    return ptr;
}



#pragma mark - CEParam public api

uint32_t CEParamGetCount(CEParamRef _Nonnull param) {
    assert(param);
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return stackParam->base.count;
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return heapParam->base.count;
    } else {
        abort();
        return 0;
    }
}
_Bool CEParamGetItemType(CEParamRef _Nonnull param, uint32_t index, CEParamType_e * _Nonnull itemType) {
    assert(param);
    assert(itemType);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        if (index < stackParam->base.count) {
            CEParamItem_s * items = (CEParamItem_s *)(stackParam->itemsAndExt);
            CEParamType_e t = items[index].type;
            *itemType = t;
            return true;
        } else {
            return false;
        }
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        if (index < heapParam->base.count) {
            CEParamItem_s * items = (CEParamItem_s *)(heapParam->itemsAndExt);
            CEParamType_e t = items[index].type;
            *itemType = t;
            return true;
        } else {
            return false;
        }
    } else {
        abort();
        return false;
    }
}

//get
_Bool CEParamGetBool(CEParamRef _Nonnull param, uint32_t index, _Bool * _Nonnull item) {
    assert(param);
    assert(item);

    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeBool;
    size_t itemSize = sizeof(_Bool);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}


_Bool CEParamGetSInt8(CEParamRef _Nonnull param, uint32_t index, int8_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt8;
    size_t itemSize = sizeof(int8_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetSInt16(CEParamRef _Nonnull param, uint32_t index, int16_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt16;
    size_t itemSize = sizeof(int16_t);

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetSInt32(CEParamRef _Nonnull param, uint32_t index, int32_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt32;
    size_t itemSize = sizeof(int32_t);

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetSInt64(CEParamRef _Nonnull param, uint32_t index, int64_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt64;
    size_t itemSize = sizeof(int64_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamGetUInt8(CEParamRef _Nonnull param, uint32_t index, uint8_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt8;
    size_t itemSize = sizeof(uint8_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetUInt16(CEParamRef _Nonnull param, uint32_t index, uint16_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt16;
    size_t itemSize = sizeof(uint16_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetUInt32(CEParamRef _Nonnull param, uint32_t index, uint32_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt32;
    size_t itemSize = sizeof(uint32_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetUInt64(CEParamRef _Nonnull param, uint32_t index, uint64_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt64;
    size_t itemSize = sizeof(uint64_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamGetFloat(CEParamRef _Nonnull param, uint32_t index, float * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeFloat;
    size_t itemSize = sizeof(float);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetDouble(CEParamRef _Nonnull param, uint32_t index, double * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeDouble;
    size_t itemSize = sizeof(double);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamGetPtr(CEParamRef _Nonnull param, uint32_t index, void * _Nullable * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypePtr;
    size_t itemSize = sizeof(CEPtr);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetRef(CEParamRef _Nonnull param, uint32_t index, CERef _Nullable * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeRef;
    size_t itemSize = sizeof(CERef);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetBuffer(CEParamRef _Nonnull param, uint32_t index, void * _Nonnull buffer, size_t size) {
    assert(param);
    assert(buffer);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeBuffer;
    size_t itemSize = size;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, buffer, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, buffer, itemSize);
    } else {
        abort();
        return false;
    }
}

//append


_Bool CEParamAppendBool(CEParamRef _Nonnull param, _Bool item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeBool;
    size_t itemSize = sizeof(_Bool);

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}


_Bool CEParamAppendSInt8(CEParamRef _Nonnull param, int8_t item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt8;
    size_t itemSize = sizeof(int8_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendSInt16(CEParamRef _Nonnull param, int16_t item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt16;
    size_t itemSize = sizeof(int16_t);

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendSInt32(CEParamRef _Nonnull param, int32_t item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt32;
    size_t itemSize = sizeof(int32_t);

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendSInt64(CEParamRef _Nonnull param, int64_t item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt64;
    size_t itemSize = sizeof(int64_t);

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendUInt8(CEParamRef _Nonnull param, uint8_t item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt8;
    size_t itemSize = sizeof(uint8_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendUInt16(CEParamRef _Nonnull param, uint16_t item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt16;
    size_t itemSize = sizeof(uint16_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendUInt32(CEParamRef _Nonnull param, uint32_t item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt32;
    size_t itemSize = sizeof(uint32_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendUInt64(CEParamRef _Nonnull param, uint64_t item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt64;
    size_t itemSize = sizeof(uint64_t);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendFloat(CEParamRef _Nonnull param, float item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeFloat;
    size_t itemSize = sizeof(float);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendDouble(CEParamRef _Nonnull param, double item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeDouble;
    size_t itemSize = sizeof(double);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendPtr(CEParamRef _Nonnull param, void * _Nullable item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypePtr;
    size_t itemSize = sizeof(CEPtr);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendRef(CEParamRef _Nonnull param, CERef _Nullable item) {
    assert(param);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeRef;
    size_t itemSize = sizeof(CERef);

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item, itemSize);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendBuffer(CEParamRef _Nonnull param, void * _Nonnull buffer, size_t size) {
    assert(param);
    assert(buffer);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeBuffer;
    size_t itemSize = size;

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, buffer, itemSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, buffer, itemSize);
    } else {
        abort();
        return false;
    }
}
