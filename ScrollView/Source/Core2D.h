//
//  Core2D.h
//  ScrollView
//
//  Created by vector on 2019/8/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef Core2D_h
#define Core2D_h

#include <stdio.h>


typedef struct {
    int32_t x;
    int32_t y;
} C2DPoint;

typedef struct {
    int32_t width;
    int32_t height;
} C2DSize;

//typedef struct {
//    C2DPoint origin;
//    C2DSize size;
//} C2DRect;



typedef struct {
    int64_t x;
    int64_t y;
} C2DPoint64;

typedef struct {
    int64_t width;
    int64_t height;
} C2DSize64;

//typedef struct {
//    C2DPoint64 origin;
//    C2DSize64 size;
//} C2DRect64;


typedef struct {
    int32_t top, left, bottom, right;
} C2DEdgeInsets;

static inline C2DEdgeInsets C2DEdgeInsetsMake(int32_t top, int32_t left, int32_t bottom, int32_t right) {
    C2DEdgeInsets insets = {top, left, bottom, right};
    return insets;
}


//static inline C2DRect C2DEdgeInsetsInsetRect(C2DRect rect, C2DEdgeInsets insets) {
//    rect.origin.x    += insets.left;
//    rect.origin.y    += insets.top;
//    rect.size.width  -= (insets.left + insets.right);
//    rect.size.height -= (insets.top  + insets.bottom);
//    return rect;
//}

static inline _Bool C2DEdgeInsetsEqualToEdgeInsets(C2DEdgeInsets insets1, C2DEdgeInsets insets2) {
    return insets1.left == insets2.left && insets1.top == insets2.top && insets1.right == insets2.right && insets1.bottom == insets2.bottom;
}





#endif /* Core2D_h */
