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


#if CEBuild64Bit

#define CEParamBufferItemSizeMax 2048
#define CEParamItemContentLengthBitCount 16
#define CEParamItemContentLengthMask 0xFFFFul
#define CEParamItemContentLocationMask 0x1FFFFFFFFFFul

#else

#define CEParamBufferItemSizeMax 1024
#define CEParamItemContentLengthBitCount 11
#define CEParamItemContentLengthMask 0x7FFu
#define CEParamItemContentLocationMask 0x3FFFu

#endif


#define CEParamItemMaxCount 16
//static const uint32_t CEParamItemMaxCount = 32;

typedef void (*CEParamItemRelease_f)(CEParamType_e type, char * _Nullable typeName, CEParamItemValue_u value);

#if CEBuild64Bit
#define CEStackParamSize(capacity, structSize) (CECpuWordByteSize * (2 + capacity) + 8 * capacity + structSize)
#else
#define CEStackParamSize(capacity, structSize) (CECpuWordByteSize * (3 + capacity) + 8 * capacity + structSize)
#endif



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






CEHeapParamRef _Nonnull CEHeapParamCreate(uint32_t capacity);


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
