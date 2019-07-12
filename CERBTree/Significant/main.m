//
//  main.m
//  Significant
//
//  Created by vector on 2019/6/26.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>


static const uint32_t __CCCircularBufferCapacitys[35] = {0, 0x4uL, 0x8uL, 0x10uL, 0x20uL, 0x40uL, 0x80uL, 0x100uL, 0x1c0uL, 0x310uL, 0x55cuL, 0x960uL, 0x1068uL, 0x1cb4uL, 0x3238uL, 0x57e0uL, 0x99c8uL, 0x10d1cuL, 0x1d6f0uL, 0x33824uL, 0x5a23cuL, 0x9dbe8uL, 0x1140d4uL, 0x1e3170uL, 0x34d684uL, 0x5c7764uL, 0xa1d0ecuL, 0x11b2d9cuL, 0x1ef8fd0uL, 0x3633bacuL, 0x5eda86cuL, 0xa5fe6bcuL, 0x1227d3c8uL, 0x1fc5b29cuL, 0x20000000uL};

static inline uint32_t __CCCircularBufferGoodCapacity(uint32_t capacity) {
    int low = 1;
    int high = 34;
    while (low <= high) {
        int mid = low + (high - low)/2;
        
        uint32_t right = __CCCircularBufferCapacitys[mid];
        uint32_t left = __CCCircularBufferCapacitys[mid-1];

        if (right > capacity && left <= capacity) {
            printf("[%x  %x  %x]\n", left, capacity, right);
            return right;
        } else {
            if (right < capacity) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
    }
    printf("max\n");

    return __CCCircularBufferCapacitys[34];
}


int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        int count = 0;
        uint32_t value = 1;

        while (true) {
            
            value += count;
            count += 1;
            
            uint32_t a = __CCCircularBufferGoodCapacity(value);
            if (a == 0x20000000ul) {
                break;
            }
        }
        
        
        for (uint32_t i=0; i<9; i++) {
            value = 1;
            value = value << i;
            printf("0x%xuL, ", value);
            count += 1;
        }
        
    }
    return 0;
}
