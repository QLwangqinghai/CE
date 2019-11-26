//
//  BCBaseDatabase.h
//  BitherText
//
//  Created by 陈少楠 on 2018/9/11.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <Foundation/Foundation.h>

@class FMDatabase, BCBaseDataConfig;
@protocol BCDatabaseConfigProtocol;

@interface BCBaseDatabase : NSObject

- (instancetype)initWithDatabaseConfig:(id <BCDatabaseConfigProtocol>)databaseConfig;

- (id)handleDatabase:(id(^)(FMDatabase *database))handle;

@end
