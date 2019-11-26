//
//  BCBusinessDatabase.h
//  BitherText
//
//  Created by 陈少楠 on 2018/9/11.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCBaseDatabase.h"

@class BCBusinessStoreItem;

@interface BCBusinessDatabase : BCBaseDatabase

- (BOOL)setItem:(BCBusinessStoreItem *)item;
- (BOOL)setItems:(NSArray <BCBusinessStoreItem *>*)items;

- (BOOL)removeItem:(BCBusinessStoreItem *)item;
- (BOOL)removeItems:(NSArray <BCBusinessStoreItem *>*)items;


- (NSArray <BCBusinessStoreItem *> *)getItemsWithService:(NSString *)service;
- (NSArray <BCBusinessStoreItem *> *)getItemsWithService:(NSString *)service name:(NSString *)name;
- (NSArray <BCBusinessStoreItem *> *)getItemsWithService:(NSString *)service feature:(NSString *)feature;
- (BCBusinessStoreItem *)getItemWithService:(NSString *)service name:(NSString *)name feature:(NSString *)feature;

- (BOOL)removeItemsWithService:(NSString *)service;
- (BOOL)removeItemsWithService:(NSString *)service name:(NSString *)name;
- (BOOL)removeItemsWithService:(NSString *)service feature:(NSString *)feature;
- (BOOL)removeItemWithService:(NSString *)service name:(NSString *)name feature:(NSString *)feature;


+ (instancetype)sharedInstance;
@end
