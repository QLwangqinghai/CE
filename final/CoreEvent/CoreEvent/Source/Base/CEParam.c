//
//  CEParam.c
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEParam.h"

#include "CEMemory.h"

#if CEBuild64Bit
typedef union _CEParamItemInlineValue {
    _Bool boolValue;
    int8_t sint8Value;
    uint8_t uint8Value;
    int16_t sint16Value;
    uint16_t uint16Value;
    int32_t sint32Value;
    uint32_t uint32Value;
    float floatValue;
} CEParamItemInlineValue_u;

typedef struct _CEParamItem {
    uint64_t type: 6;
    uint64_t hasRelease: 1;
    uint64_t content: 57;
} CEParamItem_s;

#else

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

#endif

size_t CEStackParamGetSize(CERef _Nonnull p);



const CEType_s __CETypeStackParam = {
    .type = CETypeMate,
    .version = CERuntimeVersion,
    .masks = CETypeMaskNoRc,
    .objectSize = 0,
    .name = "CEStackParam",
    .identifier = (uintptr_t)(&__CETypeStackParam),
    .alloctor = &__CETypeDefaultAlloctor,
    
    .getSize = CEStackParamGetSize,
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
    
    .getSize = CEStackParamGetSize,
    .deinit = CETypeDefaultDeinit,
    .descript = CETypeDefaultDescript,
};


CETypeRef _Nonnull CETypeStackParam = &__CETypeStackParam;
CETypeRef _Nonnull CETypeHeapParam = &__CETypeHeapParam;


typedef struct _CEParamItemContent {
    char * _Nullable name;//option
    char * _Nullable typeName;//option
    CEParamItemValue_u value;
    CEParamItemRelease_f _Nullable release;//option
} CEParamItemContent_s;




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
    void * _Nullable content;//存放struct
    uint8_t baseContent[0];//    CEParamItem_s items[base.bufferSize]; uint8_t content[];
} CEHeapParam_s;

struct _CEParam11 {
    _Atomic(uint_fast32_t) refCount;//引用计数器
    uint8_t count: 8;//参数容纳总个数
    uint8_t typeContentSize: 8;//参数全部类型占的字节数 CPU位宽对齐
    uint32_t contentSize: 16;//参数容纳总个数

    uint8_t content[0];
};

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


void CEParamItemSetRange(CEParamItem_s * _Nonnull item, size_t location, size_t length) {
#if CEBuild64Bit
    uint64_t loc = (uint64_t)location;
    uint64_t len = (uint64_t)length;
    item->content = (loc << CEParamItemContentLengthBitCount) | len;
#else
    uint32_t loc = (uint32_t)location;
    uint32_t len = (uint32_t)length;
    item->content = (loc << CEParamItemContentLengthBitCount) | len;
#endif
}
void CEParamItemGetRange(CEParamItem_s * _Nonnull item, size_t * _Nonnull location, size_t * _Nonnull length) {
#if CEBuild64Bit
    uint64_t content = item->content;
    uint64_t loc = (content >> CEParamItemContentLengthBitCount) & CEParamItemContentLocationMask;
    uint64_t len = content & CEParamItemContentLengthMask;
    *location = (size_t)loc;
    *length = (size_t)len;
#else
    uint32_t content = item->content;
    uint32_t loc = (content >> CEParamItemContentLengthBitCount) & CEParamItemContentLocationMask;
    uint32_t len = content & CEParamItemContentLengthMask;
    *location = (size_t)loc;
    *length = (size_t)len;
#endif
}

_Bool _CEStackParamAppendInlineValue(CEStackParamRef _Nonnull stackParam, CEParamType_e t, CEParamItemInlineValue_u value) {
    CEStackParam_s * param = stackParam;
    CETypeRef type = param->runtime.type;
    CEStackParamCheck(param, type);
    CEStackParamCheckCapacity(param);
    
    CEParamItem_s * items = (CEParamItem_s *)(param->itemsAndExt);
    uint8_t count = param->base.count;
    items[count].type = t;
    items[count].content = value.uint32Value;
    param->base.count += 1;
    
    return true;
}

_Bool _CEStackParamAppendContentValue(CEStackParamRef _Nonnull stackParam, CEParamType_e t, void * _Nonnull value, size_t size) {
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

    CEParamItemSetRange(&items[param->base.count], offset, size);
    param->base.count += 1;
    return true;
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
#if CEBuild64Bit
    CEParamItemInlineValue_u value = {
        .sint32Value = item,
    };
    return _CEStackParamAppendInlineValue(stackParam, CEParamTypeSInt32, value);
#else
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeSInt32, value, sizeof(int32_t));
#endif
}
_Bool CEStackParamAppendUInt32(CEStackParamRef _Nonnull stackParam, uint32_t item) {
#if CEBuild64Bit
    CEParamItemInlineValue_u value = {
        .uint32Value = item,
    };
    return _CEStackParamAppendInlineValue(stackParam, CEParamTypeUInt32, value);
#else
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeUInt32, value, sizeof(uint32_t));
#endif
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
#if CEBuild64Bit
    CEParamItemInlineValue_u value = {
        .floatValue = item,
    };
    return _CEStackParamAppendInlineValue(stackParam, CEParamTypeFloat, value);
