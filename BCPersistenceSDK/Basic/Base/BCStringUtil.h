//
//  BCStringUtil.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/28.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BCStringUtil : NSObject

//注意 这个方法 nil 当成 @"" 处理
+ (BOOL)string:(NSString *)string isEquelTo:(NSString *)other;

+ (BOOL)isAllNumber:(NSString *)string;



@end
