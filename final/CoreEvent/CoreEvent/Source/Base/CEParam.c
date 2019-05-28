//
//  CEParam.c
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEParam.h"

#include "CEMemory.h"


#pragma mark - private types

typedef union _CEParamItemContent {
    CERef _Nullable ref;
    CEPtr _Nullable ptr;
    int64_t sint64Value;
    uint64_t uint64Value;
    double doubleValue;
} CEParamItemContent_u;


size_t _CEStackParamGetSize(CERef _Nonnull p);
size_t _CEHeapParamGetSize(CERef _Nonnull p);
void _CEHeapParamDeinit(CERef _Nonnull heapParam);

const CETypeSpecific_s CETypeSpecificStackParam = {
    .name = "CEStackParam",
    .descript = CETypeDefaultDescript,
    .deinit = CERuntimeDefaultDeinit,
};

const CEType_s __CETypeStackParam = CEType(CETypeBitHasRc | CETypeBitStatic, 0, (uintptr_t)(&__CETypeStackParam), &CETypeSpecificStackParam);


const CETypeSpecific_s CETypeSpecificHeapParam = {
    .name = "CEHeapParam",
    .descript = CETypeDefaultDescript,
    .deinit = _CEHeapParamDeinit,
};
const CEType_s __CETypeHeapParam = CEType(CETypeBitHasRc, 0, (uintptr_t)(&__CETypeStackParam), &CETypeSpecificHeapParam);

CETypeRef _Nonnull CETypeStackParam = &__CETypeStackParam;
CETypeRef _Nonnull CETypeHeapParam = &__CETypeHeapParam;


#pragma pack(push)
#pragma pack(1)

//typedef struct _CEParamItem {
//    uint32_t type: 8;
//    uint32_t location: 24;
//} CEParamItem_s;


typedef struct _CEParamBase {
    uint32_t capacity: 6;
    uint32_t count: 6;
    uint32_t error: 20;
    uint16_t contentSize;
    uint16_t contentUsedSize;
    uint8_t types[CEParamItemMaxCount];
    uint16_t locations[CEParamItemMaxCount];
//    CEParamDeinitHandle_f _Nullable beforeDeinit;
} CEParamBase_t;

#pragma pack(pop)


typedef struct _CEParam {
    CERuntimeAtomicRcBase_s runtime;
    CEParamBase_t base;
    uint8_t itemsAndExt[0];
} CEParam_s;


#define CEParamCheckCapacity(param)     assert(param);\
if (param->base.capacity >= param->base.count) {\
return false;\
}


#define CEStackParamCheck(param, type)     assert(param);\
assert(type);\
assert(CETypeIsEqual(type, CETypeStackParam));


#define CEStackParamCheckCapacity(param)     assert(param);\
if (param->base.capacity >= param->base.count) {\
return false;\
}

#define CEHeapParamCheck(param, type)     assert(param);\
assert(type);\
assert(CETypeIsEqual(type, CETypeHeapParam));

#define CEHeapParamAppendCheck CEParam_s * heapParam = paramRef;\
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

_Bool __CEParamAppendContentValue(CEParamRef _Nonnull paramRef, CEParamType_e t, void * _Nonnull value) {
    CEParam_s * param = paramRef;
    CEParamCheckCapacity(param);
    
    uint16_t size = CEParamTypeGetSize(t);
    
    uint16_t offset = param->base.contentUsedSize;
    uint8_t * content = &(param->itemsAndExt[offset]);
    
    if (offset + size > param->base.contentSize) {
        return false;
    }
    if (CEParamTypeRef == t) {
        CERef ref = *((CERef *)value);
        CERetain(ref);
    }
    
    uint32_t index = param->base.count;
    
    if (size > 0) {
        memcpy(content, value, size);
    }
    param->base.types[index] = t;
    param->base.locations[index] = offset;

    uint16_t goodSizeMask = ~(3);
    uint16_t goodSize = size + 3;
    goodSize = goodSize & goodSizeMask;
    
    param->base.contentUsedSize += goodSize;
    param->base.count += 1;
    return true;
}

