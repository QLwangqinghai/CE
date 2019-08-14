//
//  CInteger.c
//  CoreDigest
//
//  Created by vector on 2019/8/11.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CInteger.h"

const CUInt128_t CUInt128Zero = {};


////arm uint32_t
//static inline uint32_t CUInt32ByteSwap(uint32_t val) {
//    __asm__("bswap %0" : "=r" (val) : "0" (val));
//    return val;
//}
//
//
////arm uint64_t
//static inline uint64_t CUInt64ByteSwap64(uint64_t val) {
//#ifdef __X64
//    __asm__("bswapq %0" : "=r" (val) : "0" (val));
//#else
//
//    uint64_t lower = val & 0xffffffffU;
//    uint32_t higher = (val >> 32) & 0xffffffffU;
//
//    lower = CUInt32ByteSwap(lower);
//    higher = CUInt32ByteSwap(higher);
//
//    return (lower << 32) + higher;
//
//#endif
//    return val;
//}


