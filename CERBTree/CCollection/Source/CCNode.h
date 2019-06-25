//
//  CCNode.h
//  CCollection
//
//  Created by vector on 2019/6/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCNode_h
#define CCNode_h

#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <errno.h>





struct __CCNode;
typedef struct __CCNode CCNode_s;
typedef CCNode_s * CCNodeRef;



struct __CCNode {
    
    //    CFRuntimeBase _base;
    CCNodeRef _Nullable parent;    /* Not retained */
    CCNodeRef _Nullable sibling;    /* Not retained */
    CCNodeRef _Nullable leftmostChild;    /* All children get a retain from the parent */
    CCNodeRef _Nullable rightmostChild;    /* Not retained */
};


#endif /* CCNode_h */
