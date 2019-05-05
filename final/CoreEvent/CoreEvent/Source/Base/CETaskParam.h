//
//  CETaskParam.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CETaskParam_h
#define CETaskParam_h

#include "CEBase.h"
#include "CEParamItem.h"

static const uint32_t CETaskParamItemMaxCount = 255;

typedef void (*CETaskParamItemRelease_f)(uint32_t type, CEParamItemValue_u value);

typedef struct _CETaskParamItem {
    CEParamItemValue_u value;
    CEParamType_e type;
} CETaskParamItem_s;

typedef struct _CETaskParamItemContent {
    CEParamItemValue_u value;
    CETaskParamItemRelease_f _Nullable release;//option
} CETaskParamItemContent_s;

//CEParamRef _Nullable CEParamInit(uint32_t itemCount);
//
//CEParamRef _Nonnull CEParamRetain(CEParamRef _Nonnull param);
//void CEParamRelease(CEParamRef _Nonnull param);
//
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
