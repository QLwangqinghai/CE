//
//  BCNetworkCrypto.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <Foundation/Foundation.h>


typedef struct {
    uint8_t content[16];
} BCAes128Key_t;


@interface BCNetworkCrypto : NSObject

+ (uint32_t)crc32:(NSData *)data;

+ (NSData *)aes128ECBEncrypt:(NSData *)data withKey:(BCAes128Key_t *)key;

+ (NSData *)aes128ECBDecrypt:(NSData *)data withKey:(BCAes128Key_t *)key;


+ (NSString *)base64EncodeToString:(NSData *)data;

+ (NSData *)base64EncodeToData:(NSData *)data;

+ (NSData *)base64DecodeFromString:(NSString*)str;

+ (NSData *)base64DecodeFromData:(NSData *)data;

@end

int BCNetworkCryptoGenerateAesKey(NSData *seed, BCAes128Key_t *key);



