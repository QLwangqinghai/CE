//
//  Memory.m
//  T
//
//  Created by vector on 2020/1/12.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "TMemory.h"
#import "T-Swift.h"


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
 
 
 ipad 5
 2020-01-22 10:19:30.939845+0800 T[6476:2069402] 256Mb memcpy 274.1ms(1.071ms/mb) custom:215.8ms(0.843ms/mb) threads 88.8ms(0.347ms/mb)
 2020-01-22 10:19:33.779286+0800 T[6476:2069402] 256Mb memcpy 47.8ms(0.187ms/mb) custom:54.0ms(0.211ms/mb) threads 54.7ms(0.213ms/mb)
 2020-01-22 10:19:37.103765+0800 T[6476:2069402] 256Mb memcpy 46.2ms(0.181ms/mb) custom:57.5ms(0.224ms/mb) threads 54.0ms(0.211ms/mb)
 2020-01-22 10:19:40.397942+0800 T[6476:2069402] 256Mb memcpy 48.1ms(0.188ms/mb) custom:53.8ms(0.210ms/mb) threads 54.1ms(0.211ms/mb)
 2020-01-22 10:19:43.496014+0800 T[6476:2069402] 256Mb memcpy 45.7ms(0.179ms/mb) custom:53.8ms(0.210ms/mb) threads 53.9ms(0.211ms/mb)
 2020-01-22 10:19:47.029713+0800 T[6476:2069402] 256Mb memcpy 43.1ms(0.168ms/mb) custom:54.1ms(0.211ms/mb) threads 57.9ms(0.226ms/mb)
 
 结论
 1. 多线程对于联系memcpy是没有什么用的 （取决于内存带宽？）
 2. 可以按照1mb/0.2ms）(5mb/s) 去计算
 
 
 
 文件读写
 
 2020-01-22 14:54:46.642322+0800 T[6525:2108241] 128Mb write 3173.1ms(24.790ms/mb) read:190.4ms(1.488ms/mb)
 2020-01-22 14:54:49.810542+0800 T[6525:2108241] 128Mb write 2898.6ms(22.645ms/mb) read:261.5ms(2.043ms/mb)
 2020-01-22 14:54:54.245485+0800 T[6525:2108241] 128Mb write 3691.2ms(28.837ms/mb) read:628.6ms(4.911ms/mb)
 2020-01-22 14:54:57.888634+0800 T[6525:2108241] 128Mb write 3388.4ms(26.472ms/mb) read:247.9ms(1.937ms/mb)
 2020-01-22 14:55:01.018993+0800 T[6525:2108241] 128Mb write 2961.5ms(23.137ms/mb) read:162.7ms(1.271ms/mb)
 2020-01-22 14:55:04.180936+0800 T[6525:2108241] 128Mb write 2967.3ms(23.182ms/mb) read:189.2ms(1.478ms/mb)
 写 40mb/s
 读取 500mb/s
 策略 同步读 异步写
 */

typedef uint64_t CCUInt64;

static inline void _CCMemoryQuickCopy(CCUInt64 * _Nonnull dst, const CCUInt64 * _Nonnull src, size_t count) {
    static size_t countQuickMask = ~((size_t)(0x7));

    if ((dst + count) <= src || (src + count) <= dst) {//没有重叠区域
        CCUInt64 * to = dst;
        const CCUInt64 * from = src;
        CCUInt64 * end = to + (count & countQuickMask);

        while (to < end) {
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
        }
        end = (CCUInt64 *)dst + count;
        while (to < end) {
            *to = *from;
            to ++;
            from ++;
        }
    } else {
        CCUInt64 * end = dst;
        end = end + (count & countQuickMask);
        CCUInt64 * to = end + count;
        const CCUInt64 * from = src;
        from = from + count;
        
        end = (CCUInt64 *)dst + count;
        while (to > end) {
            to --;
            from --;
            *to = *from;
        }
        
        end = dst;
        while (to > end) {
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
        }
    }
}

void CCMemoryCopy(void * _Nonnull dst, const void * _Nonnull src, size_t size) {
    assert(dst);
    assert(src);
    assert(size >= 0);
    uintptr_t from = (uintptr_t)dst;
    uintptr_t to = (uintptr_t)src;

    size_t offset = (from & 0x7);
    if (offset == (to & 0x7)) {
        uint8_t * dst8 = dst;
        const uint8_t * src8 = src;
        if (offset > 0) {
            memcpy(dst, src, offset);
            dst8 += offset;
            src8 += offset;
        }
        size_t remain = (size - offset);
        size_t count = remain / 8;
        if (count > 0) {
            _CCMemoryQuickCopy((CCUInt64 *)dst8, (const CCUInt64 *)src8, count);
        }
        remain = remain & 0x7;
        if (remain > 0) {
            memcpy(dst8 + (count * 8), src8 + (count * 8), remain);
        }
    } else {
        memcpy(dst, src, size);
    }
}


@interface TMemory ()

@property (nonatomic, copy) NSString * sizeInfo;
@property (nonatomic, assign) size_t sizeInMb;

