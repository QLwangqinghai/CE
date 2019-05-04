//
//  main.m
//  TGCD
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#import <Foundation/Foundation.h>


static dispatch_queue_t queue;

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            NSLog(@"11");
        });
        queue = dispatch_queue_create("convert", DISPATCH_QUEUE_CONCURRENT);
        dispatch_async(queue, ^{
            NSLog(@"121");

        });
        
        [[NSRunLoop currentRunLoop] run];
        
    }
    return 0;
}
