//
//  BCBusinessDatabase.m
//  BitherText
//
//  Created by 陈少楠 on 2018/9/11.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCBusinessDatabase.h"
#import <FMDB/FMDatabase.h>
#import "BCBusinessStoreItem.h"
#import "BCBaseDataConfig.h"



@implementation BCBusinessDatabase
- (instancetype)initWithDatabaseConfig:(BCBaseDataConfig *)databaseConfig {
    self = [super initWithDatabaseConfig:databaseConfig];
    if (self) {
        [self bc_createTables];
    }
    return self;
}

- (void)bc_createTables {
    [self handleDatabase:^id(FMDatabase *database) {
        NSString *sq = @"CREATE TABLE IF NOT EXISTS bc_business_data (primary_key INTEGER PRIMARY KEY AUTOINCREMENT, service TEXT NOT NULL, name TEXT NOT NULL, data BLOB, feature TEXT NOT NULL, flag TEXT NOT NULL, UNIQUE (service, name, feature));";
        
        BOOL result = [database executeUpdate:sq];
        if (result) {
            NSLog(@"创建表成功");
        } else {
            NSLog(@"创建表失败, error: %@", [database lastErrorMessage]);
        }
        
        return [NSNumber numberWithBool:result];
    }];
}


-(BOOL)setItem:(BCBusinessStoreItem *)item {
    if (nil == item) {
        return false;
    }
    return [[self handleDatabase:^id(FMDatabase *database) {
        BOOL result;
        result = [database executeUpdate:@"REPLACE INTO bc_business_data (service, name, feature, data, flag) VALUES (?,?,?,?,?)"
                         withArgumentsInArray:@[
                                                nil != item.service ? item.service : @"",
                                                nil != item.name ? item.name : @"",
                                                nil != item.feature ? item.feature : @"",
                                                nil != item.data ? item.data : [NSData new],
                                                nil != item.flag ? item.flag : @"",
                                                ]];

        if (result) {
            NSLog(@"REPLACE INTO成功");
        } else {
            NSLog(@"REPLACE INTO失败, error: %@", database.lastErrorMessage);
        }
        
        return  [NSNumber numberWithBool:result];
    }] boolValue];
}


- (BOOL)setItems:(NSArray <BCBusinessStoreItem *>*)items {
    if (items.count <= 0) {
        return true;
    }
    return [[self handleDatabase:^id(FMDatabase *database) {
        BOOL result = false;
        
        BOOL beginTransaction = [database beginTransaction];
        if (!beginTransaction) {
            result = false;
            return [NSNumber numberWithBool:result];
        }
        for (BCBusinessStoreItem *item in items) {
            if (item.flag) {
                result = [database executeUpdate:@"REPLACE INTO bc_business_data (service, name, feature, data, flag) VALUES (?,?,?,?,?)"
                            withArgumentsInArray:@[
                                                   nil != item.service ? item.service : @"",
                                                   nil != item.name ? item.name : @"",
                                                   nil != item.feature ? item.feature : @"",
                                                   nil != item.data ? item.data : [NSData new],
                                                   item.flag,
                                                   ]];
            } else {
                result = [database executeUpdate:@"REPLACE INTO bc_business_data (service, name, feature, data) VALUES (?,?,?,?)"
                            withArgumentsInArray:@[
                                                   nil != item.service ? item.service : @"",
                                                   nil != item.name ? item.name : @"",
                                                   nil != item.feature ? item.feature : @"",
                                                   nil != item.data ? item.data : [NSData new],
                                                   ]];
            }
            if (result) {
                NSLog(@"REPLACE INTO成功");
            } else {
                NSLog(@"REPLACE INTO失败, error: %@", database.lastErrorMessage);
            }
        }
        if (!result) {
            [database rollback];
        } else {
            BOOL commit = [database commit];
            result = commit;
        }
        return [NSNumber numberWithBool:result];
    }] boolValue];
}

