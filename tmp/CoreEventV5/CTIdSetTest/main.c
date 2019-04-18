//
//  main.c
//  CTIdSetTest
//
//  Created by vector on 2018/12/6.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

static const uint32_t CTIdInvalid = 0xFFFFFFFFul;


static uint64_t __CTIdSet[64] = {};
static uint64_t __CTIdSetRoot = 0;

static inline uint32_t CTGetUnusedIndex64(uint64_t n) {
    if (UINT64_MAX == n) {
        return UINT32_MAX;
    }
    
    uint32_t i = 0;
    uint32_t j = 63;
    uint32_t n1 = (uint32_t)(n >> 32);
    if (n1 == UINT32_MAX) {
        i += 32;
        n1 = (uint32_t)(n & UINT32_MAX);
    } else {
        j -= 32;
    }
    uint16_t n2 = (uint16_t)(n1 >> 16);
    if (n2 == UINT16_MAX) {
        i += 16;
        n2 = (uint16_t)(n1 & UINT16_MAX);
    } else {
        j -= 16;
    }
    uint8_t n3 = (uint8_t)(n2 >> 8);
    if (n3 == UINT8_MAX) {
        i += 8;
        n3 = (uint8_t)(n2 & UINT8_MAX);
    } else {
        j -= 8;
    }
    uint8_t n4 = (n3 >> 4);
    if (n4 == 0xf) {
        i += 4;
        n4 = (n3 & 0xf);
    } else {
        j -= 4;
    }
    uint8_t n5 = (n4 >> 2);
    if (n5 == 0x3) {
        i += 2;
        n5 = (n4 & 0x3);
    } else {
        j -= 2;
    }
    uint8_t n6 = (n5 >> 1);
    if (n6 == 0x1) {
        return j;
    } else {
        return i;
    }
}


static inline uint32_t __CTIdMake(void) {
    uint64_t mask = 0x8000000000000000ull;
    
    uint32_t id = CTIdInvalid;
    
    uint64_t n0 = __CTIdSetRoot;
    uint32_t offset0 = CTGetUnusedIndex64(n0);
    if (offset0 > 63) {
        return id;
    }
    
    uint32_t index1 = offset0;
    uint64_t n1 = __CTIdSet[index1];
    uint32_t offset1 = CTGetUnusedIndex64(n1);
    if (offset1 > 63) {
        abort();
    }
    
    n1 = n1 | (mask >> offset1);
    __CTIdSet[index1] = n1;
    
    if (UINT64_MAX == n1) {
        n0 = n0 | (mask >> offset0);
        __CTIdSetRoot = n0;
    }
    id = (uint32_t)((index1 << 6) + offset1);
    return id;
}

static inline void __CTIdFree(uint32_t id) {
    assert(id <= 4096);
    uint64_t mask = 0x8000000000000000ull;
    
    uint64_t n0 = __CTIdSetRoot;
    uint32_t offset0 = id >> 6;
    
    uint32_t index1 = offset0;
    uint32_t offset1 = id & 0x3F;
    uint64_t n1 = __CTIdSet[index1];
    
    
    n1 = n1 & (~(mask >> offset1));
    __CTIdSet[index1] = n1;
    
    n0 = n0 & (~(mask >> offset0));
    __CTIdSetRoot = n0;
}



int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    for (int i=0; i<5000; i++) {
        uint32_t id = __CTIdMake();
        
        printf("%d: %u\n", i, id);
    }
    for (int i=2000; i<4096; i++) {
        __CTIdFree(i);
    }
    __CTIdFree(188);

    for (int i=0; i<5000; i++) {
        uint32_t id = __CTIdMake();
        
        printf("%d: %u\n", i, id);
    }
    return 0;
}
