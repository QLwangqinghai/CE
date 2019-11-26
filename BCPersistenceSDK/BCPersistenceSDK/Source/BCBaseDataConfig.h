//
//  BCBaseDataConfig.h
//  BitherText
//
//  Created by 陈少楠 on 2018/9/11.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol BCDatabaseConfigProtocol <NSObject>

// key 用来加密数据库
@property (nonatomic, copy, readonly) NSData *sqliteKeyData;

// 路径 创建数据库
@property (nonatomic, copy, readonly) NSString *sqlitePath;


- (void)sync:(dispatch_block_t)block;


@end


@interface BCBaseDataConfig : NSObject <BCDatabaseConfigProtocol>
// 线程 用来操作数据库
@property (nonatomic, strong, readonly) dispatch_queue_t databaseQueue;

// key 用来加密数据库
@property (nonatomic, copy, readonly) NSData *sqliteKeyData;

// 路径 创建数据库
@property (nonatomic, copy, readonly) NSString *sqlitePath;

- (instancetype)initWithKeySeed:(NSString *)seed key:(uint8_t *)key keyLength:(uint32_t)length path:(NSString *)path queue:(dispatch_queue_t)queue;


- (void)sync:(dispatch_block_t)block;

@end



typedef struct BCByteBuffer32 {
    uint32_t length;
    uint8_t buffer[32];
} BCByteBuffer32_t;

typedef NS_ENUM(NSUInteger, BCPersistenceGenerateType) {
    BCPersistenceGenerateTypeHmacMd5,
    BCPersistenceGenerateTypeHmacSha256,
};

BCByteBuffer32_t BCPersistenceGenerateKey(BCPersistenceGenerateType type, NSData *seed, uint8_t *key, uint32_t keyLength);
