//
//  CCData.h
//  SITP
//
//  Created by vector on 2019/12/23.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCData_h
#define CCData_h

#include "CCValue.h"

#pragma pack(push,1)
typedef struct {
    CCValueBase base;
    CCHashCode hashCode;
} CCDataBase;

typedef struct {
    CCDataBase base;
    CCUInt length;
    const CCUInt8 * _Nonnull bytes;
} CCConstantData;
#pragma pack(pop)



CCIndex CCDataGetLength(CCDataRef _Nonnull theData);
const CCUInt8 * _Nullable CFDataGetBytePtr(CCDataRef _Nonnull theData);
void CCDataGetBytes(CCRef _Nonnull CCDataRef, CCRange range, void * _Nonnull buffer);










#endif /* CCData_h */