#else
    void * value = &item;
    return _CEStackParamAppendContentValue(stackParam, CEParamTypeFloat, value, sizeof(float));
#endif
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



//
//void CEParamDeinit(CEParamRef _Nonnull param) {
//    assert(param);
//    if (param->count > 0) {
//        CEParamItemContent_s * contents = (CEParamItemContent_s *)&(param->content[param->typeContentSize]);
//
//        for (uint32_t index=0; index<param->count; index++) {
//            CEParamItemContent_s * item = contents + index;
//            if (item->release) {
//                item->release(param->content[index], item->typeName, item->value);
//            }
//        }
//    }
//
//    CEDeallocate(param);
//}
//
//CEParamRef _Nonnull CEParamRetain(CEParamRef _Nonnull param) {
//    assert(param);
//
//    uint32_t refCount = 0;
//    uint32_t newValue = 0;
//
//    do {
//        refCount = atomic_load(&(param->refCount));
//        assert(refCount > 0);
//        newValue = refCount + 1;
//    } while (!atomic_compare_exchange_weak(&(param->refCount), &refCount, newValue));
//
//    return param;
//}
//
//void CEParamRelease(CEParamRef _Nonnull param) {
//    assert(param);
//
//    uint32_t refCount = 0;
//    uint32_t newValue = 0;
//
//    do {
//        refCount = atomic_load(&(param->refCount));
//        assert(refCount > 0);
//        newValue = refCount - 1;
//    } while (!atomic_compare_exchange_weak(&(param->refCount), &refCount, newValue));
//    if (0 == newValue) {
//        CEParamDeinit(param);
//    }
//}
//
//_Bool CEParamSetItem(CEParamRef _Nonnull param,
//                     uint32_t index,
//                     CEParamType_e type,
//                     char * _Nullable name,
//                     char * _Nullable typeName,
//                     CEParamItemRelease_f _Nullable release,
//                     void * _Nonnull valuePtr) {
//    assert(param);
//    assert(valuePtr);
//    if (type > CEParamTypeMax) {
//        return false;
//    }
//    if (index >= param->count) {
//        return false;
//    }
//    CEParamItemContent_s * contents = (CEParamItemContent_s *)&(param->content[param->typeContentSize]);
//
//    CEParamItemContent_s * item = contents + index;
//
//    CEParamItemValue_u value = {};
//
//    switch (type) {
//        case CEParamTypePtr: {
//            void ** ptr = valuePtr;
//            value.ptrValue = *ptr;
//        }
//            break;
//        case CEParamTypeBool: {
//            _Bool * ptr = valuePtr;
//            value.boolValue = *ptr;
//        }
//            break;
//        case CEParamTypeSInt8: {
//            int8_t * ptr = valuePtr;
//            value.sint8Value = *ptr;
//        }
//            break;
//        case CEParamTypeUInt8: {
//            uint8_t * ptr = valuePtr;
//            value.uint8Value = *ptr;
//        }
//            break;
//        case CEParamTypeSInt16: {
//            int16_t * ptr = valuePtr;
//            value.sint16Value = *ptr;
//        }
//            break;
//        case CEParamTypeUInt16: {
//            uint16_t * ptr = valuePtr;
//            value.uint16Value = *ptr;
//        }
//            break;
//        case CEParamTypeSInt32: {
//            int32_t * ptr = valuePtr;
//            value.sint32Value = *ptr;
//        }
//            break;
//        case CEParamTypeUInt32: {
//            uint32_t * ptr = valuePtr;
//            value.uint32Value = *ptr;
//        }
//            break;
//        case CEParamTypeSInt64: {
//            int64_t * ptr = valuePtr;
//            value.sint64Value = *ptr;
//        }
//            break;
//        case CEParamTypeUInt64: {
//            uint64_t * ptr = valuePtr;
//            value.uint64Value = *ptr;
//        }
//            break;
//        case CEParamTypeFloat: {
//            float * ptr = valuePtr;
//            value.floatValue = *ptr;
//        }
//            break;
//        case CEParamTypeDouble: {
//            double * ptr = valuePtr;
//            value.doubleValue = *ptr;
//        }
//            break;
//            //        case CEParamTypeStruct: {
//            //
//            //        }
//            //            break;
//        default:
//            break;
//    }
//    param->content[index] = type;
//    item->name = name;
//    item->typeName = typeName;
//    item->release = release;
//    item->value = value;
//
//    return true;
//}
//
//_Bool CEParamGetItem(CEParamRef _Nonnull param,
//                     uint32_t index,
//                     CEParamItem_s * _Nonnull itemRef) {
//    assert(param);
//    if (index >= param->count) {
//        return false;
//    }
//    if (NULL == itemRef) {
//        return true;
//    }
//
//    CEParamItemContent_s * contents = (CEParamItemContent_s *)&(param->content[param->typeContentSize]);
//    CEParamItemContent_s * content = contents + index;
//
//    itemRef->type = param->content[index];
//    itemRef->name = content->name;
//    itemRef->typeName = content->typeName;
//    itemRef->value = content->value;
//
//    return true;
//}

size_t CEStackParamGetSize(CERef _Nonnull p) {
    CEStackParam_s * param = p;
    CETypeRef type = param->runtime.type;
    CEStackParamCheck(param, type);
    CEStackParamCheckCapacity(param);
    
    
    
    
    return 0;
}
