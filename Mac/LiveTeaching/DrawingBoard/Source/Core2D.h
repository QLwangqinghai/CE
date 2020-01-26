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
#include <stdbool.h>


#if !defined(MIN)
    #define MIN(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#endif

#if !defined(MAX)
    #define MAX(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif

typedef struct {
    int32_t x;
    int32_t y;
} C2DPoint;

static inline _Bool C2DPointEqualPoint(C2DPoint lhs, C2DPoint rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

typedef struct {
    uint32_t width;
    uint32_t height;
} C2DSize;

static inline _Bool C2DSizeEqualSize(C2DSize lhs, C2DSize rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

typedef struct {
    C2DPoint origin;
    C2DSize size;
} C2DRect;

static inline C2DRect C2DRectStandardize(C2DRect rect) {
    if (rect.size.width < 0) {
        rect.origin.x += rect.size.width;
        rect.size.width *= -1;
    }
    if (rect.size.height < 0) {
        rect.origin.y += rect.size.height;
        rect.size.height *= -1;
    }
    return rect;
}

//并集
static inline _Bool C2DRectUnion(C2DRect rect, C2DRect rect1, C2DRect * _Nonnull result) {
    rect = C2DRectStandardize(rect);
    rect1 = C2DRectStandardize(rect1);
    C2DRect tmp = {};
    tmp.origin.x = MIN(rect.origin.x, rect1.origin.x);
    tmp.origin.y = MIN(rect.origin.y, rect1.origin.y);
    tmp.size.width = MAX(rect.origin.x + rect.size.width, rect1.origin.x + rect1.size.width) - tmp.origin.x;
    tmp.size.height = MAX(rect.origin.y + rect.size.height, rect1.origin.y + rect1.size.height) - tmp.origin.y;
    if (tmp.size.width > 0 && tmp.size.height > 0) {
        *result = tmp;
        return true;
    }
    return false;
}

//交集
static inline _Bool C2DRectIntersect(C2DRect rect, C2DRect rect1, C2DRect * _Nonnull result) {
    rect = C2DRectStandardize(rect);
    rect1 = C2DRectStandardize(rect1);
    C2DRect tmp = {};
    tmp.origin.x = MAX(rect.origin.x, rect1.origin.x);
    tmp.origin.y = MAX(rect.origin.y, rect1.origin.y);
    tmp.size.width = MIN(rect.origin.x + rect.size.width, rect1.origin.x + rect1.size.width) - tmp.origin.x;
    tmp.size.height = MIN(rect.origin.y + rect.size.height, rect1.origin.y + rect1.size.height) - tmp.origin.y;
    if (tmp.size.width > 0 && tmp.size.height > 0) {
        *result = tmp;
        return true;
    }
    return false;
}


static inline C2DRect C2DRectMake(int32_t x, int32_t y, int32_t width, int32_t height) {
    C2DRect result = {
        .origin = {
            .x = x,
            .y = y,
        },
        .size = {
            .width = width,
            .height = height,
        },
    };
    return result;
}

typedef struct {
    int32_t location;
    int32_t length;
} C2DBytesRange;

static inline _Bool C2DRectEqualRect(C2DRect lhs, C2DRect rhs) {
    lhs = C2DRectStandardize(lhs);
    rhs = C2DRectStandardize(rhs);
    return C2DPointEqualPoint(lhs.origin, rhs.origin) && C2DSizeEqualSize(lhs.size, rhs.size);
}

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



int32_t C2DGetCachelineSize(void);

#endif /* Core2D_h */
