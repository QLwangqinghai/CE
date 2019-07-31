//
//  CBridge.h
//  CoreDigest
//
//  Created by vector on 2019/7/31.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CBridage : NSObject

+ (NSData *)md5:(NSData *)data;
+ (NSData *)sha160:(NSData *)data;

+ (NSData *)sha224:(NSData *)data;
+ (NSData *)sha256:(NSData *)data;
+ (NSData *)sha384:(NSData *)data;
+ (NSData *)sha512:(NSData *)data;


+ (NSData *)sha3th224:(NSData *)data;
+ (NSData *)sha3th256:(NSData *)data;
+ (NSData *)sha3th384:(NSData *)data;
+ (NSData *)sha3th512:(NSData *)data;


+ (NSData *)sha3thKeccak224:(NSData *)data;
+ (NSData *)sha3thKeccak256:(NSData *)data;
+ (NSData *)sha3thKeccak384:(NSData *)data;
+ (NSData *)sha3thKeccak512:(NSData *)data;


@end

NS_ASSUME_NONNULL_END
