////  CryptoSwift
//
//  Copyright (C) 2014-__YEAR__ Marcin Krzyżanowski <marcin@krzyzanowskim.com>
//  This software is provided 'as-is', without any express or implied warranty.
//
//  In no event will the authors be held liable for any damages arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
//
//  - The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation is required.
//  - Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
//  - This notice may not be removed or altered from any source or binary distribution.
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


@end

NS_ASSUME_NONNULL_END
