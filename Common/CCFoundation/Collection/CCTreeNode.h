//
//  CCTreeNode.h
//  CCollection
//
//  Created by vector on 2019/6/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCTreeNode_h
#define CCTreeNode_h


#include "CCBase.h"


typedef struct __CCNodeBase {
    _Atomic(uint32_t) ref;
    uint32_t type: 5;
    uint32_t size;
} CCNodeBase_s;


struct __CCNode;
typedef struct __CCNode CCNode_s;
typedef CCNode_s * CCNodeRef;


#if CCBuild64Bit
#pragma pack(push, 8)
#else
#pragma pack(push, 4)
#endif

struct __CCNode {
    CCNodeBase_s _base;
//    CCNodeRef _Nullable parent;    /* Not retained */
//    CCNodeRef _Nullable sibling;    /* Not retained */
//    CCNodeRef _Nullable leftmostChild;    /* All children get a retain from the parent */
//    CCNodeRef _Nullable rightmostChild;    /* Not retained */
};

#pragma pack(pop)






#endif /* CCNode_h */
