//
//  AppDelegate.m
//  TestOC
//
//  Created by vector on 2018/12/13.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#import "AppDelegate.h"
#include <semaphore.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#import <CommonCrypto/CommonCryptor.h>

#include <mach/mach_time.h>


typedef struct __CSMachTimeBaseInfo {
    long double frac;
    bool ratio_1_to_1;
} CSMachTimeBaseInfo_s;

static CSMachTimeBaseInfo_s __CSMachTimeBaseInfo = {};

static void CSMachTimeBaseInfoInit(mach_timebase_info_data_t * _Nonnull tbi) {
    __CSMachTimeBaseInfo.frac = tbi->numer;
    __CSMachTimeBaseInfo.frac /= tbi->denom;
    __CSMachTimeBaseInfo.ratio_1_to_1 = (tbi->numer == tbi->denom);
}

void __CSMachTimeBaseInfoShareOnceBlockFunc(void) {
    mach_timebase_info_data_t tbi;
    assert(0 == mach_timebase_info(&tbi));
    CSMachTimeBaseInfoInit(&tbi);
}
void __CSMachTimeBaseInfoOnceInit(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CSMachTimeBaseInfoShareOnceBlockFunc);
}

const CSMachTimeBaseInfo_s * _Nonnull CSMachTimeBaseInfo(void) {
    __CSMachTimeBaseInfoOnceInit();
    return &__CSMachTimeBaseInfo;
}

static inline struct timespec CSMachTimeToTime(uint64_t machtime) {
    struct timespec time;
    const CSMachTimeBaseInfo_s * data = CSMachTimeBaseInfo();
    if (!machtime || data->ratio_1_to_1) {
        time.tv_sec = machtime / NSEC_PER_SEC;
        time.tv_nsec = machtime % NSEC_PER_SEC;
        return time;
    }
    long double big_tmp = ((long double)machtime * data->frac) + .5L;
    long double big_tmp_sec = big_tmp / 1000000000.0L;
    
    if (big_tmp_sec >= INT64_MAX) {
        time.tv_sec = INT64_MAX;
        return time;
    }
    long sec = (long)big_tmp_sec;
    big_tmp_sec -= sec;
    time.tv_sec = sec;
    time.tv_nsec = (long)(big_tmp_sec * 1000000000.0L);
    
    return time;
}

struct timespec CSBootInterval(void) {
    uint64_t t = mach_continuous_time();
    return CSMachTimeToTime(t);
}

//
//void AES256Encrypt(void * key) {
//    // 'key' should be 32 bytes for AES256, will be null-padded otherwise
//    char keyPtr[kCCKeySizeAES256+1]; // room for terminator (unused)
//    bzero(keyPtr, sizeof(keyPtr)); // fill with zeroes (for padding)
//
//    // fetch key data
//    [key getCString:keyPtr maxLength:sizeof(keyPtr) encoding:NSUTF8StringEncoding];
//
//    NSUInteger dataLength = [self length];
//
//    //See the doc: For block ciphers, the output size will always be less than or
//    //equal to the input size plus the size of one block.
//    //That's why we need to add the size of one block here
//    size_t bufferSize = dataLength + kCCBlockSizeAES128;
//    void *buffer = malloc(bufferSize);
//
//    size_t numBytesEncrypted = 0;
//    CCCryptorStatus cryptStatus = CCCrypt(kCCEncrypt, kCCAlgorithmAES128, kCCOptionPKCS7Padding,
//                                          keyPtr, kCCKeySizeAES256,
//                                          NULL /* initialization vector (optional) */,
//                                          [self bytes], dataLength, /* input */
//                                          buffer, bufferSize, /* output */
//                                          &numBytesEncrypted);
//    if (cryptStatus == kCCSuccess) {
//        //the returned NSData takes ownership of the buffer and will free it on deallocation
//        return [NSData dataWithBytesNoCopy:buffer length:numBytesEncrypted];
//    }
//
//    free(buffer); //free the buffer;
//    return nil;
//}
//
//- (NSData *)AES256DecryptWithKey:(NSString *)key {
//    // 'key' should be 32 bytes for AES256, will be null-padded otherwise
//    char keyPtr[kCCKeySizeAES256+1]; // room for terminator (unused)
//    bzero(keyPtr, sizeof(keyPtr)); // fill with zeroes (for padding)
//
//    // fetch key data
//    [key getCString:keyPtr maxLength:sizeof(keyPtr) encoding:NSUTF8StringEncoding];
//
//    NSUInteger dataLength = [self length];
//
//    //See the doc: For block ciphers, the output size will always be less than or
//    //equal to the input size plus the size of one block.
//    //That's why we need to add the size of one block here
//    size_t bufferSize = dataLength + kCCBlockSizeAES128;
//    void *buffer = malloc(bufferSize);
//
//    size_t numBytesDecrypted = 0;
//    CCCryptorStatus cryptStatus = CCCrypt(kCCDecrypt, kCCAlgorithmAES128, kCCOptionPKCS7Padding,
//                                          keyPtr, kCCKeySizeAES256,
//                                          NULL /* initialization vector (optional) */,
//                                          [self bytes], dataLength, /* input */
//                                          buffer, bufferSize, /* output */
//                                          &numBytesDecrypted);
//
//    if (cryptStatus == kCCSuccess) {
//        //the returned NSData takes ownership of the buffer and will free it on deallocation
//        return [NSData dataWithBytesNoCopy:buffer length:numBytesDecrypted];
//    }
//
//    free(buffer); //free the buffer;
//    return nil;
//}