_Bool __CEParamGetContentValue(CEParamRef _Nonnull paramRef, uint32_t index, CEParamType_e t, void * _Nonnull valuePtr) {
    CEParam_s * param = paramRef;
    if (index < param->base.count) {
        if (t != param->base.types[index]) {
            return false;
        } else {
            size_t size = CEParamTypeGetSize(t);
            if (size > 0) {
                uint8_t * target = &(param->itemsAndExt[param->base.locations[index]]);
                memcpy(valuePtr, target, size);
            }
            return true;
        }
    } else {
        return false;
    }
}


_Bool __CEParamGetItem(CEParamRef _Nonnull paramRef, uint32_t index, CEParamType_e * _Nullable typePtr, size_t * _Nullable sizePtr, void * _Nonnull ptr, size_t maxSize) {
    CEParam_s * param = paramRef;
    if (index < param->base.count) {
        CEParamType_e t = param->base.types[index];
        size_t size = CEParamTypeGetSize(t);
        if (maxSize < size) {
            return false;
        } else {
            uint8_t * target = &(param->itemsAndExt[param->base.locations[index]]);
            memcpy(ptr, target, size);
            if (typePtr) {
                *typePtr = t;
            }
            if (sizePtr) {
                *sizePtr = size;
            }
            return true;
        }
    } else {
        return false;
    }
}

_Bool _CEStackParamAppendContentValue(CEStackParamRef _Nonnull stackParam, CEParamType_e t, void * _Nonnull value) {
    return __CEParamAppendContentValue(stackParam, t, value);
}

_Bool _CEStackParamGetContentValue(CEStackParamRef _Nonnull stackParam, uint32_t index, CEParamType_e t, void * _Nonnull valuePtr) {
    return __CEParamGetContentValue(stackParam, index, t, valuePtr);
}

_Bool _CEStackParamGetItem(CEStackParamRef _Nonnull stackParam, uint32_t index, CEParamType_e * _Nullable type, size_t * _Nullable size, void * _Nonnull ptr, size_t maxSize) {
    return __CEParamGetItem(stackParam, index, type, size, ptr, maxSize);
}

_Bool _CEHeapParamAppendContentValue(CEHeapParamRef _Nonnull heapParam, CEParamType_e t, void * _Nonnull value) {
    return __CEParamAppendContentValue(heapParam, t, value);
}

_Bool _CEHeapParamGetContentValue(CEHeapParamRef _Nonnull heapParam, uint32_t index, CEParamType_e t, void * _Nonnull valuePtr) {
    return __CEParamGetContentValue(heapParam, index, t, valuePtr);
}

_Bool _CEHeapParamGetItem(CEHeapParamRef _Nonnull heapParam, uint32_t index, CEParamType_e * _Nullable type, size_t * _Nullable size, void * _Nonnull ptr, size_t maxSize) {
    return __CEParamGetItem(heapParam, index, type, size, ptr, maxSize);
}

size_t _CEStackParamGetSize(CERef _Nonnull p) {
    CEParam_s * param = p;
    CETypeRef type = param->runtime.type;
    CEStackParamCheck(param, type);
    CEStackParamCheckCapacity(param);
    
    return sizeof(CEParam_s) + param->base.contentSize;
}

void _CEHeapParamDeinit(CERef _Nonnull heapParam) {
    assert(heapParam);
    CERuntimeBase_s * base = heapParam;
    CETypeRef type = base->type;
    assert(type);
    assert(CETypeIsEqual(type, CETypeHeapParam));
    
    CEParam_s * param = heapParam;
    for (uint32_t index=0; index<param->base.count; index++) {
        if (param->base.types[index] != CEParamTypeRef) {
            continue;
        }
        CERef ref = NULL;
        memcpy(&ref, &(param->itemsAndExt[param->base.locations[index]]), sizeof(CERef));
        if (ref) {
            CERelease(ref);
        }
    }
}


size_t _CEHeapParamGetSize(CERef _Nonnull p) {
    CEParam_s * param = p;
    CETypeRef type = param->runtime.type;
    CEHeapParamCheck(param, type);
    CEHeapParamCheckCapacity(param);
    
    return sizeof(CEParam_s) + param->base.contentSize;
}

