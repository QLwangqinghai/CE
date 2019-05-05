//
//  CEParamType.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEParamType_h
#define CEParamType_h

typedef uint8_t CEParamType_e;

static const CEParamType_e CEParamTypePtr = 0;
static const CEParamType_e CEParamTypeBool = 1;
static const CEParamType_e CEParamTypeSInt8 = 2;
static const CEParamType_e CEParamTypeUInt8 = 3;
static const CEParamType_e CEParamTypeSInt16 = 4;
static const CEParamType_e CEParamTypeUInt16 = 5;
static const CEParamType_e CEParamTypeSInt32 = 6;
static const CEParamType_e CEParamTypeUInt32 = 7;
static const CEParamType_e CEParamTypeSInt64 = 8;
static const CEParamType_e CEParamTypeUInt64 = 9;
static const CEParamType_e CEParamTypeFloat = 10;
static const CEParamType_e CEParamTypeDouble = 11;
//static const CEParamType_e CEParamTypeStruct = 12;

static const CEParamType_e CEParamTypeMax = CEParamTypeDouble;


#endif /* CEParamType_h */
