//
//  BCBaseModel.m
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCBaseModel.h"
#import "NSObject+BCModel.h"

@implementation BCBaseModel

- (NSString *)debugDescription {
    return [NSString stringWithFormat:@"<%@: %p> %@", [self class], self, [self modelToJSONString]];
}

@end
