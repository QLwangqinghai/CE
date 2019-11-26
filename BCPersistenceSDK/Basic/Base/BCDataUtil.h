//
//  BCDataUtil.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/22.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BCDataUtil : NSObject


+ (NSString *)hexWithData:(NSData *)data;

+ (NSData *)dataWithHexString:(NSString *)string;


@end
