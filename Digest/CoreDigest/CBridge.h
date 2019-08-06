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

+ (void)md5:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha160:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;

+ (void)sha224:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha256:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha384:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha512:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;


+ (void)sha3th224:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha3th256:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha3th384:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha3th512:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;


+ (void)sha3thKeccak224:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha3thKeccak256:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha3thKeccak384:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;
+ (void)sha3thKeccak512:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)hash;


@end

NS_ASSUME_NONNULL_END
