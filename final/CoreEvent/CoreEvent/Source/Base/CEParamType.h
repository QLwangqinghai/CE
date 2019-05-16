//
//  CEParamType.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEParamType_h
#define CEParamType_h

//6bit



typedef uint8_t CEParamType_e;

static const CEParamType_e CEParamTypeBool = 0;
static const CEParamType_e CEParamTypeSInt8 = 1;
static const CEParamType_e CEParamTypeUInt8 = 2;
static const CEParamType_e CEParamTypeSInt16 = 3;
static const CEParamType_e CEParamTypeUInt16 = 4;
static const CEParamType_e CEParamTypeSInt32 = 5;
static const CEParamType_e CEParamTypeUInt32 = 6;
static const CEParamType_e CEParamTypeFloat = 7;

static const CEParamType_e CEParamTypePtr = 8;
static const CEParamType_e CEParamTypeRef = 9;

static const CEParamType_e CEParamTypeSInt64 = 10;
static const CEParamType_e CEParamTypeUInt64 = 11;
static const CEParamType_e CEParamTypeDouble = 12;
static const CEParamType_e CEParamTypeBuffer = 13;

static const CEParamType_e CEParamTypeMax = CEParamTypeBuffer;


#endif /* CEParamType_h */