@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

static uint64_t * from;
static uint64_t * to;

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    sem_t * sem = NULL;
    char semName[64];
    uintptr_t threadPtr = (uintptr_t)pthread_self();
    uint32_t threadTag = (uint32_t)threadPtr;
    uint32_t pidTag = (uint32_t)getpid();
    
    snprintf(semName, 64, "/%x.%x.%x", pidTag, threadTag, arc4random());
//    sem = sem_open(semName, O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem = sem_open(semName, O_CREAT, S_IRUSR | S_IWUSR, 0);

    if (SEM_FAILED == sem) {
        fprintf(stderr, "CEPoolMakeThreadRunDefault sem_open error %s; \n", strerror(errno));
        fflush(stderr);
    }
    
    int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        fprintf(stderr, "CEPoolMakeThreadRunDefault sem_open error %s; \n", strerror(errno));
        fflush(stderr);
    }
    //64 * 8

    from = malloc(64 * 1024 * 1024 * sizeof(uint64_t));
    to = malloc(64 * 1024 * 1024 * sizeof(uint64_t));

    for (uint64_t i=0; i<64 * 1024 * 1024; i++) {
        from[i] = i;
        to[i] = UINT64_MAX - i;
    }
    
    struct timespec begin = CSBootInterval();
    memcpy(to, from, 64 * 1024 * 1024 * sizeof(uint64_t));
    struct timespec end = CSBootInterval();

    int64_t begin1 = begin.tv_sec * NSEC_PER_SEC + begin.tv_nsec;
    
    int64_t end1 = end.tv_sec * NSEC_PER_SEC + end.tv_nsec;
    
    int64_t us = end1 - begin1;
    int64_t s = us / NSEC_PER_SEC;
    int64_t n = us % NSEC_PER_SEC;
    int64_t ms = n / NSEC_PER_MSEC;
    int64_t se = n % NSEC_PER_MSEC / 1000;
    int64_t usec = n % NSEC_PER_MSEC % 1000;

    NSLog(@"tc1: %lld.%03lld %03lld %03lld", s, ms, se, usec);
    
    
    for (uint64_t i=0; i<64 * 1024 * 1024; i++) {
        uint64_t f = from[i];
        uint64_t t = to[i];
        if (f != t) {
            NSLog(@"un");
        }
        from[i] = i * 2;
        to[i] = UINT64_MAX - i * 2;

    }
    
    //168109496
    //94776053
    //96199326
    //103392478
    //0.105 807 310
    
    /*
     0.105 807 310
     0.096 764 566
     0.093 658 522
     0.094 185 919
     */
    
    [self tc2];
    
    
    uint8_t keyPtr[16] = {};
    uint8_t b[32] = {};


    uint8_t result[256] = {};
    uint8_t result1[256] = {};

    size_t numBytesEncrypted = 0;
    CCCryptorStatus cryptStatus = CCCrypt(kCCEncrypt, kCCAlgorithmAES128, kCCModeCBC | kCCOptionPKCS7Padding,
                                          keyPtr, kCCKeySizeAES128,
                                          NULL /* initialization vector (optional) */,
                                          b, 16, /* input */
                                          result, 256, /* output */
                                          &numBytesEncrypted);
    if (cryptStatus == kCCSuccess) {
        //the returned NSData takes ownership of the buffer and will free it on deallocation
    }
    NSLog(@"%@", @(cryptStatus));
    
    CCCryptorStatus cryptStatus1 = CCCrypt(kCCDecrypt, kCCAlgorithmAES128, kCCModeCBC,
                                          keyPtr, kCCKeySizeAES128,
                                          NULL /* initialization vector (optional) */,
                                          result, numBytesEncrypted, /* input */
                                          result1, 256, /* output */
                                          &numBytesEncrypted);
    if (cryptStatus1 == kCCSuccess) {
        //the returned NSData takes ownership of the buffer and will free it on deallocation
    }
    NSLog(@"%@", @(cryptStatus1));
    
}

- (void)tc2 {
    struct timespec begin = CSBootInterval();
    memcpy(to, from, 64 * 1024 * 1024 * sizeof(uint64_t));
    struct timespec end = CSBootInterval();
    
    int64_t begin1 = begin.tv_sec * NSEC_PER_SEC + begin.tv_nsec;
    
    int64_t end1 = end.tv_sec * NSEC_PER_SEC + end.tv_nsec;
    
    int64_t us = end1 - begin1;
    int64_t s = us / NSEC_PER_SEC;
    int64_t n = us % NSEC_PER_SEC;
    int64_t ms = n / NSEC_PER_MSEC;
    int64_t se = n % NSEC_PER_MSEC / 1000;
    int64_t usec = n % NSEC_PER_MSEC % 1000;
    
    NSLog(@"tc2: %lld.%03lld %03lld %03lld", s, ms, se, usec);
    
    
    for (uint64_t i=0; i<64 * 1024 * 1024; i++) {
        uint64_t f = from[i];
        uint64_t t = to[i];
        if (f != t) {
            NSLog(@"un");
        }
        from[i] = i * 2;
        to[i] = UINT64_MAX - i * 2;
    }
    
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{

}



@end
