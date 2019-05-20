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

#pragma pack(push)
#pragma pack(1)

typedef union _CEParamItemInlineValue {
    _Bool boolValue;
    int8_t sint8Value;
    uint8_t uint8Value;
    int16_t sint16Value;
    uint16_t uint16Value;
} CEParamItemInlineValue_u;

typedef struct _CEParamItem {
    uint32_t type: 6;
    uint32_t hasRelease: 1;
    uint32_t content: 25;
} CEParamItem_s;

#pragma pack(pop)



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
    uint32_t capacity: 8;
    uint32_t contentSize: 24;
    uint32_t count: 8;
    uint32_t contentUsedSize: 24;
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
    CERuntimeBase_t runtime;
    CEParamBase_t base;
    uint8_t itemsAndExt[0];//    CEParamItem_s items[base.bufferSize]; uint8_t content[];
} CEHeapParam_s;

#define CEHeapParamCheck(param, type)     assert(param);\
assert(type);\
assert(CETypeIsEqual(type, CETypeHeapParam));


#define CEHeapParamCheckCapacity(param)     assert(param);\
if (param->base.capacity >= param->base.count) {\
return false;\
}


#pragma mark - CEParamItem private methods

void _CEParamItemSetRange(CEParamItem_s * _Nonnull item, size_t location, size_t length) {
    uint32_t loc = (uint32_t)location;
    uint32_t len = (uint32_t)length;
    item->content = (loc << CEParamItemContentLengthBitCount) | len;
}
void _CEParamItemGetRange(CEParamItem_s * _Nonnull item, size_t * _Nonnull location, size_t * _Nonnull length) {
    uint32_t content = item->content;
    uint32_t loc = (content >> CEParamItemContentLengthBitCount) & CEParamItemContentLocationMask;
    uint32_t len = content & CEParamItemContentLengthMask;
    *location = (size_t)loc;
    *length = (size_t)len;
}

#pragma mark - CEStackParam private methods

_Bool _CEStackParamAppendInlineValue(CEStackParamRef _Nonnull stackParam, CEParamType_e t, CEParamItemInlineValue_u value) {
    CEStackParam_s * param = stackParam;
    CETypeRef type = param->runtime.type;
    CEStackParamCheck(param, type);
    CEStackParamCheckCapacity(param);
    
    CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
    uint8_t count = param->base.count;
    items[count].type = t;
    items[count].content = value.uint16Value;
    param->base.count += 1;
    
    return true;
}

_Bool _CEStackParamAppendContentValue(CEStackParamRef _Nonnull stackParam, CEParamType_e t, void * _Nonnull value, size_t size) {
    assert(size > 2 || CEParamTypeBuffer == t);
    
    CEStackParam_s * param = stackParam;
    CETypeRef type = param->runtime.type;
    CEStackParamCheck(param, type);
    CEStackParamCheckCapacity(param);
    
    if (size > CEParamBufferItemSizeMax) {
        return false;
    }
    
    CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
    uint8_t * ext = param->itemsAndExt + sizeof(CEParamItem_s) * param->base.count;

    uint8_t * target = NULL;

    size_t offset = ((param->base.contentUsedSize + sizeof(void *) -1) / sizeof(void *) * sizeof(void *));
    target = ext + offset;

    if (offset + size > param->base.contentSize) {
        return false;
    }

    items[param->base.count].type = t;
    memcpy(target, value, size);
    param->base.contentUsedSize += size;

    _CEParamItemSetRange(&items[param->base.count], offset, size);
    param->base.count += 1;
    return true;
}

_Bool _CEStackParamGetInlineValue(CEStackParamRef _Nonnull stackParam, uint32_t index, CEParamType_e t, CEParamItemInlineValue_u * _Nonnull valuePtr) {
    CEStackParam_s * param = stackParam;
    if (index < param->base.count) {
        CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
        CEParamType_e itemType = items[index].type;
        if (itemType != t) {
            return false;
        } else {
            uint32_t content = items[index].content;
            valuePtr->uint16Value = (uint16_t)content;
            return true;
        }
    } else {
        return false;
    }
}

