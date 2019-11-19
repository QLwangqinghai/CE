//
//  CCCircularBuffer.c
//  CCollection
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCCircularBuffer.h"


#if CBuild64Bit
const uint32_t __CCCircularBufferSizes[__CCCircularBufferSizeCount] = {0, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000};
#else
const uint32_t __CCCircularBufferSizes[__CCCircularBufferSizeCount] = {0, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000};
#endif

//4kb

/*
 32
 31
 p: 64*2kb
 pc: 32kb * 4
 usd: 128kb

 64
 63
 
 23 20 21
 p: 2mb
 pc: 8mb (count: 1mb)  20
 p2c: 512kb = 4mb  21
 */


//uint8_t [a][b] 

//存储层页码转化， 页码重映射，

void CCCircularBufferEnumerateCopyToBuffer(void * _Nullable context, CCRange range, size_t elementSize, const void * _Nonnull values) {
    uint8_t * ptr = context;
    assert(ptr);
    memcpy(ptr + range.location * elementSize, values, range.length * elementSize);
}
