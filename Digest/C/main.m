//
//  main.m
//  C
//
//  Created by vector on 2019/8/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <math.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        
        NSLog(@"%x", (uint8_t)(-1));
        NSLog(@"%x", (uint8_t)(-2));
        NSLog(@"%x", (uint8_t)(-128));

        uint32_t a = 1;
        int16_t b = -1;
        uint32_t d = (uint32_t)b;

        uint32_t c = a - b;

        
    }
    return 0;
}