_Bool _CEStackParamGetContentValue(CEStackParamRef _Nonnull stackParam, uint32_t index, CEParamType_e t, void * _Nonnull valuePtr, size_t size) {
    CEStackParam_s * param = stackParam;
    if (index < param->base.count) {
        CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
        CEParamType_e itemType = items[index].type;
        if (itemType != t) {
            return false;
        } else {
            size_t location = 0;
            size_t length = 0;
            _CEParamItemGetRange(&items[index], &location, &length);
            
            if (length != size) {
                return false;
            }
            uint8_t * target = param->itemsAndExt + location;
            memcpy(valuePtr, target, length);
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
    memset(ptr, 0, size);
    memcpy(ptr, &CETypeStackParam, sizeof(void *));
    
    CEStackParam_s * result = ptr;
    result->base.capacity = capacity;
    
    size_t contentSize = size - sizeof(CEStackParam_s);
    if (contentSize > 0xFFFFFFu) {
        result->base.contentSize = 0xFFFFFFu;
    } else {
        result->base.contentSize = (uint32_t)contentSize;
    }
    result->base.contentUsedSize = capacity * sizeof(CEParamItem_s);
    return result;
}

_Bool CEStackParamAppendBool(CEStackParamRef _Nonnull stackParam, _Bool item) {
    CEParamItemInlineValue_u value = {
        .boolValue = item,
    };
    return _CEStackParamAppendInlineValue(stackParam, CEParamTypeBool, value);
}
_Bool CEStackParamAppendSInt8(CEStackParamRef _Nonnull stackParam, int8_t item) {
    CEParamItemInlineValue_u value = {
        .sint8Value = item,
    };
    return _CEStackParamAppendInlineValue(stackParam, CEParamTypeSInt8, value);
}
_Bool CEStackParamAppendUInt8(CEStackParamRef _Nonnull stackParam, uint8_t item) {
    CEParamItemInlineValue_u value = {
        .uint8Value = item,
    };
    return _CEStackParamAppendInlineValue(stackParam, CEParamTypeUInt8, value);
}
_Bool CEStackParamAppendSInt16(CEStackParamRef _Nonnull stackParam, int16_t item) {
    CEParamItemInlineValue_u value = {
        .sint16Value = item,
    };
    return _CEStackParamAppendInlineValue(stackParam, CEParamTypeSInt16, value);
}
_Bool CEStackParamAppendUInt16(CEStackParamRef _Nonnull stackParam, uint16_t item) {
    CEParamItemInlineValue_u value = {
        .uint16Value = item,
    };
    return _CEStackParamAppendInlineValue(stackParam, CEParamTypeUInt16, value);
}
_Bool CEStackParamAppendSInt32(CEStackParamRef _Nonnull stackParam, int32_t item) {
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeSInt32, value, sizeof(int32_t));
}
_Bool CEStackParamAppendUInt32(CEStackParamRef _Nonnull stackParam, uint32_t item) {
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeUInt32, value, sizeof(uint32_t));
}
_Bool CEStackParamAppendSInt64(CEStackParamRef _Nonnull stackParam, int64_t item) {
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeSInt64, value, sizeof(int64_t));
}
_Bool CEStackParamAppendUInt64(CEStackParamRef _Nonnull stackParam, uint64_t item) {
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeUInt64, value, sizeof(uint64_t));
}
_Bool CEStackParamAppendFloat(CEStackParamRef _Nonnull stackParam, float item) {
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeFloat, value, sizeof(float));
}
_Bool CEStackParamAppendDouble(CEStackParamRef _Nonnull stackParam, double item) {
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeDouble, value, sizeof(double));
}
_Bool CEStackParamAppendPtr(CEStackParamRef _Nonnull stackParam, void * _Nullable item) {
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypePtr, value, sizeof(void *));
}
_Bool CEStackParamAppendRef(CEStackParamRef _Nonnull stackParam, CERef _Nullable item) {
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeRef, value, sizeof(CERef));
}
_Bool CEStackParamAppendBuffer(CEStackParamRef _Nonnull stackParam, void * _Nonnull buffer, size_t size) {
    if (0 == size) {
        return false;
    }
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeBuffer, buffer, size);
}


#pragma mark - CEHeapParam private methods


/*
 typedef struct _CEHeapParam {
 CERuntimeBase_t runtime;
 CEParamBase_t base;
 uint8_t itemsAndExt[0];//    CEParamItem_s items[base.bufferSize]; uint8_t content[];
 } CEHeapParam_s;

 */
//, CEParamItemRelease_f _Nullable release


