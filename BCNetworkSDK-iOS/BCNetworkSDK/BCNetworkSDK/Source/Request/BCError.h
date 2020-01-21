//
//  BCError.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/30.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BCNetworkError.h"

@interface BCError : NSError

@property (nonatomic, copy) NSString *message;
@property (nonatomic, copy, readonly) NSString *tostMessage;

+ (instancetype)networkErrorWithCode:(NSInteger)code message:(NSString *)message userInfo:(NSDictionary *)dict;

@end
