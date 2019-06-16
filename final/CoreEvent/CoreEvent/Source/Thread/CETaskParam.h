//
//  CETaskParam.h
//  CoreEvent
//
//  Created by vector on 2019/6/15.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CETaskParam_h
#define CETaskParam_h

#include <CoreEvent/CECType.h>
#include <CoreEvent/CERuntime.h>

#define CETaskParamErrorNone 0

#define CETaskParamErrorInput -1
#define CETaskParamErrorUnknown -2
#define CETaskParamErrorType -3
#define CETaskParamErrorSize -4
#define CETaskParamErrorSizeNotEnough -5
#define CETaskParamErrorIndexOutOfRange -6



typedef CEPtr CETaskParamRef;

typedef void (*CEFunction_f)(CETaskParamRef _Nullable param, CETaskParamRef _Nullable result);



extern CETypeRef _Nonnull CETypeTaskParamStack;
extern CETypeRef _Nonnull CETypeTaskParamHeap;

typedef void (*CETaskParamDestroyItems_f)(CETaskParamRef _Nullable param);


CETaskParamRef _Nullable CETaskParamStackInit(void * _Nonnull ptr, size_t bufferSize);

CETaskParamRef _Nullable CETaskParamHeapCreate(size_t loadSize);


uint32_t CETaskParamGetCount(CETaskParamRef _Nonnull paramRef);

_Bool CETaskParamGetItemType(CETaskParamRef _Nonnull paramRef, uint32_t index, CECType_e * _Nonnull itemType);

int32_t CETaskParamGetItem(CETaskParamRef _Nonnull paramRef, uint32_t index, CECType_e * _Nullable typePtr, void * _Nonnull valuePtr, size_t valueMaxSize);

int32_t CETaskParamAppendItem(CETaskParamRef _Nonnull paramRef, CECType_e t, void * _Nonnull valuePtr, size_t valueSize);


#pragma mark - get
_Bool CETaskParamGetBool(CETaskParamRef _Nonnull paramRef, uint32_t index, _Bool * _Nonnull item);

_Bool CETaskParamGetSInt8(CETaskParamRef _Nonnull paramRef, uint32_t index, sint8_t * _Nonnull item);
_Bool CETaskParamGetSInt16(CETaskParamRef _Nonnull paramRef, uint32_t index, sint16_t * _Nonnull item);
_Bool CETaskParamGetSInt32(CETaskParamRef _Nonnull paramRef, uint32_t index, sint32_t * _Nonnull item);
_Bool CETaskParamGetSInt64(CETaskParamRef _Nonnull paramRef, uint32_t index, sint64_t * _Nonnull item);

_Bool CETaskParamGetUInt8(CETaskParamRef _Nonnull paramRef, uint32_t index, uint8_t * _Nonnull item);
_Bool CETaskParamGetUInt16(CETaskParamRef _Nonnull paramRef, uint32_t index, uint16_t * _Nonnull item);
_Bool CETaskParamGetUInt32(CETaskParamRef _Nonnull paramRef, uint32_t index, uint32_t * _Nonnull item);
_Bool CETaskParamGetUInt64(CETaskParamRef _Nonnull paramRef, uint32_t index, uint64_t * _Nonnull item);
_Bool CETaskParamGetFloat(CETaskParamRef _Nonnull paramRef, uint32_t index, float * _Nonnull item);
_Bool CETaskParamGetDouble(CETaskParamRef _Nonnull paramRef, uint32_t index, double * _Nonnull item);

_Bool CETaskParamGetPtr(CETaskParamRef _Nonnull paramRef, uint32_t index, CEPtr _Nullable * _Nonnull item);

_Bool CETaskParamGetBuffer(CETaskParamRef _Nonnull paramRef, uint32_t index, void * _Nonnull buffer, size_t size);



#pragma mark - append
_Bool CETaskParamAppendBool(CETaskParamRef _Nonnull paramRef, _Bool item);


_Bool CETaskParamAppendSInt8(CETaskParamRef _Nonnull paramRef, sint8_t item);
_Bool CETaskParamAppendSInt16(CETaskParamRef _Nonnull paramRef, sint16_t item);
_Bool CETaskParamAppendSInt32(CETaskParamRef _Nonnull paramRef, sint32_t item);
_Bool CETaskParamAppendSInt64(CETaskParamRef _Nonnull paramRef, sint64_t item);

_Bool CETaskParamAppendUInt8(CETaskParamRef _Nonnull paramRef, uint8_t item);
_Bool CETaskParamAppendUInt16(CETaskParamRef _Nonnull paramRef, uint16_t item);
_Bool CETaskParamAppendUInt32(CETaskParamRef _Nonnull paramRef, uint32_t item);
_Bool CETaskParamAppendUInt64(CETaskParamRef _Nonnull paramRef, uint64_t item);

_Bool CETaskParamAppendFloat(CETaskParamRef _Nonnull paramRef, float item);
_Bool CETaskParamAppendDouble(CETaskParamRef _Nonnull paramRef, double item);

_Bool CETaskParamAppendPtr(CETaskParamRef _Nonnull paramRef, CEPtr _Nullable item);

_Bool CETaskParamAppendBuffer(CETaskParamRef _Nonnull paramRef, void * _Nonnull buffer, size_t size);








#endif /* CETaskParam_h */
