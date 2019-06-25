//
//  CETaskParam.c
//  CoreEvent
//
//  Created by vector on 2019/6/15.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CETaskParam.h"


static const uint32_t CETaskParamItemLocationNone = 0xfff;


static const size_t CETaskParamTypeLoadSize[13] = {
    0, 0, 0, 0, 0, 0, sizeof(sint32_t), sizeof(uint32_t), sizeof(float_t), sizeof(CEPtr), sizeof(sint64_t), sizeof(uint64_t), sizeof(double_t),
};

static const size_t CETaskParamTypeValueSize[13] = {
    0, sizeof(bool_t), sizeof(sint8_t), sizeof(uint8_t), sizeof(sint16_t), sizeof(uint16_t), sizeof(sint32_t), sizeof(uint32_t), sizeof(float_t), sizeof(CEPtr), sizeof(sint64_t), sizeof(uint64_t), sizeof(double_t),
};

static inline size_t CETaskParamTypeGetValueSize(CType_e type) {
    if (type <= CTypeDouble) {
        return CETaskParamTypeValueSize[type];
    } else {
        return 0;
    }
}

static inline size_t CETaskParamTypeGetLoadSize(CType_e type, size_t length) {
    if (type <= CTypeDouble) {
        return CETaskParamTypeLoadSize[type];
    } else if (CTypeBuffer == type) {
        return length;
    } else {
        return 0;
    }
}

#pragma pack(push, 1)

typedef struct _CETaskParamItem {
    uint32_t type: 4;
    uint32_t location: 12;
    uint32_t ext: 16;
} CETaskParamItem_t;

#pragma pack(pop)

#define CETaskParamItemSize sizeof(CETaskParamItem_t)

typedef struct _CETaskParam {
    CERuntimeAtomicRcBase_s runtime;
    CETaskParamDestroyItems_f _Nullable destroy;
    int32_t code;
    uint32_t totalSize: 16;
    uint32_t contentSize: 16;//内容区占用的字节数
    uint32_t fieldSize: 16;
    uint32_t count: 16;
    uint8_t load[];
} CETaskParam_s;


extern CETypeRef _Nonnull CETypeTaskParamStack;
extern CETypeRef _Nonnull CETypeTaskParamHeap;


