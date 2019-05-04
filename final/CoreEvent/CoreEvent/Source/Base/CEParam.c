//
//  CEParam.c
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEParam.h"

#include "CEMemory.h"



struct _CEParam {
    _Atomic(uint_fast32_t) refCount;//引用计数器
    uint32_t bufferSize;//参数容纳总个数
    CEParamItem_s items[0];
};

CEParamRef _Nullable CEParamInit(uint32_t itemCount) {
    if (itemCount > CETaskParamItemMaxCount || 0 == itemCount) {
        return NULL;
    }
    CEParamRef result = CEAllocateClear(sizeof(CEParam_s) + sizeof(CEParamItem_s) * ((size_t)itemCount));
    atomic_store(&(result->refCount), 1);

    result->bufferSize = itemCount;
    
    for (uint32_t index=0; index<itemCount; index++) {
        result->items[index].index = index;
    }
    
    return result;
}

void CEParamDeinit(CEParamRef _Nonnull param) {
    assert(param);
    for (uint32_t index=0; index<param->bufferSize; index++) {
        CEParamItem_s * item = &(param->items[index]);

        if (item->release) {
            item->release(item->type, item->typeName, item->value);
        }
    }
    CEDeallocate(param);
}

CEParamRef _Nonnull CEParamRetain(CEParamRef _Nonnull param) {
    assert(param);

    uint32_t refCount = 0;
    uint32_t newValue = 0;
    
    do {
        refCount = atomic_load(&(param->refCount));
        assert(refCount > 0);
        newValue = refCount + 1;
    } while (!atomic_compare_exchange_weak(&(param->refCount), &refCount, newValue));
    
    return param;
}

void CEParamRelease(CEParamRef _Nonnull param) {
    assert(param);
    
    uint32_t refCount = 0;
    uint32_t newValue = 0;
    
    do {
        refCount = atomic_load(&(param->refCount));
        assert(refCount > 0);
        newValue = refCount - 1;
    } while (!atomic_compare_exchange_weak(&(param->refCount), &refCount, newValue));
    if (0 == newValue) {
        CEParamDeinit(param);
    }
}

_Bool CEParamSetItem(CEParamRef _Nonnull param,
                     uint32_t index,
                     CETaskParamItemType_e type,
                     char * _Nullable name,
                     char * _Nullable typeName,
                     CEParamItemRelease_f _Nullable release,
                     void * _Nonnull valuePtr) {
    assert(param);
    assert(valuePtr);
    if (type > CETaskParamItemTypeMax) {
        return false;
    }
    if (index >= param->bufferSize) {
        return false;
    }
    CEParamItem_s * item = &(param->items[index]);
    
    CEParamItemValue_u value = {};
    
    switch (type) {
        case CETaskParamItemTypePtr: {
            void ** ptr = valuePtr;
            value.ptrValue = *ptr;
        }
            break;
        case CETaskParamItemTypeBool: {
            _Bool * ptr = valuePtr;
            value.boolValue = *ptr;
        }
            break;
        case CETaskParamItemTypeSInt8: {
            int8_t * ptr = valuePtr;
            value.sint8Value = *ptr;
        }
            break;
        case CETaskParamItemTypeUInt8: {
            uint8_t * ptr = valuePtr;
            value.uint8Value = *ptr;
        }
            break;
        case CETaskParamItemTypeSInt16: {
            int16_t * ptr = valuePtr;
            value.sint16Value = *ptr;
        }
            break;
        case CETaskParamItemTypeUInt16: {
            uint16_t * ptr = valuePtr;
            value.uint16Value = *ptr;
        }
            break;
        case CETaskParamItemTypeSInt32: {
            int32_t * ptr = valuePtr;
            value.sint32Value = *ptr;
        }
            break;
        case CETaskParamItemTypeUInt32: {
            uint32_t * ptr = valuePtr;
            value.uint32Value = *ptr;
        }
            break;
        case CETaskParamItemTypeSInt64: {
            int64_t * ptr = valuePtr;
            value.sint64Value = *ptr;
        }
            break;
        case CETaskParamItemTypeUInt64: {
            uint64_t * ptr = valuePtr;
            value.uint64Value = *ptr;
        }
            break;
        case CETaskParamItemTypeFloat: {
            float * ptr = valuePtr;
            value.floatValue = *ptr;
        }
            break;
        case CETaskParamItemTypeDouble: {
            double * ptr = valuePtr;
            value.doubleValue = *ptr;
        }
            break;
            //        case CETaskParamItemTypeStruct: {
            //
            //        }
            //            break;
        default:
            break;
    }
    item->type = type;
    item->name = name;
    item->typeName = typeName;
    item->release = release;
    item->value = value;
    
    return true;
}

_Bool CEParamGetItem(CEParamRef _Nonnull param,
                     uint32_t index,
                     CEParamItem_s * _Nonnull itemRef) {
    assert(param);
    if (NULL == itemRef) {
        return true;
    }
    
    if (index >= param->bufferSize) {
        return false;
    }
    
    CEParamItem_s * item = &(param->items[index]);
    memcpy(itemRef, item, sizeof(CEParamItem_s));
    return true;
}
