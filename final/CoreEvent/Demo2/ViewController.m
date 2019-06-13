//
//  ViewController.m
//  Demo2
//
//  Created by vector on 2019/5/3.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#import "ViewController.h"
#include "CETime.h"

#pragma pack(push)
#pragma pack(1)

struct __CETypeBaseInfo111 {
    uint8_t objectSize1;
    uint16_t objectSize2;
};

#pragma pack(pop)

struct __CETypeBaseInfo112 {
    uint8_t objectSize1;
    uint16_t objectSize2;
};

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
//    CESemRef sem = CESemInit(0);
//
//    NSLog(@"%ld", sizeof(struct __CETypeBaseInfo111));
//    NSLog(@"%ld", sizeof(struct __CETypeBaseInfo112));

    dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
    NSMutableSet * set = [NSMutableSet set];
    for (NSInteger i=0; i<1000; i++) {
        dispatch_async(queue, ^{
            [set addObject:[NSThread currentThread]];
            sleep(1);
            NSLog(@"%ld exit", i);
        });
        if (i==1000-1) {
            dispatch_async(queue, ^{
                [set addObject:[NSThread currentThread]];
                sleep(5);
                NSLog(@"%ld exit", i);
                NSLog(@"count: %@, set: %@", @(set.count), set);
                
                [set enumerateObjectsUsingBlock:^(NSThread *  _Nonnull t, BOOL * _Nonnull stop) {
                    NSLog(@"thread: %@, isExecuting: %@, isCancelled: %@, isFinished: %@", t, @(t.isExecuting), @(t.isCancelled), @(t.isFinished));

                }];
            });
        }
    }
    return;
    
    NSInteger i=3;
    
    while (i < 243) {
        int j=0;
        while (j < 16) {
            j ++;
            printf("%ld, ", i);
            
            i++;
        }
        
        printf("\n");

    }

    NSInteger count = 1024;
    CFAbsoluteTime beginTime = CFAbsoluteTimeGetCurrent();
    NSLog(@"beginTime %lf", beginTime);
    
    for (NSInteger i=0; i<128; i++) {
        for (NSInteger j=0; j<count; j++) {
            for (NSInteger k=0; k<count; k++) {
                struct timespec t = CESystemBootInterval();
            }
        }
//        NSLog(@"i: %ld %lf", i, CFAbsoluteTimeGetCurrent());
    }
    
    CFAbsoluteTime endTime = CFAbsoluteTimeGetCurrent();
    NSLog(@"endTime %lf", endTime);
    NSLog(@"used %lf, per: %lf", endTime - beginTime, (endTime - beginTime)/(double)128.0);

//
    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
    
}


@end
