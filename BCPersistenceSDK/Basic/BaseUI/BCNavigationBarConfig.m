//
//  BCNavigationBarConfig.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCNavigationBarConfig.h"

@implementation BCNavigationBarConfig

- (instancetype)init {
    self = [super init];
    if (self) {
        _statusBarStyle = UIStatusBarStyleDefault;
        _statusBarAnimation = UIStatusBarAnimationFade;
        _hidesBottomBarWhenPushed = true;
        _backgroundColor = [UIColor whiteColor];
    }
    return self;
}

+ (instancetype)defaultConfig {
    static BCNavigationBarConfig *__defaultConfig;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __defaultConfig = [[BCNavigationBarConfig alloc] init];
    });
    return __defaultConfig;
}

@end
