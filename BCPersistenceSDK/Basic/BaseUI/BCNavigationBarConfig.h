//
//  BCNavigationBarConfig.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BCNavigationBarConfig : NSObject

#pragma mark - navigationBar
//是否显示导航 默认为false
@property (nonatomic, assign) BOOL barHidden;


#pragma mark - statusBar
//是否显示状态栏
@property (nonatomic, assign) BOOL statusBarHidden;

//默认值为UIStatusBarStyleDefault
@property (nonatomic, assign) UIStatusBarStyle statusBarStyle;

//默认值UIStatusBarAnimationFade
@property (nonatomic, assign) UIStatusBarAnimation statusBarAnimation;

//默认值TRUE
@property (nonatomic, assign) BOOL hidesBottomBarWhenPushed;

@property (nonatomic, assign) UIColor *backgroundColor;

+ (instancetype)defaultConfig;

@end
