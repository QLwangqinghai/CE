//
//  AppDelegate.m
//  AESDemo
//
//  Created by vector on 2018/12/20.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#import "AppDelegate.h"
#include "CCAes.h"
#import <CommonCrypto/CommonCryptor.h>

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    
    uint8_t old[1024] = {};

    
    for (uint64_t i=0; i<1024; i++) {
        old[i] = i;
    }
    
    
    uint8_t keyPtr[16] = {};
    for (uint8_t i=0; i<16; i++) {
        keyPtr[i] = i + 5;
    }
    
    uint8_t result[2048] = {};
    uint8_t result1[2048] = {};
    
    uint8_t dresult[2048] = {};
    uint8_t dresult1[2048] = {};


    size_t numBytesEncrypted = 0;
    CCCryptorStatus cryptStatus = CCCrypt(kCCEncrypt, kCCAlgorithmAES128, kCCOptionPKCS7Padding,
                                          keyPtr, kCCKeySizeAES128,
                                          NULL /* initialization vector (optional) */,
                                          old, 15, /* input */
                                          result, 2048, /* output */
                                          &numBytesEncrypted);
    if (cryptStatus == kCCSuccess) {
        //the returned NSData takes ownership of the buffer and will free it on deallocation
    }
    
//    CCCryptorStatus cryptStatus1 = CCCrypt(kCCDecrypt, kCCAlgorithmAES128, kCCModeCBC | kCCOptionPKCS7Padding,
//                                           keyPtr, kCCKeySizeAES128,
//                                           NULL /* initialization vector (optional) */,
//                                           result, numBytesEncrypted, /* input */
//                                           result, 256, /* output */
//                                           &numBytesEncrypted);
//    if (cryptStatus1 == kCCSuccess) {
//        //the returned NSData takes ownership of the buffer and will free it on deallocation
//    }
//    NSLog(@"%@", @(cryptStatus1));

    size_t moved = 0;
    CCAesContext_s context = {};
    int ccr1 = CCAesContextInit(&context, keyPtr, 128, NULL, CCAesOperationEncrypt, CCAesCryptModeCbc, CCAesPaddingModePkcs5);
    
    int ccr = CCAesCrypt(&context, old, 15, result1, 2048, &moved);
    


    CCAesContext_s dcontext = {};
    size_t dmoved = 0;

    int dccr1 = CCAesContextInit(&dcontext, keyPtr, 128, NULL, CCAesOperationDecrypt, CCAesCryptModeCbc, CCAesPaddingModeNoPadding);//CCAesPaddingModePkcs5
    int dccr = CCAesCrypt(&dcontext, result, moved, dresult1, 2048, &dmoved);

//    int dccr = CCAesCrypt(&dcontext, result1, moved, dresult1, 2048, &dmoved);

    
    size_t dnumBytesEncrypted = 0;
    CCCryptorStatus dcryptStatus = CCCrypt(kCCDecrypt, kCCAlgorithmAES128, 0,
                                          keyPtr, kCCKeySizeAES128,
                                          NULL /* initialization vector (optional) */,
                                          result, 32, /* input */
                                          dresult, 2048, /* output */
                                          &dnumBytesEncrypted);
    if (dcryptStatus == kCCSuccess) {
        //the returned NSData takes ownership of the buffer and will free it on deallocation
    }
    
    
    for (uint64_t i=0; i<2048; i++) {
        if (result[i] != result1[i]) {
            NSLog(@"%@", @(i));
            
        }
    }
    NSLog(@"eq");

    for (uint64_t i=0; i<2048; i++) {
        if (dresult[i] != dresult1[i]) {
            NSLog(@"d %@", @(i));
            
        }
    }
    NSLog(@"d eq");
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
