//
//  BCNetworkManager+Private.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/27.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCNetworkManager.h"

#import "BCNetworkSerializer.h"


@class BCBaseRequest, BCError;
@interface BCNetworkManager (Private)

- (void)updateBaseHeaderStoreage:(void(^)(NSMutableDictionary *info))handler;

- (NSMutableDictionary *)makeBCHeader;

- (void)reportWithContext:(BCNetworkSerializerContext *)context error:(BCError *)error;

- (void)putTailInfo:(NSMutableDictionary *)dict;

@end
