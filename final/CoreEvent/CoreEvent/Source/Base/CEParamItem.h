//
//  CEParamItem.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEParamItem_h
#define CEParamItem_h

#include "CEBase.h"
#include "CEParamType.h"

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




#endif /* CEParamItem_h */
