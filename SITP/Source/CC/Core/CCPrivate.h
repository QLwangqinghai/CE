//
//  CCPrivate.h
//  SITP
//
//  Created by vector on 2019/12/26.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCPrivate_h
#define CCPrivate_h

#include "CCBase.h"

typedef struct __CCStringDataBase {
    CCRefBase base;
    CCUInt length;
    void * content;
} CCStringDataBase;

#endif /* CCPrivate_h */
