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

#import "CBridage.h"
#import "CoreDigest.h"


@implementation CBridage

+ (NSData *)md5:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:16];
    
    CDMD5Context_s context = {};
    
    CDMD5ContextInit(&context);
    CDMD5Update(&context, data.bytes, data.length);
    CDMD5Final(&context);
    CDMD5ExportHashValue(&context, result.mutableBytes);
    return result;
}
+ (NSData *)sha160:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:20];
    CDSHA1th160Context_s context = {};
    CDSHA1th160ContextInit(&context);
    CDSHA1th160Update(&context, data.bytes, data.length);
    CDSHA1th160Final(&context);
    CDSHA1th160ExportHashValue(&context, result.mutableBytes);
    return result;
}

+ (NSData *)sha224:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:28];
    
    CDSHA2Context_s context = {};
    
    CDSHA2ContextInit(&context, CDVariantSHA2th224);
    CDSHA2Update(&context, data.bytes, data.length);
    CDSHA2Final(&context);
    CDSHA2ExportHashValue(&context, result.mutableBytes);
    return result;
}

+ (NSData *)sha256:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:32];
    CDSHA2Context_s context = {};
    CDSHA2ContextInit(&context, CDVariantSHA2th256);
    CDSHA2Update(&context, data.bytes, data.length);
    CDSHA2Final(&context);
    CDSHA2ExportHashValue(&context, result.mutableBytes);
    return result;
}

+ (NSData *)sha512:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:64];
    CDSHA2Context_s context = {};
    CDSHA2ContextInit(&context, CDVariantSHA2th512);
    CDSHA2Update(&context, data.bytes, data.length);
    CDSHA2Final(&context);
    CDSHA2ExportHashValue(&context, result.mutableBytes);
    return result;
}

+ (NSData *)sha384:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:48];
    CDSHA2Context_s context;
    CDSHA2ContextInit(&context, CDVariantSHA2th384);
    CDSHA2Update(&context, data.bytes, data.length);
    CDSHA2Final(&context);
    CDSHA2ExportHashValue(&context, result.mutableBytes);
    return result;
}


+ (NSData *)sha3th256:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:32];
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CDVariantSHA3th256);
    CDSHA3Update(&context, data.bytes, data.length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result.mutableBytes);
    return result;
}
+ (NSData *)sha3th224:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:28];
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CDVariantSHA3th224);
    CDSHA3Update(&context, data.bytes, data.length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result.mutableBytes);
    return result;
}

+ (NSData *)sha3th384:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:48];
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CDVariantSHA3th384);
    CDSHA3Update(&context, data.bytes, data.length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result.mutableBytes);
    return result;
}
+ (NSData *)sha3th512:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:64];
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CDVariantSHA3th512);
    CDSHA3Update(&context, data.bytes, data.length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result.mutableBytes);
    return result;
}

@end
