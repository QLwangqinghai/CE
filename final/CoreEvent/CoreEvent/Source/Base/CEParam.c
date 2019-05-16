//
//  CEParam.c
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CEParam.h"

#include "CEMemory.h"



typedef struct _CEParamItemContent {
    char * _Nullable name;//option
    char * _Nullable typeName;//option
    CEParamItemValue_u value;
    CEParamItemRelease_f _Nullable release;//option
} CEParamItemContent_s;





typedef struct _CEParamBase {
#if CEBuild64Bit
    uint32_t contentSize;
    uint32_t count;
#else
    uint16_t contentSize;
    uint16_t count;
#endif
    
    CEParamItem_s items[CEParamItemMaxCount];
    uint8_t * _Nonnull content;
} CEParamBase_t;

struct _CEParam {
    CEParamBase_t base;
    uint8_t content[0];
};


struct _CEParam11 {
    
    
    
    
    
    _Atomic(uint_fast32_t) refCount;//引用计数器
    uint32_t count: 8;//参数容纳总个数
    uint32_t typeContentSize: 8;//参数全部类型占的字节数 CPU位宽对齐
    uint32_t contentSize: 16;//参数容纳总个数

    uint8_t content[0];
};

//CEParamRef _Nullable CEParamInit(uint32_t itemCount) {
//    if (itemCount > CEParamItemMaxCount || 0 == itemCount) {
//        return NULL;
//    }
//    uint32_t typeContentSize = (itemCount + CECpuWordByteSize - 1) / CECpuWordByteSize * CECpuWordByteSize;
//    size_t contentSize = sizeof(uint8_t) * typeContentSize + sizeof(CEParamItemContent_s) * itemCount;
//    CEParamRef result = CEAllocateClear(sizeof(CEParam_s) + contentSize);
//    atomic_store(&(result->refCount), 1);
//
//    result->count = itemCount;
//    result->typeContentSize = typeContentSize;
//    result->contentSize = (uint32_t)contentSize;
//
//    return result;
//}
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
