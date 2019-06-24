//
//  CTreeNode.h
//  CTree
//
//  Created by vector on 2019/6/24.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CTreeNode_h
#define CTreeNode_h

#include <stdio.h>


struct __CFTreeNode;
typedef struct __CFTreeNode CFTreeNode_s;
typedef CFTreeNode_s * CFTreeNodeRef;



struct __CTreeNode {
    
//    CFRuntimeBase _base;
    CFTreeNodeRef _Nullable parent;    /* Not retained */
    CFTreeNodeRef _Nullable sibling;    /* Not retained */
    CFTreeNodeRef _Nullable leftmostChild;    /* All children get a retain from the parent */
    CFTreeNodeRef _Nullable rightmostChild;    /* Not retained */

};


#endif /* CTreeNode_h */