- (BOOL)removeItem:(BCBusinessStoreItem *)item {
    if (nil == item) {
        return true;
    }
    return [self handleDatabase:^id(FMDatabase *database) {
        
        BOOL result = [database executeUpdate:@"DELETE FROM bc_business_data where service = ? and name = ? and feature = ?;"
                         withArgumentsInArray:@[
                                                nil != item.service ? item.service : @"",
                                                nil != item.name ? item.name : @"",
                                                nil != item.feature ? item.feature : @"",
                                                ]];
        if (result) {
            NSLog(@"UPDATE成功");
        } else {
            NSLog(@"UPDATE失败");
        }
        return [NSNumber numberWithBool:result];
    }];
}

- (BOOL)removeItems:(NSArray <BCBusinessStoreItem *>*)items {
    if (items.count <= 0) {
        return true;
    }
    return [[self handleDatabase:^id(FMDatabase *database) {
        BOOL result = false;
        
        BOOL beginTransaction = [database beginTransaction];
        if (!beginTransaction) {
            result = false;
            return [NSNumber numberWithBool:result];
        }
        for (BCBusinessStoreItem *item in items) {
            BOOL result = [database executeUpdate:@"DELETE FROM bc_business_data where service = ? and name = ? and feature = ?;"
                             withArgumentsInArray:@[
                                                    nil != item.service ? item.service : @"",
                                                    nil != item.name ? item.name : @"",
                                                    nil != item.feature ? item.feature : @"",
                                                    ]];
            if (result) {
                NSLog(@"REPLACE INTO成功");
            } else {
                NSLog(@"REPLACE INTO失败, error: %@", database.lastErrorMessage);
            }
        }
        if (!result) {
            [database rollback];
        } else {
            BOOL commit = [database commit];
            result = commit;
        }
        return [NSNumber numberWithBool:result];
    }] boolValue];
}


- (NSArray <BCBusinessStoreItem *> *)getItemsWithService:(NSString *)service {
    if (nil == service) {
        return nil;
    }
    NSArray *items = [self handleDatabase:^id(FMDatabase *database) {
        FMResultSet * resultSet = [database executeQuery:@"select * from bc_business_data where service = ?;"
                                    withArgumentsInArray:@[
                                                           nil != service ? service : @"",
                                                           ]];
        NSMutableArray *array = [NSMutableArray array];
        while ([resultSet next]) {
            BCBusinessStoreItem *item = [BCBusinessStoreItem itemWithDatabaseResultSet:resultSet];
            if (item) {
                [array addObject:item];
            }
        }
        return array;
    }];
    
    return items;
}

- (NSArray <BCBusinessStoreItem *> *)getItemsWithService:(NSString *)service name:(NSString *)name {
    if (nil == service) {
        return nil;
    }
    if (nil == name) {
        return nil;
    }
    NSArray *items = [self handleDatabase:^id(FMDatabase *database) {
        FMResultSet * resultSet = [database executeQuery:@"select * from bc_business_data where service = ? and name = ?;"
                                    withArgumentsInArray:@[
                                                           nil != service ? service : @"",
                                                           nil != name ? name : @"",
                                                           ]];
        NSMutableArray *array = [NSMutableArray array];
        while ([resultSet next]) {
            BCBusinessStoreItem *item = [BCBusinessStoreItem itemWithDatabaseResultSet:resultSet];
            if (item) {
                [array addObject:item];
            }
        }
        return array;
    }];
    
    return items;
}

- (NSArray <BCBusinessStoreItem *> *)getItemsWithService:(NSString *)service feature:(NSString *)feature {
    if (nil == service) {
        return nil;
    }
    NSArray *items = [self handleDatabase:^id(FMDatabase *database) {
        FMResultSet * resultSet = [database executeQuery:@"select * from bc_business_data where service = ? and feature = ?;"
                                    withArgumentsInArray:@[
                                                           nil != service ? service : @"",
                                                           nil != feature ? feature : @"",
                                                           ]];
        NSMutableArray *array = [NSMutableArray array];
        while ([resultSet next]) {
            BCBusinessStoreItem *item = [BCBusinessStoreItem itemWithDatabaseResultSet:resultSet];
            if (item) {
                [array addObject:item];
            }
        }
        return array;
    }];
    
    return items;
}

