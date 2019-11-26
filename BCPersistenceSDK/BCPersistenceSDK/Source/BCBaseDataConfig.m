//
//  BCBaseDataConfig.m
//  BitherText
//
//  Created by 陈少楠 on 2018/9/11.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCBaseDataConfig.h"
#import <CommonCrypto/CommonCrypto.h>


@interface BCBaseDataConfig ()
// 线程 用来操作数据库
@property (nonatomic, strong) dispatch_queue_t databaseQueue;

// key 用来加密数据库
@property (nonatomic, copy) NSString *sqliteKeySeed;

// 路径 创建数据库
@property (nonatomic, copy) NSString *sqlitePath;

@property (nonatomic, assign) uint8_t *key;
@property (nonatomic, assign) uint32_t keyLength;

@end
@implementation BCBaseDataConfig
- (instancetype)init {
    return nil;
}

- (instancetype)initWithKeySeed:(NSString *)seed key:(uint8_t *)key keyLength:(uint32_t)length path:(NSString *)path queue:(dispatch_queue_t)queue {
    if (0 == path.length) {
        return nil;
    }
    if (NULL == queue) {
        return nil;
    }
    if (NULL == queue) {
        return nil;
    }
    self = [super init];
    
    if (self) {
        _sqliteKeySeed = seed;
        _sqlitePath = path;
        _databaseQueue = queue;
        uint8_t *ptr = malloc(length > 0 ? length : 8);
        assert(ptr);
        memcpy(ptr, key, length);
        self.key = ptr;
        self.keyLength = length;
    }
    return self;
}


- (void)sync:(dispatch_block_t)block {
    dispatch_sync(self.databaseQueue, block);
}

- (void)dealloc {
    memset(self.key, 0, self.keyLength);
    free(self.key);
}

- (NSData *)sqliteKeyData {
    BCByteBuffer32_t result = BCPersistenceGenerateKey(BCPersistenceGenerateTypeHmacSha256, [self.sqliteKeySeed dataUsingEncoding:NSUTF8StringEncoding], self.key, self.keyLength);
    return [NSData dataWithBytes:result.buffer length:result.length];
}

@end



BCByteBuffer32_t BCPersistenceGenerateKey(BCPersistenceGenerateType type, NSData *seed, uint8_t *key, uint32_t keyLength) {
    BCByteBuffer32_t result = {};
    if (nil == seed || nil == key) {
        return result;
    }
    if (type == BCPersistenceGenerateTypeHmacMd5) {
        CCHmac(kCCHmacAlgMD5, key, keyLength, seed.bytes, seed.length, result.buffer);
        result.length = 16;
    } else if (type == BCPersistenceGenerateTypeHmacSha256) {
        CCHmac(kCCHmacAlgSHA256, key, keyLength, seed.bytes, seed.length, result.buffer);
        result.length = 32;
    }
    return result;
}
