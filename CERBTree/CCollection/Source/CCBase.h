//
//  CCBase.h
//  CCollection
//
//  Created by vector on 2019/6/26.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCBase_h
#define CCBase_h



#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include "CInteger.h"
#include "CType.h"
#include "CConfig.h"

typedef struct __CCRange {
    uint64_t location;
    uint64_t length;
} CCRange_t;

#endif /* CCBase_h */