_Bool _CEHeapParamAppendInlineValue(CEHeapParamRef _Nonnull heapParam, CEParamType_e t, CEParamItemInlineValue_u value) {
    CEHeapParam_s * param = heapParam;
    CETypeRef type = param->runtime.type;
    CEHeapParamCheck(param, type);
    CEHeapParamCheckCapacity(param);
    
    CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
    uint8_t count = param->base.count;
    items[count].type = t;
    items[count].content = value.uint16Value;
    param->base.count += 1;
    return true;
}

_Bool _CEHeapParamAppendContentValue(CEHeapParamRef _Nonnull heapParam, CEParamType_e t, void * _Nonnull value, size_t size, CEParamItemRelease_f _Nullable release) {
    CEHeapParam_s * param = heapParam;
    CETypeRef type = param->runtime.type;
    CEHeapParamCheck(param, type);
    CEHeapParamCheckCapacity(param);
    
    if (size > CEParamBufferItemSizeMax) {
        return false;
    }
    
    CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
    uint8_t * ext = param->itemsAndExt + sizeof(CEParamItem_s) * param->base.count;
    uint8_t * target = NULL;
    size_t offset = ((param->base.contentUsedSize + sizeof(void *) -1) / sizeof(void *) * sizeof(void *));
    target = ext + offset;
    
    if (offset + size > param->base.contentSize) {
        return false;
    }
    
    items[param->base.count].type = t;
    memcpy(target, value, size);
    param->base.contentUsedSize += size;
    
    _CEParamItemSetRange(&items[param->base.count], offset, size);
    param->base.count += 1;
    return true;
}

CEParamItem_s * _Nullable _CEHeapParamGetParamItem(CEStackParamRef _Nonnull heapParam, uint32_t index, CEParamType_e t) {
    CEHeapParam_s * param = heapParam;
    if (index < param->base.count) {
        CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
        CEParamType_e itemType = items[index].type;
        if (itemType != t) {
            return NULL;
        } else {
            return items;
        }
    } else {
        return NULL;
    }
}

_Bool _CEHeapParamGetValue(CEHeapParam_s * _Nonnull heapParam, uint32_t index, CEParamType_e t, void * _Nonnull valuePtr, size_t size) {
    assert(heapParam);
    
    CEHeapParam_s * param = heapParam;
    if (index < param->base.count) {
        CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
        CEParamItem_s * item = &(items[index]);

        CEParamType_e itemType = item->type;
        if (itemType != t) {
            return false;
        } else {
            if (0 == item->hasRelease && itemType <= CEParamTypeUInt16) {
                CEParamItemInlineValue_u value = {
                    .uint16Value = (uint16_t)(item->content),
                };
                if (CEParamTypeBool == itemType) {
                    assert(size = sizeof(_Bool));
                    _Bool result = value.boolValue;
                    memcpy(valuePtr, &result, size);
                } else if (CEParamTypeSInt8 == itemType) {
                    assert(size = sizeof(int8_t));
                    int8_t result = value.sint8Value;
                    memcpy(valuePtr, &result, size);
                } else if (CEParamTypeUInt8 == itemType) {
                    assert(size = sizeof(uint8_t));
                    uint8_t result = value.uint8Value;
                    memcpy(valuePtr, &result, size);
                } else if (CEParamTypeSInt16 == itemType) {
                    assert(size = sizeof(int16_t));
                    int16_t result = value.sint16Value;
                    memcpy(valuePtr, &result, size);
                } else if (CEParamTypeUInt16 == itemType) {
                    assert(size = sizeof(uint16_t));
                    uint16_t result = value.uint16Value;
                    memcpy(valuePtr, &result, size);
                } else {
                    abort();
                }
                return true;
            } else {
                size_t location = 0;
                size_t length = 0;
                if (size != length) {
                    return false;
                } else {
                    _CEParamItemGetRange(&items[index], &location, &length);
                    memcpy(valuePtr, &(param->itemsAndExt[location]), size);
                    return true;
                }
            }
        }
    } else {
        return false;
    }
}

