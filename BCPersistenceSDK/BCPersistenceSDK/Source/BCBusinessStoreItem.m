//
//  BCBusinessDatabaseItem.m
//  BitherText
//
//  Created by 陈少楠 on 2018/9/11.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCBusinessStoreItem.h"
#import <FMDB/FMDatabase.h>


@interface BCBusinessStoreItem ()

@property (nonatomic, copy) NSString *service;
@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSString *feature;

@end

@implementation BCBusinessStoreItem

- (instancetype)initWithService:(NSString *)service name:(NSString *)name feature:(NSString *)feature {
    if (nil == service) {
        return nil;
    }
    if (nil == name) {
        return nil;
    }
    
    self = [super init];
    if (self) {
        self.service = service;
        self.name = name;
        self.feature = nil != feature ? feature : @"";
    }
    return self;
}

+ (BCBusinessStoreItem *)itemWithDatabaseResultSet:(FMResultSet *)set {
    if (nil == set) {
        return nil;
    }
    
    BCBusinessStoreItem *model = [[BCBusinessStoreItem alloc] init];
    model.name = [set stringForColumn:@"name"];
    model.data = [set dataForColumn:@"data"];
    model.service = [set stringForColumn:@"service"];
    model.feature = [set stringForColumn:@"feature"];
    model.flag = [set stringForColumn:@"flag"];
    
    return model;
}

@end
