//
//  CoreLayout.h
//  Layout
//
//  Created by vector on 2019/9/11.
//  Copyright © 2019 vector. All rights reserved.
//

#ifndef CoreLayout_h
#define CoreLayout_h

#include <stdio.h>
#include <stdbool.h>

#pragma pack (push,1)

typedef struct {
    int32_t x;
    int32_t y;
} CLVector_t;

typedef struct {
    CLVector_t origin;
    CLVector_t size;
} CLRect_t;

#pragma pack(pop)

static inline _Bool CLVectorEqualToVector(CLVector_t v1, CLVector_t v2) {
    return v1.x == v2.x && v1.y == v2.y;
}


#endif /* CoreLayout_h */
