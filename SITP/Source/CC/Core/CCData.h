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

typedef CCRef CCDataRef;

CCInt CCDataGetLength(CCRef _Nonnull CCDataRef);


CCInt CCDataGetBytes(CCRef _Nonnull CCDataRef, CCRange range, void * _Nonnull buffer);










#endif /* CCData_h */
