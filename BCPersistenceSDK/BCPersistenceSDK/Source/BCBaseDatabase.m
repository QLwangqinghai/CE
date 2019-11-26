//
//  BCBaseDatabase.m
//  BitherText
//
//  Created by 陈少楠 on 2018/9/11.
//  Copyright © 2018年 陈少楠. All rights reserved.
//


#import "BCBaseDatabase.h"
#import <FMDatabase.h>
#import "BCBaseDataConfig.h"

@interface BCBaseDatabase ()
{
    FMDatabase *_database;
}
@property (nonatomic, readonly) FMDatabase *database;

//此处用strong
@property (nonatomic, strong) id <BCDatabaseConfigProtocol> databaseConfig;
@end

@implementation BCBaseDatabase

- (instancetype)init {
    return nil;
}

- (instancetype)initWithDatabaseConfig:(id <BCDatabaseConfigProtocol>)databaseConfig {

    if (databaseConfig == nil) {
        return nil;
    }
    
    FMDatabase * database = [FMDatabase databaseWithPath:databaseConfig.sqlitePath];
    
    if (nil == database) {
        return nil;
    }
    
    self = [super init];
    
    if (self) {
        _database = database;
        self.databaseConfig = databaseConfig;
    }
    
    return self;
}

- (BOOL)openDatabase {
    if (![self.database goodConnection]) {
        if ([self.database open]) {
            NSData *data = self.databaseConfig.sqliteKeyData;
            if (data.length > 0) {
                BOOL result = [self.database setKeyWithData:data];
                NSLog(@"setKeyWithData result:%@", @(result));
            }
            return true;
        } else {
            return false;
        }
    }
    return true;
}


- (id)handleDatabase:(id(^)(FMDatabase *database))handle {

    __block id result = nil;
    
    [self.databaseConfig sync:^{
        if ([self openDatabase]) {
            if (handle) {
                result = handle(self.database);
            }
        } else {
            NSLog(@"打开数据库失败");
        }
    }];

    return result;
}

- (void)dealloc {
    [self.database close];
}

@end