static inline CETaskParam_s * _Nullable CETaskParamCheckBridge(CERef _Nullable param) {
    if (NULL != param) {
        CETaskParam_s * result = param;
        CETypeRef type = result->runtime.type;
        
        if (CETypeIsEqual(type, CETypeTaskParamStack) || CETypeIsEqual(type, CETypeTaskParamHeap)) {
            return result;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}


void _CETaskParamHeapDeinit(CERef _Nonnull stackParam);

const CETypeSpecific_s CETypeSpecificTaskParamStack = {
    .name = "CETaskParamStack",
    .descript = CETypeDefaultDescript,
    .deinit = CERuntimeDefaultDeinit,
};

const CEType_s __CETypeTaskParamStack = CEType(CETypeBitHasRc | CETypeBitStatic, 0, (uintptr_t)(&__CETypeTaskParamStack), &CETypeSpecificTaskParamStack);


const CETypeSpecific_s CETypeSpecificTaskParamHeap = {
    .name = "CETaskParamHeap",
    .descript = CETypeDefaultDescript,
    .deinit = _CETaskParamHeapDeinit,
};
const CEType_s __CETypeTaskParamHeap = CEType(CETypeBitHasRc, 0, (uintptr_t)(&__CETypeTaskParamHeap), &CETypeSpecificTaskParamHeap);

CETypeRef _Nonnull CETypeTaskParamStack = &__CETypeTaskParamStack;
CETypeRef _Nonnull CETypeTaskParamHeap = &__CETypeTaskParamHeap;


void _CETaskParamHeapDeinit(CERef _Nonnull heapParam) {
    assert(heapParam);
    CETaskParam_s * param = heapParam;
    CETypeRef type = param->runtime.type;
    assert(CETypeIsEqual(type, CETypeTaskParamHeap));
    CETaskParamDestroyItems_f destroy = param->destroy;
    if (destroy) {
        destroy(param);
    }
}

#pragma mark - private methods
static inline CETaskParamItem_t * _Nullable _CETaskParamGetItem(CETaskParam_s * _Nonnull param, uint32_t index) {
    if (param->count <= index) {
        return NULL;
    }
    
    CETaskParamItem_t * item = (CETaskParamItem_t *)(param->load) + index;
    return item;
}

int32_t __CETaskParamAppendContentValue(CETaskParamRef _Nonnull paramRef, CType_e t, void * _Nonnull value, size_t length) {
    CETaskParam_s * param = CETaskParamCheckBridge(paramRef);
    if (NULL == param || NULL == value) {
        return CETaskParamErrorInput;
    }
    if (!CTypeIsValid(t)) {
        return CETaskParamErrorType;
    }
    int32_t totalSize = param->totalSize;
    int32_t contentSize = param->contentSize;//内容区占用的字节数
    int32_t fieldSize = param->fieldSize;
    int32_t count = param->count;
    if (count * CETaskParamItemSize != fieldSize) {
        return CETaskParamErrorUnknown;
    }
    int32_t remainSize = totalSize - contentSize - fieldSize;
    if (remainSize < 0) {
        return CETaskParamErrorUnknown;
    }
    
    size_t size = CETaskParamTypeGetLoadSize(t, length);
    size_t rUsedSize = size + CETaskParamItemSize;
    if (rUsedSize > remainSize) {
        return CETaskParamErrorSizeNotEnough;
    }
    int32_t usedSize = (int32_t)rUsedSize;
    if (size > length) {
        return CETaskParamErrorInput;
    }
    CETaskParamItem_t * item = (CETaskParamItem_t *)(param->load) + param->count;
    switch (t) {
        case CTypeBool: {
            bool_t v = *((bool_t *)value);
            item->ext = v ? 1 : 0;
            goto onExtValueSuccess;
        }
            break;
        case CTypeSInt8: {
            sint8_t v = *((sint8_t *)value);
            item->ext = (uint32_t)((uint8_t)v);
            goto onExtValueSuccess;
        }
            break;
        case CTypeUInt8: {
            uint8_t v = *((uint8_t *)value);
            item->ext = (uint32_t)v;
            goto onExtValueSuccess;
        }
            break;
        case CTypeSInt16: {
            sint16_t v = *((sint16_t *)value);
            item->ext = (uint32_t)((uint16_t)v);
            goto onExtValueSuccess;
        }
            break;
        case CTypeUInt16: {
            uint16_t v = *((uint16_t *)value);
            item->ext = (uint32_t)v;
            goto onExtValueSuccess;
        }
            break;
        case CTypeSInt32: case CTypeUInt32: case CTypeFloat: case CTypePtr: case CTypeSInt64: case CTypeUInt64: case CTypeDouble: case CTypeBuffer: {
            void * valueToStore = &(param->load[contentSize]);
            memcpy(valueToStore, value, size);
            
            item->ext = (uint32_t)size;
            item->location = (uint32_t)contentSize;
            item->type = t;
            
            param->contentSize += size;
            param->count += 1;
            param->fieldSize += CETaskParamItemSize;
            return usedSize;
        }
            break;
        default: {
            abort();
            return CETaskParamErrorUnknown;
        }
    }
    
onExtValueSuccess:
    item->location = CETaskParamItemLocationNone;
    item->type = t;
    
    param->count += 1;
    param->fieldSize += CETaskParamItemSize;

    return usedSize;
}

int32_t __CETaskParamGetItem(CETaskParamRef _Nonnull paramRef, uint32_t index, CType_e * _Nonnull typePtr, CType_e expectedType, void * _Nonnull valuePtr, size_t valueMaxSize) {
    CETaskParam_s * param = CETaskParamCheckBridge(paramRef);
    if (NULL == param || NULL == valuePtr || NULL == typePtr) {
        return CETaskParamErrorInput;
    }

    CETaskParamItem_t * item = _CETaskParamGetItem(param, index);
    if (NULL == item) {
        return CETaskParamErrorIndexOutOfRange;
    }
    CType_e t = item->type;
    if (!CTypeIsValid(t)) {
        return CETaskParamErrorUnknown;
    }
    if (CTypeNone != expectedType) {
        if (t != expectedType) {
            return CETaskParamErrorType;
        }
    }
    
    size_t vSize = CETaskParamTypeGetValueSize(t);
    if (vSize < valueMaxSize) {
        return CETaskParamErrorSizeNotEnough;
    }
    size_t size = valueMaxSize;
    switch (t) {
        case CTypeBool: {
            if (item->location != CETaskParamItemLocationNone) {
                return CETaskParamErrorUnknown;
            }
            _Bool * vPtr = valuePtr;
            _Bool v = (item->ext != 0);
            *vPtr = v;
            *typePtr = t;
            return (int32_t)vSize;
        }
            break;
        case CTypeSInt8: {
            if (item->location != CETaskParamItemLocationNone) {
                return CETaskParamErrorUnknown;
            }
            sint8_t * vPtr = valuePtr;
            sint8_t v = (sint8_t)((uint8_t)(item->ext));
            *vPtr = v;
            *typePtr = t;
            return (int32_t)vSize;
        }
            break;
        case CTypeUInt8: {
            if (item->location != CETaskParamItemLocationNone) {
                return CETaskParamErrorUnknown;
            }
            uint8_t * vPtr = valuePtr;
            uint8_t v = ((uint8_t)(item->ext));
            *vPtr = v;
            *typePtr = t;
            return (int32_t)vSize;
        }
            break;
        case CTypeSInt16: {
            if (item->location != CETaskParamItemLocationNone) {
                return CETaskParamErrorUnknown;
            }
            sint16_t * vPtr = valuePtr;
            sint16_t v = (sint16_t)((uint16_t)(item->ext));
            *vPtr = v;
            *typePtr = t;
            return (int32_t)vSize;
        }
            break;
        case CTypeUInt16: {
            if (item->location != CETaskParamItemLocationNone) {
                return CETaskParamErrorUnknown;
            }
            uint16_t * vPtr = valuePtr;
            uint16_t v = (uint16_t)(item->ext);
            *vPtr = v;
            *typePtr = t;
            return (int32_t)vSize;
        }
            break;
        case CTypeSInt32: case CTypeUInt32: case CTypeFloat: case CTypePtr: case CTypeSInt64: case CTypeUInt64: case CTypeDouble: {
            assert(item->location != CETaskParamItemLocationNone);
            void * p = &(param->load[item->location]);
            size_t s = CETaskParamTypeGetLoadSize(t, 0);
            if (s != vSize) {
                abort();
            }
            memcpy(valuePtr, p, s);
            *typePtr = t;
            return (int32_t)vSize;
        }
            break;
        case CTypeBuffer: {
            if (item->location == CETaskParamItemLocationNone) {
                return CETaskParamErrorUnknown;
            }
            if (item->location + item->ext >= param->contentSize) {
                return CETaskParamErrorUnknown;
            }
            if (size < item->ext) {
                return CETaskParamErrorSizeNotEnough;
            }
            
            void * p = &(param->load[item->location]);
            memcpy(valuePtr, p, CETaskParamTypeGetLoadSize(t, 0));
            *typePtr = t;
            return item->ext;
        }
            break;
        default:
            abort();
    }
    
    return CETaskParamErrorUnknown;
}

int32_t __CETaskParamGetContentValue(CETaskParamRef _Nonnull paramRef, uint32_t index, CType_e t, size_t size, void * _Nonnull valuePtr) {
    if (!CTypeIsValid(t)) {
        return CETaskParamErrorType;
    }
    CType_e type = CTypeNone;
    return __CETaskParamGetItem(paramRef, index, &type, t, valuePtr, size);
}


_Bool CETaskParamAppendContentValue(CETaskParamRef _Nonnull paramRef, CType_e t, void * _Nonnull value, size_t length) {
    int32_t result = __CETaskParamAppendContentValue(paramRef, t, value, length);
    if (result >= 0) {
        return true;
    } else {
        if (CETaskParamErrorSizeNotEnough == result) {
            return false;
        } else {
            abort();
            return false;
        }
    }
}

int32_t CETaskParamGetContentValue(CETaskParamRef _Nonnull paramRef, uint32_t index, CType_e t, size_t size, void * _Nonnull valuePtr) {
    return __CETaskParamGetContentValue(paramRef, index, t, size, valuePtr);
}


static inline _Bool CETaskParamGetContentValueWithEnoughSize(CETaskParamRef _Nonnull paramRef, uint32_t index, CType_e t, size_t size, void * _Nonnull valuePtr) {
    int32_t result = __CETaskParamGetContentValue(paramRef, index, t, size, valuePtr);
    if (result >= 0) {
        return true;
    } else {
        if (CETaskParamErrorIndexOutOfRange == result || CETaskParamErrorType == result) {
            return false;
        } else {
            abort();
            return false;
        }
    }
}

void __CETaskParamInit(CETaskParam_s * _Nonnull param, uint32_t totalSize) {
    param->code = 0;
    param->totalSize = totalSize;
    param->contentSize = 0;
    param->fieldSize = 0;
    param->count = 0;
}

#pragma mark - CETaskParam public api

CETaskParamRef _Nullable CETaskParamStackInit(void * _Nonnull ptr, size_t bufferSize, CETaskParamDestroyItems_f _Nullable destroy) {
    if (NULL == ptr) {
        return NULL;
    }
    if (bufferSize < sizeof(CETaskParam_s)) {
        return false;
    }
    
    CERuntimeInitializeType(ptr, CETypeTaskParamStack);
    
    size_t loadSize = bufferSize - sizeof(CETaskParam_s);
    if (loadSize >= CETaskParamItemLocationNone) {
        loadSize = CETaskParamItemLocationNone;
    }
    CETaskParam_s * param = ptr;
    __CETaskParamInit(param, (uint32_t)loadSize);
    param->destroy = destroy;
    return ptr;
}
CETaskParamRef _Nullable CETaskParamHeapCreate(size_t loadSize) {
    if (loadSize >= CETaskParamItemLocationNone) {
        return NULL;
    }
    CETaskParamRef ptr = CETypeTaskParamHeap->alloctor->allocate(CETypeTaskParamHeap, loadSize + sizeof(CETaskParam_s));
    CETaskParam_s * param = ptr;
    __CETaskParamInit(param, (uint32_t)loadSize);
    return ptr;
}




uint32_t CETaskParamGetCount(CETaskParamRef _Nonnull paramRef) {
    CETaskParam_s * param = CETaskParamCheckBridge(paramRef);
    if (NULL == param) {
        abort();
        return 0;
    }
    return param->count;
}

_Bool CETaskParamGetItemType(CETaskParamRef _Nonnull paramRef, uint32_t index, CType_e * _Nonnull itemType) {
    CETaskParam_s * param = CETaskParamCheckBridge(paramRef);
    if (NULL == param || NULL == itemType) {
        abort();
    }
    CETaskParamItem_t * item = _CETaskParamGetItem(param, index);
    if (NULL == item) {
        return false;
    }
    CType_e t = item->type;
    *itemType = t;
    return true;
}

//get
_Bool CETaskParamGetBool(CETaskParamRef _Nonnull paramRef, uint32_t index, _Bool * _Nonnull item) {
    CType_e itemType = CTypeBool;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}

_Bool CETaskParamGetSInt8(CETaskParamRef _Nonnull paramRef, uint32_t index, sint8_t * _Nonnull item) {
    CType_e itemType = CTypeSInt8;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}
_Bool CETaskParamGetSInt16(CETaskParamRef _Nonnull paramRef, uint32_t index, sint16_t * _Nonnull item) {
    CType_e itemType = CTypeSInt16;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}
_Bool CETaskParamGetSInt32(CETaskParamRef _Nonnull paramRef, uint32_t index, sint32_t * _Nonnull item) {
    CType_e itemType = CTypeSInt32;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}
_Bool CETaskParamGetSInt64(CETaskParamRef _Nonnull paramRef, uint32_t index, sint64_t * _Nonnull item) {
    CType_e itemType = CTypeSInt64;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}

_Bool CETaskParamGetUInt8(CETaskParamRef _Nonnull paramRef, uint32_t index, uint8_t * _Nonnull item) {
    CType_e itemType = CTypeUInt8;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}
_Bool CETaskParamGetUInt16(CETaskParamRef _Nonnull paramRef, uint32_t index, uint16_t * _Nonnull item) {
    CType_e itemType = CTypeUInt16;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}
_Bool CETaskParamGetUInt32(CETaskParamRef _Nonnull paramRef, uint32_t index, uint32_t * _Nonnull item) {
    CType_e itemType = CTypeUInt32;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}
_Bool CETaskParamGetUInt64(CETaskParamRef _Nonnull paramRef, uint32_t index, uint64_t * _Nonnull item) {
    CType_e itemType = CTypeUInt64;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}

_Bool CETaskParamGetFloat(CETaskParamRef _Nonnull paramRef, uint32_t index, float * _Nonnull item) {
    CType_e itemType = CTypeFloat;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}
_Bool CETaskParamGetDouble(CETaskParamRef _Nonnull paramRef, uint32_t index, double * _Nonnull item) {
    CType_e itemType = CTypeDouble;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}

_Bool CETaskParamGetPtr(CETaskParamRef _Nonnull paramRef, uint32_t index, CEPtr _Nullable * _Nonnull item) {
    CType_e itemType = CTypePtr;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamGetContentValueWithEnoughSize(paramRef, index, itemType, size, item);
}

_Bool CETaskParamGetBuffer(CETaskParamRef _Nonnull paramRef, uint32_t index, void * _Nonnull buffer, size_t size) {
    CType_e itemType = CTypeBuffer;
    int32_t result = __CETaskParamGetContentValue(paramRef, index, itemType, size, buffer);
    if (result >= 0) {
        return true;
    } else {
        if (CETaskParamErrorIndexOutOfRange == result || CETaskParamErrorType == result || CETaskParamErrorSizeNotEnough == result) {
            return false;
        } else {
            abort();
            return false;
        }
    }
}


//append


_Bool CETaskParamAppendBool(CETaskParamRef _Nonnull paramRef, _Bool item) {
    CType_e itemType = CTypeBool;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}


_Bool CETaskParamAppendSInt8(CETaskParamRef _Nonnull paramRef, sint8_t item) {
    CType_e itemType = CTypeSInt8;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}
_Bool CETaskParamAppendSInt16(CETaskParamRef _Nonnull paramRef, sint16_t item) {
    CType_e itemType = CTypeSInt16;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}
_Bool CETaskParamAppendSInt32(CETaskParamRef _Nonnull paramRef, sint32_t item) {
    CType_e itemType = CTypeSInt32;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}
_Bool CETaskParamAppendSInt64(CETaskParamRef _Nonnull paramRef, sint64_t item) {
    CType_e itemType = CTypeSInt64;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}

_Bool CETaskParamAppendUInt8(CETaskParamRef _Nonnull paramRef, uint8_t item) {
    CType_e itemType = CTypeUInt8;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}
_Bool CETaskParamAppendUInt16(CETaskParamRef _Nonnull paramRef, uint16_t item) {
    CType_e itemType = CTypeUInt16;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}
_Bool CETaskParamAppendUInt32(CETaskParamRef _Nonnull paramRef, uint32_t item) {
    CType_e itemType = CTypeUInt32;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}
_Bool CETaskParamAppendUInt64(CETaskParamRef _Nonnull paramRef, uint64_t item) {
    CType_e itemType = CTypeUInt64;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}

_Bool CETaskParamAppendFloat(CETaskParamRef _Nonnull paramRef, float item) {
    CType_e itemType = CTypeFloat;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}
_Bool CETaskParamAppendDouble(CETaskParamRef _Nonnull paramRef, double item) {
    CType_e itemType = CTypeDouble;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}

_Bool CETaskParamAppendPtr(CETaskParamRef _Nonnull paramRef, CEPtr _Nullable item) {
    CType_e itemType = CTypePtr;
    size_t size = CETaskParamTypeGetValueSize(itemType);
    return CETaskParamAppendContentValue(paramRef, itemType, &item, size);
}

_Bool CETaskParamAppendBuffer(CETaskParamRef _Nonnull paramRef, void * _Nonnull buffer, size_t size) {
    CType_e itemType = CTypeBuffer;
    return CETaskParamAppendContentValue(paramRef, itemType, buffer, size);
}




int32_t CETaskParamGetItem(CETaskParamRef _Nonnull paramRef, uint32_t index, CType_e * _Nullable typePtr, void * _Nonnull valuePtr, size_t valueMaxSize) {
    return __CETaskParamGetItem(paramRef, index, typePtr, CTypeNone, valuePtr, valueMaxSize);
}


int32_t CETaskParamAppendItem(CETaskParamRef _Nonnull paramRef, CType_e t, void * _Nonnull valuePtr, size_t valueSize) {
    return __CETaskParamAppendContentValue(paramRef, t, valuePtr, valueSize);
}
