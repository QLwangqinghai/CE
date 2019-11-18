//
//  CCCircularBuffer.c
//  CCollection
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCCircularBuffer.h"

//4kb

/*
 32
 31
 p: 64*2kb
 pc: 32kb * 4
 usd: 128kb

 64
 63
 p: 8mb 23
 pc: 8mb (count: 1mb)  20
 p2c: 512kb = 4mb  21
 */


//uint8_t [a][b] 


void CCCircularBufferEnumerateCopyToBuffer(void * _Nullable context, CCRange range, size_t elementSize, const void * _Nonnull values) {
    uint8_t * ptr = context;
    assert(ptr);
    memcpy(ptr + range.location * elementSize, values, range.length * elementSize);
}
