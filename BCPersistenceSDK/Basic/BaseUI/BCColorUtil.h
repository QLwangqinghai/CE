//
//  BCColorUtil.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/6.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BCColorUtil : NSObject

+ (UIColor *)colorWithHexString:(NSString *)hexStr;
+ (UIColor *)colorWithHexString:(NSString *)hexStr alpha:(CGFloat)alpha;

@end