CERef _Nullable __CEParamInit(CEParam_s * _Nonnull ptr, size_t size, uint32_t capacity) {
    CEParam_s * result = ptr;
    result->base.capacity = capacity;
    result->base.count = 0;
    result->base.error = 0;
    memset(result->base.types, 0, sizeof(uint8_t[CEParamItemMaxCount]));
    size_t contentSize = size - sizeof(CEParam_s);
    if (contentSize > UINT16_MAX) {
        result->base.contentSize = UINT16_MAX;
    } else {
        result->base.contentSize = (uint16_t)contentSize;
    }
    result->base.contentUsedSize = 0;
    return result;
}


#pragma mark - CEStackParam public api

size_t __CEParamGoodSize(uint32_t capacity, size_t bufferItemsTotalSize) {
    if (capacity > CEParamItemMaxCount) {
        capacity = CEParamItemMaxCount;
    }
    if (bufferItemsTotalSize > CEParamBufferItemsTotalSizeMax) {
        bufferItemsTotalSize = CEParamBufferItemsTotalSizeMax;
    }
    
    uint32_t size = (uint32_t)(sizeof(CEParam_s) + (sizeof(CEParamItemContent_u)) * capacity + bufferItemsTotalSize);
    uint32_t goodSizeMask = ~(3u);
    uint32_t goodSize = (uint32_t)size + 3;
    goodSize = goodSize & goodSizeMask;
    
    return goodSize;
}

size_t CEStackParamGoodSize(uint32_t capacity, size_t bufferItemsTotalSize) {
    return __CEParamGoodSize(capacity, bufferItemsTotalSize);
}

CEStackParamRef _Nullable CEStackParamInit(void * _Nonnull ptr, size_t size, uint32_t capacity) {
    if (NULL == ptr) {
        return NULL;
    }
    
    memcpy(ptr, &CETypeStackParam, sizeof(void *));
    return __CEParamInit(ptr, size, capacity);
}

void CEStackParamDeinit(CEStackParamRef _Nonnull heapParam) {
    assert(heapParam);
    
    CERuntimeBase_s * base = heapParam;
    CETypeRef type = base->type;
    assert(type);
    assert(CETypeIsEqual(type, CETypeStackParam));
    
    CEParam_s * param = heapParam;
    for (uint32_t index=0; index<param->base.count; index++) {
        if (param->base.types[index] != CEParamTypeRef) {
            continue;
        }
        CERef ref = NULL;
        memcpy(&ref, &(param->itemsAndExt[param->base.locations[index]]), sizeof(CERef));
        if (ref) {
            CERelease(ref);
        }
    }
}

#pragma mark - CEHeapParam public api

CEHeapParamRef _Nonnull CEHeapParamCreate(uint32_t capacity, size_t bufferItemsTotalSize) {
    size_t size = __CEParamGoodSize(capacity, bufferItemsTotalSize);
    CEHeapParamRef ptr = CETypeHeapParam->alloctor->allocate(CETypeHeapParam, size);
    return __CEParamInit(ptr, size, capacity);
}


#pragma mark - CEParam public api

