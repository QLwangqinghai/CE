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


struct _CEParam;
typedef struct _CEParam CEParam_s;
typedef CEParam_s * CEParamRef;

#define CEParamItemMaxCount 16
//static const uint32_t CEParamItemMaxCount = 32;

typedef void (*CEParamItemRelease_f)(CEParamType_e type, char * _Nullable typeName, CEParamItemValue_u value);


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




CEParamRef _Nullable CEParamInit(uint32_t itemCount);


_Bool CEParamSetItem(CEParamRef _Nonnull param,
                     uint32_t index,
                     CEParamType_e type,
                     char * _Nullable name,
                     char * _Nullable typeName,
                     CEParamItemRelease_f _Nullable release,
                     void * _Nonnull valuePtr);

_Bool CEParamGetItem(CEParamRef _Nonnull param,
                     uint32_t index,
                     CEParamItem_s * _Nonnull itemRef);

#endif /* CETaskParam_h */
