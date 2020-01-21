//
//  BCNetworkCrypto.m
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCNetworkCrypto.h"
#import <zlib.h>
#import <CommonCrypto/CommonCrypto.h>

@implementation BCNetworkCrypto

+ (uint32_t)crc32:(NSData *)data {
    uLong crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, [data bytes], (uInt)[data length]);
    return (uint32_t)crc;
}

+ (NSData *)aes128ECBEncrypt:(NSData *)data withKey:(BCAes128Key_t *)key {
    if (nil == data || nil == key) {
        return nil;
    }
    
    uint8_t *keyPtr = key->content;

    NSUInteger dataLength = [data length];
    
    size_t bufferSize = dataLength + kCCBlockSizeAES128;
    void *buffer = malloc(bufferSize);
    
    size_t outBytesLength = 0;
    CCCryptorStatus cryptStatus = CCCrypt(
                                          kCCEncrypt,
                                          kCCAlgorithmAES,
                                          kCCOptionPKCS7Padding | kCCOptionECBMode,
                                          keyPtr,
                                          kCCKeySizeAES128,
                                          NULL, /* initialization vector (optional) */
                                          data.bytes,
                                          dataLength,
                                          buffer,
                                          bufferSize,
                                          &outBytesLength
                                          );
    
    
    if (cryptStatus == kCCSuccess) {
        return [NSData dataWithBytesNoCopy:buffer length:outBytesLength freeWhenDone:true];
    }
    
    free(buffer);
    return nil;
}

+ (NSData *)aes128ECBDecrypt:(NSData *)data withKey:(BCAes128Key_t *)key {
    if (nil == data || nil == key) {
        return nil;
    }
    
    uint8_t *keyPtr = key->content;
    
    NSUInteger dataLength = [data length];
    
    //See the doc: For block ciphers, the output size will always be less than or
    //equal to the input size plus the size of one block.
    //That's why we need to add the size of one block here
    size_t bufferSize = dataLength + kCCBlockSizeAES128;
    void *buffer = malloc(bufferSize);
    assert(buffer);
    
    size_t numBytesDecrypted = 0;
    CCCryptorStatus cryptStatus = CCCrypt(kCCDecrypt,
                                          kCCAlgorithmAES,
                                          kCCOptionPKCS7Padding | kCCOptionECBMode,
                                          keyPtr,
                                          kCCKeySizeAES128,
                                          NULL /* initialization vector (optional) */,
                                          [data bytes],
                                          dataLength, /* input */
                                          buffer,
                                          bufferSize, /* output */
                                          &numBytesDecrypted);
    
    if (cryptStatus == kCCSuccess) {
        //the returned NSData takes ownership of the buffer and will free it on deallocation
        return [NSData dataWithBytesNoCopy:buffer length:numBytesDecrypted];
    }
    
    free(buffer); //free the buffer;
    return nil;
}


+ (NSString *)base64EncodeToString:(NSData *)data {
    return [data base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength];
}

+ (NSData *)base64EncodeToData:(NSData *)data {
    return [data base64EncodedDataWithOptions:NSDataBase64Encoding64CharacterLineLength];
}

+ (NSData *)base64DecodeFromString:(NSString*)str {
    return [[NSData alloc] initWithBase64EncodedString:str options:NSDataBase64DecodingIgnoreUnknownCharacters];
}
+ (NSData *)base64DecodeFromData:(NSData *)data {
    return [[NSData alloc] initWithBase64EncodedData:data options:NSDataBase64DecodingIgnoreUnknownCharacters];
}

@end


int BCNetworkCryptoGenerateAesKey(NSData *seed, BCAes128Key_t *key) {
    if (nil == seed || nil == key) {
        return -1;
    }
//    static char *k11 = "asjfkaq2lj4kladssjfkalfajkerj;alfd[werpo[paf[pl[l;adf]]]]eriasdifoo";
//    HMAC-MD5(data, key) => 32字节byte
    
    
    
    static char *k = "lj4kladssjfkalfajkerj;alfd[werpo[paf[pl[l;adf]]]]eriasdifoo";
    uint32_t len = (uint32_t)strlen(k);
    uint8_t keyBuffer[256] = {'a', 's', 'j', 'f', 'k', 'a', 'q', '2'};
    memcpy(keyBuffer + 8, k, len);
    CCHmac(kCCHmacAlgMD5, keyBuffer, len + 8, seed.bytes, seed.length, key->content);
    memset(keyBuffer, 0, 256);
    return 0;
}