- (BCBusinessStoreItem *)getItemWithService:(NSString *)service name:(NSString *)name feature:(NSString *)feature {
    if (nil == service) {
        return nil;
    }
    if (nil == name) {
        return nil;
    }
    BCBusinessStoreItem *item = [self handleDatabase:^id(FMDatabase *database) {
        FMResultSet * resultSet = [database executeQuery:@"select * from bc_business_data where service = ? and name = ? and feature = ?;"
                                    withArgumentsInArray:@[
                                                           nil != service ? service : @"",
                                                           nil != name ? name : @"",
                                                           nil != feature ? feature : @"",
                                                           ]];
        while ([resultSet next]) {
            return [BCBusinessStoreItem itemWithDatabaseResultSet:resultSet];
        }
        return nil;
    }];
    
    return item;
}

- (BOOL)removeItemsWithService:(NSString *)service {
    return [[self handleDatabase:^id(FMDatabase *database) {
        BOOL result = [database executeUpdate:@"DELETE FROM bc_business_data where service = ?;"
                         withArgumentsInArray:@[
                                                nil != service ? service : @"",
                                                ]];
        if (result) {
            NSLog(@"UPDATE成功");
        } else {
            NSLog(@"UPDATE失败");
        }
        return [NSNumber numberWithBool:result];
    }] boolValue];
}

- (BOOL)removeItemsWithService:(NSString *)service name:(NSString *)name {
    return [[self handleDatabase:^id(FMDatabase *database) {
        BOOL result = [database executeUpdate:@"DELETE FROM bc_business_data where service = ? and name = ?;"
                         withArgumentsInArray:@[
                                                nil != service ? service : @"",
                                                nil != name ? name : @"",
                                                ]];
        if (result) {
            NSLog(@"UPDATE成功");
        } else {
            NSLog(@"UPDATE失败");
        }
        return [NSNumber numberWithBool:result];
    }] boolValue];
}

- (BOOL)removeItemsWithService:(NSString *)service feature:(NSString *)feature {
    return [[self handleDatabase:^id(FMDatabase *database) {
        BOOL result = [database executeUpdate:@"DELETE FROM bc_business_data where service = ? and feature = ?;"
                         withArgumentsInArray:@[
                                                nil != service ? service : @"",
                                                nil != feature ? feature : @"",
                                                ]];
        if (result) {
            NSLog(@"UPDATE成功");
        } else {
            NSLog(@"UPDATE失败");
        }
        return [NSNumber numberWithBool:result];
    }] boolValue];
}

- (BOOL)removeItemWithService:(NSString *)service name:(NSString *)name feature:(NSString *)feature {
    return [[self handleDatabase:^id(FMDatabase *database) {
        BOOL result = [database executeUpdate:@"DELETE FROM bc_business_data where service = ? and name = ? and feature = ?;"
                         withArgumentsInArray:@[
                                                nil != service ? service : @"",
                                                nil != name ? name : @"",
                                                nil != feature ? feature : @"",
                                                ]];
        if (result) {
            NSLog(@"UPDATE成功");
        } else {
            NSLog(@"UPDATE失败");
        }
        return [NSNumber numberWithBool:result];
    }] boolValue];
}

+ (instancetype)sharedInstance {
    static BCBusinessDatabase *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[BCBusinessDatabase alloc] initWithDatabaseConfig:[BCBusinessDatabase createDefaultConfig]];
    });
    return instance;
}



static char *c04684af78ecc = "8767bd81b92aa2261a6fd170c1d1680a57ccfae8055b205aaf9e6bfe6e9fec6285f6248ca35e25f72ffe6b99b272d3caf531a128ca917b71476c04684af78ecc";
+ (BCBaseDataConfig *)createDefaultConfig {
    
    NSString *documentDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];
    assert(documentDirectory);
    NSString *dbPath = [documentDirectory stringByAppendingPathComponent:@"/business.sqlite"];
    
    BCBaseDataConfig *config = [[BCBaseDataConfig alloc] initWithKeySeed:@"d490fbbb146d75ef5589b14cb7285449f2f453c68776c58f4c464dca1d80a5d7821f91e7c9e86df79ab9f70f0d47b28f721ce14c604585bc3f4dea96473bc717" key:(uint8_t *)c04684af78ecc keyLength:(uint32_t)strlen(c04684af78ecc) path:dbPath queue:dispatch_queue_create("com.bichan.BCBusinessDatabaseQueue", NULL)];
    
    return config;
}
@end