uint32_t CEParamGetCount(CEParamRef _Nonnull param) {
    assert(param);
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return stackParam->base.count;
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return heapParam->base.count;
    } else {
        abort();
        return 0;
    }
}
_Bool CEParamGetItemType(CEParamRef _Nonnull param, uint32_t index, CEParamType_e * _Nonnull itemType) {
    assert(param);
    assert(itemType);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        if (index < stackParam->base.count) {
            CEParamType_e t = stackParam->base.types[index];
            *itemType = t;
            return true;
        } else {
            return false;
        }
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        if (index < heapParam->base.count) {
            CEParamType_e t = heapParam->base.types[index];
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

    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeBool;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}


_Bool CEParamGetSInt8(CEParamRef _Nonnull param, uint32_t index, int8_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt8;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetSInt16(CEParamRef _Nonnull param, uint32_t index, int16_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt16;

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetSInt32(CEParamRef _Nonnull param, uint32_t index, int32_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt32;

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetSInt64(CEParamRef _Nonnull param, uint32_t index, int64_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt64;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamGetUInt8(CEParamRef _Nonnull param, uint32_t index, uint8_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt8;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetUInt16(CEParamRef _Nonnull param, uint32_t index, uint16_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt16;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetUInt32(CEParamRef _Nonnull param, uint32_t index, uint32_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt32;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetUInt64(CEParamRef _Nonnull param, uint32_t index, uint64_t * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt64;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamGetFloat(CEParamRef _Nonnull param, uint32_t index, float * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeFloat;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetDouble(CEParamRef _Nonnull param, uint32_t index, double * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeDouble;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamGetPtr(CEParamRef _Nonnull param, uint32_t index, void * _Nullable * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypePtr;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetRef(CEParamRef _Nonnull param, uint32_t index, CERef _Nullable * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeRef;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamGetBuffer(CEParamRef _Nonnull param, uint32_t index, void * _Nonnull buffer, size_t size) {
    assert(param);
    assert(buffer);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    
    CEParamType_e itemType = CEParamTypeNone;
    
    if (!CEParamTypeFromBufferSize(size, &itemType)) {
        return false;
    }
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, itemType, buffer);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetContentValue(heapParam, index, itemType, buffer);
    } else {
        abort();
        return false;
    }
}


_Bool CEParamGetItem(CEParamRef _Nonnull param, uint32_t index, CEParamType_e * _Nullable typePtr, size_t * _Nullable sizePtr, void * _Nonnull ptr, size_t maxSize) {
    assert(param);
    assert(ptr);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    
    
    CEParamType_e itemType = CEParamTypeNone;
    CEParamGetItemType(param, index, &itemType);

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamGetItem(stackParam, index, typePtr, sizePtr, ptr, maxSize);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamGetItem(heapParam, index, typePtr, sizePtr, ptr, maxSize);
    } else {
        abort();
        return false;
    }
}

//append


_Bool CEParamAppendBool(CEParamRef _Nonnull param, _Bool item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeBool;

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}


_Bool CEParamAppendSInt8(CEParamRef _Nonnull param, int8_t item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt8;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendSInt16(CEParamRef _Nonnull param, int16_t item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt16;

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendSInt32(CEParamRef _Nonnull param, int32_t item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt32;

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendSInt64(CEParamRef _Nonnull param, int64_t item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeSInt64;

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendUInt8(CEParamRef _Nonnull param, uint8_t item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt8;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendUInt16(CEParamRef _Nonnull param, uint16_t item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt16;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendUInt32(CEParamRef _Nonnull param, uint32_t item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt32;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendUInt64(CEParamRef _Nonnull param, uint64_t item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeUInt64;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendFloat(CEParamRef _Nonnull param, float item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeFloat;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}
_Bool CEParamAppendDouble(CEParamRef _Nonnull param, double item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeDouble;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendPtr(CEParamRef _Nonnull param, void * _Nullable item) {
    assert(param);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypePtr;
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendRef(CEParamRef _Nonnull param, CERef _Nullable item) {
    assert(param);

    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamType_e itemType = CEParamTypeRef;

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, &item);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, &item);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendBuffer(CEParamRef _Nonnull param, void * _Nonnull buffer, size_t size) {
    assert(param);
    assert(buffer);
    
    CEParamType_e itemType = CEParamTypeNone;
    if (!CEParamTypeFromBufferSize(size, &itemType)) {
        return false;
    }
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);

    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, itemType, buffer);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, itemType, buffer);
    } else {
        abort();
        return false;
    }
}

_Bool CEParamAppendItem(CEParamRef _Nonnull param, CEParamType_e t, void * _Nonnull itemPtr) {
    assert(param);
    assert(itemPtr);
    
    CERuntimeBase_s * base = param;
    CETypeRef type = base->type;
    assert(type);
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEParam_s * stackParam = param;
        return _CEStackParamAppendContentValue(stackParam, t, itemPtr);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEParam_s * heapParam = param;
        return _CEHeapParamAppendContentValue(heapParam, t, itemPtr);
    } else {
        abort();
        return false;
    }
    
}
