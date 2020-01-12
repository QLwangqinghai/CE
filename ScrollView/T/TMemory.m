//
//  Memory.m
//  T
//
//  Created by vector on 2020/1/12.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "TMemory.h"


/*
 64mb
 debug: 80ms
 release: 50ms
 
 6plus
 
 release
 
 
 0.02
 
 2020-01-12 17:19:08.655357+0800 T[1991:1056523] memset 0.079952
 2020-01-12 17:19:08.747602+0800 T[1991:1056523] comapre 0.092256
 2020-01-12 17:19:11.750801+0800 T[1991:1056523] memset 0.011747
 2020-01-12 17:19:11.797969+0800 T[1991:1056523] comapre 0.047187
 2020-01-12 17:19:15.230342+0800 T[1991:1056523] memset 0.006127
 2020-01-12 17:19:15.250125+0800 T[1991:1056523] comapre 0.019785
 2020-01-12 17:19:18.331897+0800 T[1991:1056523] memset 0.007611
 2020-01-12 17:19:18.352290+0800 T[1991:1056523] comapre 0.020389
 2020-01-12 17:19:21.821097+0800 T[1991:1056523] memset 0.010899
 2020-01-12 17:19:21.837041+0800 T[1991:1056523] comapre 0.015945
 
 debug
 2020-01-12 17:23:27.338413+0800 T[2008:1057453] memset 0.107369
 2020-01-12 17:23:27.510253+0800 T[2008:1057453] comapre 0.171863
 2020-01-12 17:23:30.613705+0800 T[2008:1057453] memset 0.006814
 2020-01-12 17:23:30.672165+0800 T[2008:1057453] comapre 0.058468
 2020-01-12 17:23:33.997448+0800 T[2008:1057453] memset 0.007533
 2020-01-12 17:23:34.053788+0800 T[2008:1057453] comapre 0.056368
 2020-01-12 17:23:37.226516+0800 T[2008:1057453] memset 0.006945
 2020-01-12 17:23:37.282337+0800 T[2008:1057453] comapre 0.055833
 2020-01-12 17:23:40.596454+0800 T[2008:1057453] memset 0.011382
 2020-01-12 17:23:40.654546+0800 T[2008:1057453] comapre 0.058111
 
 X
 debug
 2020-01-12 17:25:36.726216+0800 T[11296:4358932] memset 0.025721
 2020-01-12 17:25:36.789587+0800 T[11296:4358932] comapre 0.063395
 2020-01-12 17:25:40.004551+0800 T[11296:4358932] memset 0.015525
 2020-01-12 17:25:40.069114+0800 T[11296:4358932] comapre 0.064583
 2020-01-12 17:25:43.219447+0800 T[11296:4358932] memset 0.013742
 2020-01-12 17:25:43.283907+0800 T[11296:4358932] comapre 0.064472
 2020-01-12 17:25:46.600894+0800 T[11296:4358932] memset 0.013737
 2020-01-12 17:25:46.666273+0800 T[11296:4358932] comapre 0.065393
 2020-01-12 17:25:49.925362+0800 T[11296:4358932] memset 0.013669
 2020-01-12 17:25:49.990357+0800 T[11296:4358932] comapre 0.065004
 
 release
 2020-01-12 17:26:27.074358+0800 T[11299:4359421] memset 0.035321
 2020-01-12 17:26:27.102542+0800 T[11299:4359421] comapre 0.028199
 2020-01-12 17:26:30.331909+0800 T[11299:4359421] memset 0.013444
 2020-01-12 17:26:30.363404+0800 T[11299:4359421] comapre 0.031506
 2020-01-12 17:26:33.516634+0800 T[11299:4359421] memset 0.013895
 2020-01-12 17:26:33.548944+0800 T[11299:4359421] comapre 0.032319
 2020-01-12 17:26:37.017331+0800 T[11299:4359421] memset 0.014658
 2020-01-12 17:26:37.042855+0800 T[11299:4359421] comapre 0.025515
 2020-01-12 17:26:40.219138+0800 T[11299:4359421] memset 0.014432
 2020-01-12 17:26:40.250114+0800 T[11299:4359421] comapre 0.030968

 */


@interface TMemory ()


@property (nonatomic, assign) size_t size;
@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) uint64_t * buffer;
@property (nonatomic, assign) uint64_t * buffer1;
@property (nonatomic, assign) uint64_t * changes;


@end
@implementation TMemory

- (instancetype)init {
    self = [super init];
    if (self) {

        NSInteger mb = 1024 * 1024;
        NSInteger size = 64 * mb;
        NSInteger count = size / sizeof(uint64_t);

        self.buffer = malloc(size);
        self.buffer1 = malloc(size);
        self.changes = malloc(size * 3);
        self.size = size;
        self.count = count;
        
        
        for (NSInteger i=0; i<count; i++) {
            if (self.buffer[i] != self.buffer1[i]) {
                
            }
            
            BOOL A = self.changes[i* 3] == self.changes[i* 3 + 1];
            BOOL B = self.changes[i* 3 + 2] == self.changes[i* 3 + 1];
            BOOL C = A == B;
        }
    }
    return self;
}


- (void)test {
    // Do any additional setup after loading the view.
    
//    NSInteger pCount = (1024 * 1024 * 2) * 8;
//    NSInteger count = count * 4 / sizeof(uint64_t);
//    NSInteger size = count * 4 / sizeof(uint64_t);

    
    
    
    NSInteger count = self.count;

    uint64_t * buffer = self.buffer;
    uint64_t * buffer1 = self.buffer1;

    uint64_t * changes = self.changes;
    NSInteger offset = 0;

    NSTimeInterval b0 = CACurrentMediaTime();

    memcpy(buffer, buffer1, self.size);
    
    NSTimeInterval b = CACurrentMediaTime();
    
    NSLog(@"memset %.06lf", b - b0);

    for (NSInteger i=0; i<count; i++) {
        if (buffer[i] != buffer1[i]) {
            changes[offset] = i;
            offset += 1;
            changes[offset] = buffer[i];
            offset += 1;
            changes[offset] = buffer1[i];
            offset += 1;
        }
    }
    NSTimeInterval e = CACurrentMediaTime();

    NSLog(@"comapre %.06lf", e - b);
}



@end
