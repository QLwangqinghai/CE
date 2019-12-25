//
//  CCData.h
//  SITP
//
//  Created by vector on 2019/12/23.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCData_h
#define CCData_h

#include "CCBase.h"

CCIndex CCDataGetLength(CCDataRef _Nonnull theData);
const CCUInt8 * _Nullable CFDataGetBytePtr(CCDataRef _Nonnull theData);
void CCDataGetBytes(CCRef _Nonnull CCDataRef, CCRange range, void * _Nonnull buffer);










#endif /* CCData_h */
