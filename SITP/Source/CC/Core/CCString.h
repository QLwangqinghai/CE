//
//  CCString.h
//  CCollection
//
//  Created by vector on 2019/7/4.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCString_h
#define CCString_h

#include "CCValue.h"




#pragma pack(push,1)

typedef struct {
    CCValueBase base;
    CCHashCode hashCode;
} CCStringBase;

typedef struct {
    CCStringBase base;
    CCUInt length;
    const CCU16Char * _Nonnull chars;
} CCConstantString;

#pragma pack(pop)

extern CCStringRef _Nonnull CCEmptyString;




#endif /* CCString_h */
