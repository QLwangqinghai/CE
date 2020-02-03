//
//  COHashMap.h
//  CoreObject
//
//  Created by vector on 2018/12/11.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef COHashMap_h
#define COHashMap_h

#include <stdio.h>

// Prime numbers. Values above 100 have been adjusted up so that the
// malloced block size will be just below a multiple of 512; values
// above 1200 have been adjusted up to just below a multiple of 4096.

num_buckets

static const uint64_t __COBasicHashTableBucketNumbers[64] = {
    0, 3, 7, 13, 23, 41, 71, 127,
    191, 251, 383, 631, 1087, 1723, 2803, 4523,
    7351, 11959, 19447, 31231, 50683, 81919, 132607, 214519,
    346607, 561109, 907759, 1468927, 2376191, 3845119, 6221311, 10066421,
    16287743, 26354171, 42641881, 68996069, 111638519, 180634607, 292272623, 472907251,
    765180413UL, 1238087663UL, 2003267557UL, 3241355263UL, 5244622819UL,
};

static const uint64_t __COBasicHashTableCapacities[64] = {
    0, 3, 6, 11, 19, 32, 52, 85,
    118, 155, 237, 390, 672, 1065, 1732, 2795,
    4543, 7391, 12019, 19302, 31324, 50629, 81956, 132580,
    214215, 346784, 561026, 907847, 1468567, 2376414, 3844982, 6221390,
    10066379, 16287773, 26354132, 42641916, 68996399, 111638327, 180634415, 292272755,
    472907503UL, 765180257UL, 1238087439UL, 2003267722UL, 3241355160UL,
};

static inline void aa () {
    int a;
    __typeof(a)
}





#endif /* COHashMap_h */
