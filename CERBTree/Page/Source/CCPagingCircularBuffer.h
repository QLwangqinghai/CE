//
//  CCPagingCircularBuffer.h
//  Page
//
//  Created by vector on 2019/11/20.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCPagingCircularBuffer_h
#define CCPagingCircularBuffer_h

#include <stdio.h>

typedef uint64_t CCIndex;
typedef uint8_t * CCPagePtr;


typedef struct {
    void * _Nonnull _section;
} CCPageSection_s;

typedef struct {
    uint32_t _offset;
    uint32_t _capacity;
    uint32_t _count;
    CCPageSection_s * _Nonnull _sections;
} CCPageSectionBuffer_s;


















#endif /* CCPagingCircularBuffer_h */
