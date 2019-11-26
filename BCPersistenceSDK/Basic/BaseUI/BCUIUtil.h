//
//  BCUIUtil.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/6.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BCUIUtil : NSObject

+ (BOOL)isPad;
+ (BOOL)isIPhoneX;
+ (BOOL)isIPhone;
+ (CGSize)screenSize;

+ (CGFloat)screenWidth;
+ (CGFloat)screenHeight;
+ (CGFloat)navigationBarHeight;
+ (CGFloat)statusBarHeight;

@end
