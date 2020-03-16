//
//  BCBusinessDatabaseItem.h
//  BitherText
//
//  Created by 陈少楠 on 2018/9/11.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <Foundation/Foundation.h>

@class FMResultSet;
@interface BCBusinessStoreItem : NSObject

//service + name + feature 唯一

//服务
@property (nonatomic, copy, readonly) NSString *service;

//名
@property (nonatomic, copy, readonly) NSString *name;

//特征, 用于筛选
@property (nonatomic, copy, readonly) NSString *feature;

//数据
@property (nonatomic, copy) NSData *data;

@property (nonatomic, copy) NSString *flag;


- (instancetype)initWithService:(NSString *)service name:(NSString *)name feature:(NSString *)feature;

+ (BCBusinessStoreItem *)itemWithDatabaseResultSet:(FMResultSet *)set;

@end
