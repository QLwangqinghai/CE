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

typedef uint32_t CETaskParamItemType_e;

static const CETaskParamItemType_e CETaskParamItemTypePtr = 0;
static const CETaskParamItemType_e CETaskParamItemTypeBool = 1;
static const CETaskParamItemType_e CETaskParamItemTypeSInt8 = 2;
static const CETaskParamItemType_e CETaskParamItemTypeUInt8 = 3;
static const CETaskParamItemType_e CETaskParamItemTypeSInt16 = 4;
static const CETaskParamItemType_e CETaskParamItemTypeUInt16 = 5;
static const CETaskParamItemType_e CETaskParamItemTypeSInt32 = 6;
static const CETaskParamItemType_e CETaskParamItemTypeUInt32 = 7;
static const CETaskParamItemType_e CETaskParamItemTypeSInt64 = 8;
static const CETaskParamItemType_e CETaskParamItemTypeUInt64 = 9;
static const CETaskParamItemType_e CETaskParamItemTypeFloat = 10;
static const CETaskParamItemType_e CETaskParamItemTypeDouble = 11;
//static const CETaskParamItemType_e CETaskParamItemTypeStruct = 12;

static const CETaskParamItemType_e CETaskParamItemTypeMax = CETaskParamItemTypeDouble;


struct _CEParam;
typedef struct _CEParam CEParam_s;
typedef CEParam_s * CEParamRef;


static const uint32_t CETaskParamItemMaxCount = 64;
static const uint32_t CETaskParamItemMaxSize = 0x4000;//16kb

typedef union _CEParamItemValue {
    void * _Nonnull * _Nullable ptrValue;
    _Bool boolValue;
    int8_t sint8Value;
    uint8_t uint8Value;
    int16_t sint16Value;
    uint16_t uint16Value;
    int32_t sint32Value;
    uint32_t uint32Value;
    int64_t sint64Value;
    uint64_t uint64Value;
    float floatValue;
    double doubleValue;
    
    //    uint8_t structContent[8];//struct 比较特殊， size <= 8 时，存放在structContent
    //    void * _Nonnull structValue;//struct 比较特殊， 内容存放在struct _CETaskParamItemStorage.content 中
} CEParamItemValue_u;

typedef void (*CEParamItemRelease_f)(uint32_t type, char * _Nullable typeName, CEParamItemValue_u value);

typedef struct _CEParamItem {
    uint32_t index: 6;
    uint32_t type: 5;
    uint32_t xxx: 21;
    
#if CEBuild64Bit
    uint32_t xxxx: 32;
#endif
    
    char * _Nullable name;//option
    char * _Nullable typeName;//option
    CEParamItemRelease_f _Nullable release;//option
    CEParamItemValue_u value;
} CEParamItem_s;


CEParamRef _Nullable CEParamInit(uint32_t itemCount);

CEParamRef _Nonnull CEParamRetain(CEParamRef _Nonnull param);
void CEParamRelease(CEParamRef _Nonnull param);


_Bool CEParamSetItem(CEParamRef _Nonnull param,
                     uint32_t index,
                     CETaskParamItemType_e type,
                     char * _Nullable name,
                     char * _Nullable typeName,
                     CEParamItemRelease_f _Nullable release,
                     void * _Nonnull valuePtr);

_Bool CEParamGetItem(CEParamRef _Nonnull param,
                     uint32_t index,
                     CEParamItem_s * _Nonnull itemRef);

#endif /* CETaskParam_h */
