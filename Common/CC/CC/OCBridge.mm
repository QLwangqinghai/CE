//
//  OCBridge.m
//  CC
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#import "OCBridge.h"
#import <CCFoundation/CCFoundation.h>

#import <CommonCrypto/CommonCrypto.h>

#import <CommonCrypto/CommonDigest.h>


@implementation OCBridge

//+ (void)test {
//}
//
//+ (void)testAes {
//    size_t size = 128*1024*1024;
//    void * buffer = malloc(size);
//    assert(buffer);
//
//    uint8_t * key = malloc(64);
//    for (int i=0; i<64; i+=4) {
//        *(uint32_t *)(key + i) = arc4random();
//    }
//
//    void * bufferA = malloc(size + 32);
//    void * bufferB = malloc(size + 32);
//    assert(bufferA);
//    assert(bufferB);
//
//
//    dispatch_async(dispatch_get_global_queue(0, 0), ^{
//        CCAesContext_s contextA = {};
//        CCAesContextInit(&contextA, key, 16 * 8, NULL, CCAesOperationEncrypt, CCAesCryptModeCbc, CCAesPaddingModeNoPadding);
//
//        int cmp0 = memcmp(bufferA, bufferB, size);
//
//        size_t outputSize = 0;
//        int a = CCCrypt(kCCEncrypt, kCCAlgorithmAES128, kCCOptionPKCS7Padding, key, 16, NULL, buffer, size, bufferA, size + 32, &outputSize);
//
//        CFTimeInterval t0 = CFAbsoluteTimeGetCurrent();
//        int b = CCCrypt(kCCEncrypt, kCCAlgorithmAES128, 0, key, 16, NULL, buffer, size, bufferA, size+32, &outputSize);
//        CFTimeInterval t1 = CFAbsoluteTimeGetCurrent();
//
//        CCResult_t mac = CCAesCrypt(&contextA, buffer, size, bufferB, size + 32, &outputSize);
//
//        CFTimeInterval t2 = CFAbsoluteTimeGetCurrent();
//
//
//        int cmp1 = memcmp(bufferA, bufferB, size);
//
//        NSLog(@"%.04lf %.04lf", t1-t0, t2-t1);
//
//
//
//        NSLog(@"%ld %ld", cmp0, cmp1);
//
//    });
//
//}



@end
