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

+ (NSData *)sha224:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:28];
    
    CDSHA2th224Context_s context = {};
    
    CDSHA2th224ContextInit(&context);
    CDSHA2th224Update(&context, data.bytes, data.length);
    CDSHA2th224Final(&context);
    CDSHA2th224ExportHashValue(&context, result.mutableBytes);
    return result;
}

+ (NSData *)sha256:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:32];
    CDSHA2th256Context_s context = {};
    CDSHA2th256ContextInit(&context);
    CDSHA2th256Update(&context, data.bytes, data.length);
    CDSHA2th256Final(&context);
    CDSHA2th256ExportHashValue(&context, result.mutableBytes);
    return result;
}

+ (NSData *)sha512:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:64];
    CDSHA2th512Context_s context = {};
    CDSHA2th512ContextInit(&context);
    CDSHA2th512Update(&context, data.bytes, data.length);
    CDSHA2th512Final(&context);
    CDSHA2th512ExportHashValue(&context, result.mutableBytes);
    return result;
}

+ (NSData *)sha384:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:48];
    CDSHA2th384Context_s context;
    CDSHA2th384ContextInit(&context);
    CDSHA2th384Update(&context, data.bytes, data.length);
    CDSHA2th384Final(&context);
    CDSHA2th384ExportHashValue(&context, result.mutableBytes);
    return result;
}


@end
