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

struct _CEParam;
typedef struct _CEParam CEParam_s;
typedef CEParam_s * CEParamRef;

static const uint32_t CEParamItemMaxCount = 255;

typedef void (*CEParamItemRelease_f)(CEParamType_e type, char * _Nullable typeName, CEParamItemValue_u value);

typedef struct _CEParamItem {
    uint32_t type: 8;
    uint32_t xxx: 24;

#if CEBuild64Bit
    uint32_t xxxx: 32;
#endif
    
    char * _Nullable name;//option
    char * _Nullable typeName;//option
    CEParamItemValue_u value;
} CEParamItem_s;


CEParamRef _Nullable CEParamInit(uint32_t itemCount);

CEParamRef _Nonnull CEParamRetain(CEParamRef _Nonnull param);
void CEParamRelease(CEParamRef _Nonnull param);

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
