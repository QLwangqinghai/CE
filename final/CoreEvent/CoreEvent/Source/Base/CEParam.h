//
//  CEParam.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEParam_h
#define CEParam_h

#include "CEBase.h"
#include "CEParamItem.h"
#include "CERuntime.h"

#define CEParamBufferItemsTotalSizeMax 1936
#define CEParamItemMaxCount 8


//tuple

typedef void * CEStackParamRef;
typedef void * CEHeapParamRef;
typedef void * CEParamRef;

typedef void (*CEFunction_f)(CEParamRef _Nullable param, CEParamRef _Nullable result);

typedef struct _CESelector {
    uint8_t paramTypes[CEParamItemMaxCount];
    uint8_t resultTypes[CEParamItemMaxCount];
    CEFunction_f _Nonnull func;
} CESelector_s;



extern CETypeRef _Nonnull CETypeStackParam;
extern CETypeRef _Nonnull CETypeHeapParam;


typedef void (*CEParamItemRelease_f)(CEParamType_e type, void * _Nonnull itemPtr);

#define CEStackParamSize(structSize) (308 + structSize)

size_t CEStackParamGoodSize(uint32_t capacity, size_t bufferItemsTotalSize);

CEStackParamRef _Nullable CEStackParamInit(void * _Nonnull ptr, size_t size, uint32_t capacity);
CEHeapParamRef _Nonnull CEHeapParamCreate(uint32_t capacity, size_t bufferItemsTotalSize);


uint32_t CEParamGetCount(CEParamRef _Nonnull param);
_Bool CEParamGetItemType(CEParamRef _Nonnull param, uint32_t index, CEParamType_e * _Nonnull itemType);


_Bool CEParamGetBool(CEParamRef _Nonnull param, uint32_t index, _Bool * _Nonnull item);

_Bool CEParamGetSInt8(CEParamRef _Nonnull param, uint32_t index, int8_t * _Nonnull item);
_Bool CEParamGetSInt16(CEParamRef _Nonnull param, uint32_t index, int16_t * _Nonnull item);
_Bool CEParamGetSInt32(CEParamRef _Nonnull param, uint32_t index, int32_t * _Nonnull item);
_Bool CEParamGetSInt64(CEParamRef _Nonnull param, uint32_t index, int64_t * _Nonnull item);

_Bool CEParamGetUInt8(CEParamRef _Nonnull param, uint32_t index, uint8_t * _Nonnull item);
_Bool CEParamGetUInt16(CEParamRef _Nonnull param, uint32_t index, uint16_t * _Nonnull item);
_Bool CEParamGetUInt32(CEParamRef _Nonnull param, uint32_t index, uint32_t * _Nonnull item);
_Bool CEParamGetUInt64(CEParamRef _Nonnull param, uint32_t index, uint64_t * _Nonnull item);

_Bool CEParamGetFloat(CEParamRef _Nonnull param, uint32_t index, float * _Nonnull item);
_Bool CEParamGetDouble(CEParamRef _Nonnull param, uint32_t index, double * _Nonnull item);

_Bool CEParamGetPtr(CEParamRef _Nonnull param, uint32_t index, void * _Nullable * _Nonnull item);
_Bool CEParamGetRef(CEParamRef _Nonnull param, uint32_t index, CERef _Nullable * _Nonnull item);
_Bool CEParamGetBuffer(CEParamRef _Nonnull param, uint32_t index, void * _Nonnull buffer, size_t size);

_Bool CEParamGetItem(CEParamRef _Nonnull param, uint32_t index, CEParamType_e * _Nullable typePtr, size_t * _Nullable sizePtr, void * _Nonnull ptr, size_t maxSize);


_Bool CEParamAppendBool(CEParamRef _Nonnull param, _Bool item);

_Bool CEParamAppendSInt8(CEParamRef _Nonnull param, int8_t item);
_Bool CEParamAppendSInt16(CEParamRef _Nonnull param, int16_t item);
_Bool CEParamAppendSInt32(CEParamRef _Nonnull param, int32_t item);
_Bool CEParamAppendSInt64(CEParamRef _Nonnull param, int64_t item);

_Bool CEParamAppendUInt8(CEParamRef _Nonnull param, uint8_t item);
_Bool CEParamAppendUInt16(CEParamRef _Nonnull param, uint16_t item);
_Bool CEParamAppendUInt32(CEParamRef _Nonnull param, uint32_t item);
_Bool CEParamAppendUInt64(CEParamRef _Nonnull param, uint64_t item);

_Bool CEParamAppendFloat(CEParamRef _Nonnull param, float item);
_Bool CEParamAppendDouble(CEParamRef _Nonnull param, double item);

_Bool CEParamAppendPtr(CEParamRef _Nonnull param, void * _Nullable item);

_Bool CEParamAppendRef(CEParamRef _Nonnull param, CERef _Nullable item);

_Bool CEParamAppendBuffer(CEParamRef _Nonnull param, void * _Nonnull buffer, size_t size);

_Bool CEParamAppendItem(CEParamRef _Nonnull param, CEParamType_e type, void * _Nonnull itemPtr);


#endif /* CETaskParam_h */
