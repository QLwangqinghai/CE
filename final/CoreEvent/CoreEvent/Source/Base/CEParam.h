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

typedef void * CEStackParamRef;
typedef void * CEHeapParamRef;
typedef void * CEParamRef;

extern CETypeRef _Nonnull CETypeStackParam;
extern CETypeRef _Nonnull CETypeHeapParam;

// 26(1+15+10)

//16kb
#define CEParamBufferItemsTotalSizeMax 16384

#define CEParamBufferItemSizeMax 512

#define CEParamItemContentLocationBitCount 15
#define CEParamItemContentLocationMask 0x7FFFu

#define CEParamItemContentLengthBitCount 10
#define CEParamItemContentLengthMask 0x3FFu

#define CEParamItemMaxCount 16

typedef void (*CEParamItemRelease_f)(CEParamType_e type, void * _Nonnull itemPtr);

#define CEStackParamSize(capacity, structSize) (CECpuWordByteSize + 8 + 12 * capacity + structSize)


CEStackParamRef _Nullable CEStackParamInit(void * _Nonnull ptr, size_t size, uint32_t capacity);

_Bool CEStackParamAppendBool(CEStackParamRef _Nonnull param, _Bool item);
_Bool CEStackParamAppendSInt8(CEStackParamRef _Nonnull param, int8_t item);
_Bool CEStackParamAppendUInt8(CEStackParamRef _Nonnull param, uint8_t item);
_Bool CEStackParamAppendSInt16(CEStackParamRef _Nonnull param, int16_t item);
_Bool CEStackParamAppendUInt16(CEStackParamRef _Nonnull param, uint16_t item);
_Bool CEStackParamAppendSInt32(CEStackParamRef _Nonnull param, int32_t item);
_Bool CEStackParamAppendUInt32(CEStackParamRef _Nonnull param, uint32_t item);
_Bool CEStackParamAppendSInt64(CEStackParamRef _Nonnull param, int64_t item);
_Bool CEStackParamAppendUInt64(CEStackParamRef _Nonnull param, uint64_t item);
_Bool CEStackParamAppendFloat(CEStackParamRef _Nonnull param, float item);
_Bool CEStackParamAppendDouble(CEStackParamRef _Nonnull param, double item);
_Bool CEStackParamAppendPtr(CEStackParamRef _Nonnull param, void * _Nullable item);
_Bool CEStackParamAppendRef(CEStackParamRef _Nonnull param, CERef _Nullable item);
_Bool CEStackParamAppendBuffer(CEStackParamRef _Nonnull param, void * _Nonnull buffer, size_t size);


uint32_t CEParamGetCount(CEParamRef _Nonnull param);
_Bool CEParamGetItemType(CEParamRef _Nonnull param, uint32_t index, CEParamType_e * _Nonnull itemType);


_Bool CEParamGetBool(CEParamRef _Nonnull param, uint32_t index, _Bool * _Nonnull item);
_Bool CEParamGetSInt8(CEParamRef _Nonnull param, uint32_t index, int8_t * _Nonnull item);
_Bool CEParamGetUInt8(CEParamRef _Nonnull param, uint32_t index, uint8_t * _Nonnull item);
_Bool CEParamGetSInt16(CEParamRef _Nonnull param, uint32_t index, int16_t * _Nonnull item);
_Bool CEParamGetUInt16(CEParamRef _Nonnull param, uint32_t index, uint16_t * _Nonnull item);
_Bool CEParamGetSInt32(CEParamRef _Nonnull param, uint32_t index, int32_t * _Nonnull item);
_Bool CEParamGetUInt32(CEParamRef _Nonnull param, uint32_t index, uint32_t * _Nonnull item);
_Bool CEParamGetSInt64(CEParamRef _Nonnull param, uint32_t index, int64_t * _Nonnull item);
_Bool CEParamGetUInt64(CEParamRef _Nonnull param, uint32_t index, uint64_t * _Nonnull item);
_Bool CEParamGetFloat(CEParamRef _Nonnull param, uint32_t index, float * _Nonnull item);
_Bool CEParamGetDouble(CEParamRef _Nonnull param, uint32_t index, double * _Nonnull item);
_Bool CEParamGetPtr(CEParamRef _Nonnull param, uint32_t index, void * _Nullable * _Nonnull item);
_Bool CEParamGetRef(CEParamRef _Nonnull param, uint32_t index, CERef _Nullable * _Nonnull item);
_Bool CEParamGetBuffer(CEParamRef _Nonnull param, uint32_t index, void * _Nonnull buffer, size_t size);


CEHeapParamRef _Nonnull CEHeapParamCreate(uint32_t capacity, size_t bufferItemsTotalSize);


CEParamRef _Nullable CEParamInit(uint32_t itemCount);


//_Bool CEParamSetItem(CEParamRef _Nonnull param,
//                     uint32_t index,
//                     CEParamType_e type,
//                     char * _Nullable name,
//                     char * _Nullable typeName,
//                     CEParamItemRelease_f _Nullable release,
//                     void * _Nonnull valuePtr);
//
//_Bool CEParamGetItem(CEParamRef _Nonnull param,
//                     uint32_t index,
//                     CEParamItem_s * _Nonnull itemRef);

#endif /* CETaskParam_h */
