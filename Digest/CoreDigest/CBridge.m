//
//  CBridge.m
//  CoreDigest
//
//  Created by vector on 2019/7/31.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "CBridge.h"
#import <CoreDigest/CDAPI.h>


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
    CDSHA1Context_s context = {};
    CDSHA1ContextInit(&context);
    CDSHA1Update(&context, data.bytes, data.length);
    CDSHA1Final(&context);
    CDSHA1ExportHashValue(&context, result.mutableBytes);
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


+ (NSData *)sha3thKeccak256:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:32];
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CDVariantSHA3thKeccak256);
    CDSHA3Update(&context, data.bytes, data.length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result.mutableBytes);
    return result;
}
+ (NSData *)sha3thKeccak224:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:28];
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CDVariantSHA3thKeccak224);
    CDSHA3Update(&context, data.bytes, data.length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result.mutableBytes);
    return result;
}

+ (NSData *)sha3thKeccak384:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:48];
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CDVariantSHA3thKeccak384);
    CDSHA3Update(&context, data.bytes, data.length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result.mutableBytes);
    return result;
}
+ (NSData *)sha3thKeccak512:(NSData *)data {
    NSMutableData * result = [NSMutableData dataWithLength:64];
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CDVariantSHA3thKeccak512);
    CDSHA3Update(&context, data.bytes, data.length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result.mutableBytes);
    return result;
}



@end