void _CEHeapParamDeinit(CERef _Nonnull heapParam) {
    assert(heapParam);
    CERuntimeBase_t * base = heapParam;
    CETypeRef type = base->type;
    assert(type);
    
    assert(CETypeIsEqual(type, CETypeHeapParam));
    
    
    CEHeapParam_s * param = heapParam;
    
    CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);

    for (uint32_t index=0; index<param->base.count; index++) {
        CEParamItem_s * item = &(items[index]);
        if (1 == item->hasRelease) {
            CEParamType_e itemType = item->type;
            size_t location = 0;
            size_t length = 0;
            _CEParamItemGetRange(&items[index], &location, &length);
            void * value = &(param->itemsAndExt[location]);
            CEParamItemRelease_f release = (*(CEParamItemRelease_f *)&(param->itemsAndExt[location + length]));
            assert(release);
            release(itemType, value);
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


_Bool CEHeapParamAppendBool(CEHeapParam_s * _Nonnull heapParam, _Bool item, CEParamItemRelease_f _Nullable release) {
    if (NULL == release) {
        CEParamItemInlineValue_u value = {
            .boolValue = item,
        };
        return _CEHeapParamAppendInlineValue(heapParam, CEParamTypeBool, value);
    } else {
        return _CEHeapParamAppendContentValue(heapParam, CEParamTypeBool, &item, sizeof(_Bool), release);
    }
}
_Bool CEHeapParamAppendSInt8(CEHeapParam_s * _Nonnull heapParam, int8_t item, CEParamItemRelease_f _Nullable release) {
    if (NULL == release) {
        CEParamItemInlineValue_u value = {
            .sint8Value = item,
        };
        return _CEHeapParamAppendInlineValue(heapParam, CEParamTypeSInt8, value);
    } else {
        return _CEHeapParamAppendContentValue(heapParam, CEParamTypeSInt8, &item, sizeof(int8_t), release);
    }
}
_Bool CEHeapParamAppendUInt8(CEHeapParam_s * _Nonnull heapParam, uint8_t item, CEParamItemRelease_f _Nullable release) {
    if (NULL == release) {
        CEParamItemInlineValue_u value = {
            .uint8Value = item,
        };
        return _CEHeapParamAppendInlineValue(heapParam, CEParamTypeUInt8, value);
    } else {
        return _CEHeapParamAppendContentValue(heapParam, CEParamTypeUInt8, &item, sizeof(uint8_t), release);
    }
}
_Bool CEHeapParamAppendSInt16(CEHeapParam_s * _Nonnull heapParam, int16_t item, CEParamItemRelease_f _Nullable release) {
    if (NULL == release) {
        CEParamItemInlineValue_u value = {
            .sint16Value = item,
        };
        return _CEHeapParamAppendInlineValue(heapParam, CEParamTypeSInt16, value);
    } else {
        return _CEHeapParamAppendContentValue(heapParam, CEParamTypeSInt16, &item, sizeof(int16_t), release);
    }
}
_Bool CEHeapParamAppendUInt16(CEHeapParam_s * _Nonnull heapParam, uint16_t item, CEParamItemRelease_f _Nullable release) {
    if (NULL == release) {
        CEParamItemInlineValue_u value = {
            .uint16Value = item,
        };
        return _CEHeapParamAppendInlineValue(heapParam, CEParamTypeUInt16, value);
    } else {
        return _CEHeapParamAppendContentValue(heapParam, CEParamTypeUInt16, &item, sizeof(uint16_t), release);
    }
}

_Bool CEHeapParamppendSInt32(CEHeapParam_s * _Nonnull heapParam, int32_t item, CEParamItemRelease_f _Nullable release) {
    return _CEHeapParamAppendContentValue(heapParam, CEParamTypeSInt32, &item, sizeof(int32_t), release);
}
_Bool CEHeapParamAppendUInt32(CEHeapParam_s * _Nonnull heapParam, uint32_t item, CEParamItemRelease_f _Nullable release) {
    return _CEHeapParamAppendContentValue(heapParam, CEParamTypeUInt32, &item, sizeof(uint32_t), release);
}
_Bool CEHeapParamAppendSInt64(CEHeapParam_s * _Nonnull heapParam, int64_t item, CEParamItemRelease_f _Nullable release) {
    return _CEHeapParamAppendContentValue(heapParam, CEParamTypeSInt64, &item, sizeof(int64_t), release);
}
_Bool CEHeapParamAppendUInt64(CEHeapParam_s * _Nonnull heapParam, uint64_t item, CEParamItemRelease_f _Nullable release) {
    return _CEHeapParamAppendContentValue(heapParam, CEParamTypeUInt64, &item, sizeof(uint64_t), release);
}
_Bool CEHeapParamAppendFloat(CEHeapParam_s * _Nonnull heapParam, float item, CEParamItemRelease_f _Nullable release) {
    return _CEHeapParamAppendContentValue(heapParam, CEParamTypeFloat, &item, sizeof(float), release);
}
_Bool CEHeapParamAppendDouble(CEHeapParam_s * _Nonnull heapParam, double item, CEParamItemRelease_f _Nullable release) {
    return _CEHeapParamAppendContentValue(heapParam, CEParamTypeDouble, &item, sizeof(double), release);
}
_Bool CEHeapParamAppendPtr(CEHeapParam_s * _Nonnull heapParam, void * _Nullable item, CEParamItemRelease_f _Nullable release) {
    return _CEHeapParamAppendContentValue(heapParam, CEParamTypePtr, &item, sizeof(void *), release);
}
_Bool CEHeapParamAppendRef(CEHeapParam_s * _Nonnull heapParam, CERef _Nullable item, CEParamItemRelease_f _Nullable release) {
    return _CEHeapParamAppendContentValue(heapParam, CEParamTypePtr, &item, sizeof(CERef), release);
}
_Bool CEHeapParamAppendBuffer(CEHeapParam_s * _Nonnull heapParam, void * _Nonnull buffer, size_t size, CEParamItemRelease_f _Nullable release) {
    if (0 == size) {
        return false;
    }
    return _CEHeapParamAppendContentValue(heapParam, CEParamTypePtr, buffer, size, release);
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


_Bool CEParamGetBool(CEParamRef _Nonnull param, uint32_t index, _Bool * _Nonnull item) {
    assert(param);
    assert(item);
    
    CERuntimeBase_t * base = param;
    CETypeRef type = base->type;
    assert(type);
    CEParamItemInlineValue_u value = {};
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        
        if (_CEStackParamGetInlineValue(stackParam, index, CEParamTypeBool, &value)) {
            *item = value.boolValue;
            return true;
        } else {
            return false;
        }
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeBool, item, sizeof(_Bool));
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
    CEParamItemInlineValue_u value = {};
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        
        if (_CEStackParamGetInlineValue(stackParam, index, CEParamTypeSInt8, &value)) {
            *item = value.sint8Value;
            return true;
        } else {
            return false;
        }
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeSInt8, item, sizeof(int8_t));
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
    CEParamItemInlineValue_u value = {};
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        
        if (_CEStackParamGetInlineValue(stackParam, index, CEParamTypeUInt8, &value)) {
            *item = value.uint8Value;
            return true;
        } else {
            return false;
        }
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeUInt8, item, sizeof(uint8_t));
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
    CEParamItemInlineValue_u value = {};
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        
        if (_CEStackParamGetInlineValue(stackParam, index, CEParamTypeSInt16, &value)) {
            *item = value.sint16Value;
            return true;
        } else {
            return false;
        }
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeSInt16, item, sizeof(int16_t));
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
    CEParamItemInlineValue_u value = {};
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        
        if (_CEStackParamGetInlineValue(stackParam, index, CEParamTypeUInt16, &value)) {
            *item = value.uint16Value;
            return true;
        } else {
            return false;
        }
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeUInt16, item, sizeof(uint16_t));
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
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, CEParamTypeSInt32, item, sizeof(int32_t));
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeSInt32, item, sizeof(int32_t));
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
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, CEParamTypeUInt32, item, sizeof(uint32_t));
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeUInt32, item, sizeof(uint32_t));
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
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, CEParamTypeSInt64, item, sizeof(int64_t));
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeSInt64, item, sizeof(int64_t));
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
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, CEParamTypeUInt64, item, sizeof(uint64_t));
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeUInt64, item, sizeof(uint64_t));
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
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, CEParamTypeFloat, item, sizeof(float));
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeFloat, item, sizeof(float));
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
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, CEParamTypeDouble, item, sizeof(double));
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeDouble, item, sizeof(double));
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
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, CEParamTypePtr, item, sizeof(void *));
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypePtr, item, sizeof(void *));
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
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, CEParamTypeRef, item, sizeof(CERef));
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeRef, item, sizeof(CERef));
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
    
    if (CETypeIsEqual(type, CETypeStackParam)) {
        CEStackParam_s * stackParam = param;
        return _CEStackParamGetContentValue(stackParam, index, CEParamTypeBuffer, buffer, size);
    } else if (CETypeIsEqual(type, CETypeHeapParam)) {
        CEHeapParam_s * heapParam = param;
        return _CEHeapParamGetValue(heapParam, index, CEParamTypeBuffer, buffer, size);
    } else {
        abort();
        return false;
    }
}