@property (nonatomic, assign) size_t size;
@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) uint64_t * buffer;
@property (nonatomic, assign) uint64_t * buffer1;


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
        self.size = size;
        self.count = count;
        
        
        for (NSInteger i=0; i<count; i++) {
            if (self.buffer[i] != self.buffer1[i]) {
                
            }
        }
    }
    return self;
}

- (void)testMemcpy {
    NSInteger mb = 1024 * 1024;


    uint64_t count = self.count;
    uint64_t countQuickMask = 0x7;
    countQuickMask = ~countQuickMask;

    uint64_t * buffer = self.buffer;
    uint64_t * buffer1 = self.buffer1;

    NSInteger offset = 0;

    NSTimeInterval b0 = CACurrentMediaTime();

    memcpy(buffer, buffer1, self.size);
    
    NSTimeInterval b = CACurrentMediaTime();
    
//    for (NSInteger idx=0; idx<self.sizeInMb / 2; idx++) {
//        NSInteger offset = (2 * mb / 8) * idx;
//        CCMemoryCopy(buffer + offset, buffer1 + offset, 2 * mb);
//    }
    CCMemoryCopy(buffer, buffer1, self.size);
    
    NSTimeInterval e = CACurrentMediaTime();
    NSTimeInterval t0 = b - b0;
    NSTimeInterval t1 = e - b;
    t0 = t0 * 1000;
    t1 = t1 * 1000;

    
    TaskHelper * helper = [[TaskHelper alloc] init];
    NSTimeInterval gb = CACurrentMediaTime();

    dispatch_group_t g = dispatch_group_create();

    for (NSInteger idx=0; idx<self.sizeInMb / 8; idx++) {
        NSInteger offset = mb * idx;
        dispatch_group_enter(g);
        [helper appendWithTask:^{
            CCMemoryCopy(buffer + offset, buffer1 + offset, 8 * mb);
            dispatch_group_leave(g);
        }];
    }
    [helper commit];
    dispatch_group_wait(g, DISPATCH_TIME_FOREVER);

    NSTimeInterval ge = CACurrentMediaTime();
    NSTimeInterval gt = ge - gb;
    gt = gt * 1000;

    NSMutableString * string = [NSMutableString string];
    [string appendFormat:@"%@ memcpy %.01lfms(%.03lfms/mb) custom:%.01lfms(%.03lfms/mb) threads %.01lfms(%.03lfms/mb)", self.sizeInfo, t0, t0 / self.sizeInMb, t1, t1 / self.sizeInMb, gt, gt / self.sizeInMb];
    NSLog(string);
    [Label shared].text = string;
}
- (void)test {
    // Do any additional setup after loading the view.
    
//    NSInteger pCount = (1024 * 1024 * 2) * 8;
//    NSInteger count = count * 4 / sizeof(uint64_t);
//    NSInteger size = count * 4 / sizeof(uint64_t);

    
    
    
    NSInteger count = self.count;

    uint64_t * buffer = self.buffer;
    uint64_t * buffer1 = self.buffer1;

    NSInteger offset = 0;

    NSTimeInterval b0 = CACurrentMediaTime();

    memcpy(buffer, buffer1, self.size);
    
    NSTimeInterval b = CACurrentMediaTime();
    
    NSLog(@"memset %.06lf", b - b0);


    NSTimeInterval e = CACurrentMediaTime();

    NSLog(@"comapre %.06lf", e - b);
}



@end




@interface TFile ()

@property (nonatomic, copy) NSString * sizeInfo;
@property (nonatomic, assign) size_t sizeInMb;

@property (nonatomic, assign) size_t size;
@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) uint64_t * buffer;

@end
@implementation TFile

- (instancetype)initWithSizeInMb:(NSInteger)sizeInMb {
    self = [super init];
    if (self) {
        self.sizeInfo = [NSString stringWithFormat:@"%ldMb", sizeInMb];
        self.sizeInMb = sizeInMb;
        NSInteger mb = 1024 * 1024;
        NSInteger size = sizeInMb * mb;
        NSInteger count = size / sizeof(uint64_t);

        self.buffer = malloc(size);
        self.size = size;
        self.count = count;
        
        
        for (NSInteger i=0; i<count; i++) {
            self.buffer[i] = i;
        }
    }
    return self;
}

- (void)test {
    NSInteger tag = arc4random();
    NSString * filePath = [NSString stringWithFormat:@"%@/Library/%ld", NSHomeDirectory(), tag];
    NSData * data = [NSData dataWithBytesNoCopy:self.buffer length:self.size freeWhenDone:false];
    NSTimeInterval b0 = CACurrentMediaTime();
    [data writeToFile:filePath atomically:true];
    NSTimeInterval b = CACurrentMediaTime();
    NSData * read = [NSData dataWithContentsOfFile:filePath];
    NSTimeInterval e = CACurrentMediaTime();
    NSTimeInterval t0 = b - b0;
    NSTimeInterval t1 = e - b;
    t0 = t0 * 1000;
    t1 = t1 * 1000;

    NSMutableString * string = [NSMutableString string];
    [string appendFormat:@"%@ write %.01lfms(%.03lfms/mb) read:%.01lfms(%.03lfms/mb)", self.sizeInfo, t0, t0 / self.sizeInMb, t1, t1 / self.sizeInMb];
    NSLog(string);
    [Label shared].text = string;
}



@end
