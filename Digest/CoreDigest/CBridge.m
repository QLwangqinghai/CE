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

+ (void)md5:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDMD5Context_s context = {};
    CDMD5ContextInit(&context);
    CDMD5Update(&context, data, length);
    CDMD5Final(&context);
    CDMD5ExportHashValue(&context, result);
}
+ (void)sha160:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA1Context_s context = {};
    CDSHA1ContextInit(&context);
    CDSHA1Update(&context, data, length);
    CDSHA1Final(&context);
    CDSHA1ExportHashValue(&context, result);
}

+ (void)sha224:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA2Context_s context = {};
    CDSHA2ContextInit(&context, CCDigestTypeSha2Variant224);
    CDSHA2Update(&context, data, length);
    CDSHA2Final(&context);
    CDSHA2ExportHashValue(&context, result);
}

+ (void)sha256:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA2Context_s context = {};
    CDSHA2ContextInit(&context, CCDigestTypeSha2Variant256);
    CDSHA2Update(&context, data, length);
    CDSHA2Final(&context);
    CDSHA2ExportHashValue(&context, result);
}

+ (void)sha512:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA2Context_s context = {};
    CDSHA2ContextInit(&context, CCDigestTypeSha2Variant512);
    CDSHA2Update(&context, data, length);
    CDSHA2Final(&context);
    CDSHA2ExportHashValue(&context, result);
}

+ (void)sha384:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA2Context_s context;
    CDSHA2ContextInit(&context, CCDigestTypeSha2Variant384);
    CDSHA2Update(&context, data, length);
    CDSHA2Final(&context);
    CDSHA2ExportHashValue(&context, result);
}


+ (void)sha3th256:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CCDigestTypeSha3Variant256);
    CDSHA3Update(&context, data, length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result);
}
+ (void)sha3th224:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CCDigestTypeSha3Variant224);
    CDSHA3Update(&context, data, length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result);
}

+ (void)sha3th384:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CCDigestTypeSha3Variant384);
    CDSHA3Update(&context, data, length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result);
}
+ (void)sha3th512:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CCDigestTypeSha3Variant512);
    CDSHA3Update(&context, data, length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result);
}


+ (void)sha3thKeccak256:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CCDigestTypeSha3VariantKeccak256);
    CDSHA3Update(&context, data, length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result);
}
+ (void)sha3thKeccak224:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CCDigestTypeSha3VariantKeccak224);
    CDSHA3Update(&context, data, length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result);
}

+ (void)sha3thKeccak384:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CCDigestTypeSha3VariantKeccak384);
    CDSHA3Update(&context, data, length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result);
}
+ (void)sha3thKeccak512:(void const * _Nonnull)data length:(size_t)length hash:(uint8_t * _Nonnull)result {
    CDSHA3Context_s context;
    CDSHA3ContextInit(&context, CCDigestTypeSha3VariantKeccak512);
    CDSHA3Update(&context, data, length);
    CDSHA3Final(&context);
    CDSHA3ExportHashValue(&context, result);
}



@end
