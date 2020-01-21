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
 
 2020-01-20 00:26:48.073387+0800 T[256:4600] 64Mb memcpy 429.8ms(6.716ms/mb) custom:177.1ms(2.767ms/mb)
 2020-01-20 00:26:50.775034+0800 T[256:4600] 64Mb memcpy 19.2ms(0.300ms/mb) custom:22.9ms(0.357ms/mb)
 2020-01-20 00:26:54.085981+0800 T[256:4600] 64Mb memcpy 13.9ms(0.217ms/mb) custom:23.3ms(0.365ms/mb)
 2020-01-20 00:26:57.401179+0800 T[256:4600] 64Mb memcpy 13.3ms(0.207ms/mb) custom:22.1ms(0.345ms/mb)
 2020-01-20 00:27:00.691063+0800 T[256:4600] 64Mb memcpy 18.6ms(0.290ms/mb) custom:23.3ms(0.365ms/mb)
 
 
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
 
 2020-01-19 18:29:27.193625+0800 T[14974:5641596] 64Mb memcpy 54.6ms(0.853ms/mb) custom:17.8ms(0.278ms/mb)
 2020-01-19 18:29:30.316955+0800 T[14974:5641596] 64Mb memcpy 22.0ms(0.344ms/mb) custom:23.8ms(0.371ms/mb)
 2020-01-19 18:29:33.770982+0800 T[14974:5641596] 64Mb memcpy 21.6ms(0.337ms/mb) custom:21.2ms(0.331ms/mb)
 2020-01-19 18:29:37.067235+0800 T[14974:5641596] 64Mb memcpy 20.9ms(0.327ms/mb) custom:22.6ms(0.353ms/mb)
 2020-01-19 18:29:40.299417+0800 T[14974:5641596] 64Mb memcpy 22.1ms(0.345ms/mb) custom:21.5ms(0.337ms/mb)
 
 2020-01-20 00:33:42.275605+0800 T[15198:5716408] 64Mb memcpy 81.4ms(1.272ms/mb) custom:26.4ms(0.412ms/mb)
 2020-01-20 00:33:45.599670+0800 T[15198:5716408] 64Mb memcpy 22.9ms(0.357ms/mb) custom:21.0ms(0.328ms/mb)
 2020-01-20 00:33:48.807795+0800 T[15198:5716408] 64Mb memcpy 25.2ms(0.394ms/mb) custom:19.1ms(0.298ms/mb)
 2020-01-20 00:33:52.208108+0800 T[15198:5716408] 64Mb memcpy 22.8ms(0.356ms/mb) custom:20.8ms(0.324ms/mb)
 2020-01-20 00:33:55.297968+0800 T[15198:5716408] 64Mb memcpy 25.5ms(0.398ms/mb) custom:18.1ms(0.283ms/mb)
 
 2020-01-20 00:34:50.394001+0800 T[15204:5717522] 64Mb memcpy 48.7ms(0.760ms/mb) custom:13.7ms(0.215ms/mb)
 2020-01-20 00:34:53.428272+0800 T[15204:5717522] 64Mb memcpy 20.3ms(0.317ms/mb) custom:20.4ms(0.319ms/mb)
 2020-01-20 00:34:56.940957+0800 T[15204:5717522] 64Mb memcpy 25.6ms(0.400ms/mb) custom:19.9ms(0.310ms/mb)
 2020-01-20 00:35:00.218466+0800 T[15204:5717522] 64Mb memcpy 22.1ms(0.345ms/mb) custom:22.1ms(0.345ms/mb)
 2020-01-20 00:35:03.585370+0800 T[15204:5717522] 64Mb memcpy 23.5ms(0.367ms/mb) custom:21.8ms(0.340ms/mb)
 
 */


@interface TMemory ()

@property (nonatomic, copy) NSString * sizeInfo;
@property (nonatomic, assign) size_t sizeInMb;

@property (nonatomic, assign) size_t size;
@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) uint64_t * buffer;
@property (nonatomic, assign) uint64_t * buffer1;
@property (nonatomic, assign) uint64_t * changes;


@end
@implementation TMemory

- (instancetype)initWithSizeInMb:(NSInteger)sizeInMb {
    self = [super init];
    if (self) {
        self.sizeInfo = [NSString stringWithFormat:@"%ldMb", sizeInMb];
        self.sizeInMb = sizeInMb;
        NSInteger mb = 1024 * 1024;
        NSInteger size = sizeInMb * mb;
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

- (void)testMemcpy {
    uint64_t count = self.count;
    uint64_t countQuickMask = 0x7;
    countQuickMask = ~countQuickMask;

    uint64_t * buffer = self.buffer;
    uint64_t * buffer1 = self.buffer1;

    uint64_t * changes = self.changes;
    NSInteger offset = 0;

    NSTimeInterval b0 = CACurrentMediaTime();

    memcpy(buffer, buffer1, self.size);
    
    NSTimeInterval b = CACurrentMediaTime();
    
    uint64_t * ptrA = self.buffer;
    uint64_t * ptrB = self.buffer1;
    uint64_t * end = buffer + (count & countQuickMask);

    while (ptrA < end) {
        *ptrA = *ptrB;
        ptrA ++;
        ptrB ++;
        *ptrA = *ptrB;
        ptrA ++;
        ptrB ++;
        *ptrA = *ptrB;
        ptrA ++;
        ptrB ++;
        *ptrA = *ptrB;
        ptrA ++;
        ptrB ++;
        *ptrA = *ptrB;
        ptrA ++;
        ptrB ++;
        *ptrA = *ptrB;
        ptrA ++;
        ptrB ++;
        *ptrA = *ptrB;
        ptrA ++;
        ptrB ++;
        *ptrA = *ptrB;
        ptrA ++;
        ptrB ++;
    }
    
    end = buffer + count;
    while (ptrA < end) {
        *ptrA = *ptrB;
        ptrA ++;
        ptrB ++;
    }
    
    NSTimeInterval e = CACurrentMediaTime();
    NSTimeInterval t0 = b - b0;
    NSTimeInterval t1 = e - b;
    t0 = t0 * 1000;
    t1 = t1 * 1000;

    NSLog(@"%@ memcpy %.01lfms(%.03lfms/mb) custom:%.01lfms(%.03lfms/mb) ", self.sizeInfo, t0, t0 / self.sizeInMb, t1, t1 / self.sizeInMb);
    
    
    
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


-(void)dispatchSignal{
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(2);
    dispatch_queue_t quene = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
     
    //任务1
    dispatch_async(quene, ^{
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        NSLog(@"run task 1");
        sleep(1);
        NSLog(@"complete task 1");
        dispatch_semaphore_signal(semaphore);
        sleep(1);
    });
    //任务2
    dispatch_async(quene, ^{
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        NSLog(@"run task 2");
        sleep(10);
        NSLog(@"complete task 2");
        dispatch_semaphore_signal(semaphore);
    });
    //任务3
    dispatch_async(quene, ^{
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        NSLog(@"run task 3");
        sleep(1);
        NSLog(@"complete task 3");
        dispatch_semaphore_signal(semaphore);
    });
    
    //任务4
    dispatch_async(quene, ^{
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        NSLog(@"run task 4");
        sleep(1);
        NSLog(@"complete task 4");
        dispatch_semaphore_signal(semaphore);
    });
}



@end
