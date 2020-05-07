//
//  CXTester.c
//  X
//
//  Created by vector on 2020/5/7.
//  Copyright © 2020 haoqi. All rights reserved.
//

#import "CXTester.h"

#include "CX.h"

#define byte0 ((const uint8_t *)(data.bytes))
#define byte1 ((const uint8_t *)(data1.bytes))

@implementation CXTester

+ (void)test0 {
    NSData * d = [@"adsfajkrjewmfaksfl;akflkalrwrwrwr" dataUsingEncoding:NSUTF8StringEncoding];
    NSData * d2 = [@"adsfajkrjewmfaksfl;1akflkalrwrwrwr1" dataUsingEncoding:NSUTF8StringEncoding];
    NSData * d3 = [@"adsfajkrjewmfaksfl;2akflkalrwrwrwr2" dataUsingEncoding:NSUTF8StringEncoding];
    NSMutableData * tmp = [NSMutableData data];

    NSMutableData * data = [NSMutableData data];
    NSMutableData * data1 = [NSMutableData data];
    
    for (int i=0; i<15000000; i++) {
        [tmp appendData:d];
    }
    
    [data appendData:tmp];
    [data1 appendData:tmp];
    
    [data appendData:d2];
    [data1 appendData:d3];
    size_t size = data.length - 8;

    XMemoryCompare(byte0, byte1, size);

    
    
    CFTimeInterval time = CFAbsoluteTimeGetCurrent();
    
    int a = memcmp(byte0, byte1, size);
    NSLog(@"%d", a);
    CFTimeInterval time1 = CFAbsoluteTimeGetCurrent();
        
    XMemoryCompare(byte0, byte1, size);
    
    CFTimeInterval time2 = CFAbsoluteTimeGetCurrent();
    
    XMemoryCompare(byte0 + 1, byte1 + 1, size);

    CFTimeInterval time3 = CFAbsoluteTimeGetCurrent();

    XMemoryCompare(byte0 + 2, byte1 + 2, size);

    CFTimeInterval time4 = CFAbsoluteTimeGetCurrent();
    
    XMemoryCompare(byte0 + 4, byte1 + 4, size);

    CFTimeInterval time5 = CFAbsoluteTimeGetCurrent();
    
    XMemoryCompare(byte0 + 7, byte1 + 7, size);

    CFTimeInterval time6 = CFAbsoluteTimeGetCurrent();
    
    XMemoryCompare(byte0 + 5, byte1 + 5, size);

    CFTimeInterval time7 = CFAbsoluteTimeGetCurrent();
    
    NSLog(@"len: %ld  %ld", data.length, data1.length);
    NSLog(@"time: %.03lf %.03lf %.03lf %.03lf %.03lf %.03lf %.03lf", time1 - time, time2 - time1, time3 - time2, time4 - time3, time5 - time4, time6 - time5, time7 - time6);

}
@end
