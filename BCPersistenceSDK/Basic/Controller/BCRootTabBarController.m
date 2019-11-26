//
//  BCRootTabBarController.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCRootTabBarController.h"

@interface BCRootTabBarController ()

@end

@implementation BCRootTabBarController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    UIView *bgView = [[UIView alloc] init];
    bgView.backgroundColor = [UIColor whiteColor];
    bgView.frame = self.tabBarController.tabBar.bounds;
    [[UITabBar appearance] insertSubview:bgView atIndex:0];
    
    self.view.backgroundColor = [UIColor whiteColor];
    self.tabBarController.tabBar.backgroundColor = [UIColor whiteColor];
    self.tabBarController.tabBar.barTintColor = [UIColor whiteColor];
    self.tabBarController.tabBar.translucent = NO;
    
    // 通过appearance统一设置UITabbarItem的文字属性
    NSMutableDictionary * attrs = [NSMutableDictionary dictionary];
    attrs[NSFontAttributeName] = [UIFont systemFontOfSize:10.0];  // 设置文字大小
    attrs[NSForegroundColorAttributeName] = [UIColor grayColor];  // 设置文字的前景色
    
    NSMutableDictionary * selectedAttrs = [NSMutableDictionary dictionary];
    selectedAttrs[NSFontAttributeName] = attrs[NSFontAttributeName];
    selectedAttrs[NSForegroundColorAttributeName] = [UIColor colorWithRed:(0xffa000 & 0xFF0000) / (float)0xFF0000 green:(0xffa000 & 0xFF00) / (float)0xFF00 blue:(0xffa000 & 0xFF) / (float)0xFF  alpha:1.0];
    
    UITabBarItem * item = [UITabBarItem appearance];  // 设置appearance
    [item setTitleTextAttributes:attrs forState:UIControlStateNormal];
    [item setTitleTextAttributes:selectedAttrs forState:UIControlStateSelected];
    // 设置title偏移量
    item.titlePositionAdjustment = UIOffsetMake(0, -3);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

+ (instancetype)sharedInstance {
    static BCRootTabBarController *__sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __sharedInstance = [[BCRootTabBarController alloc] init];
    });
    return __sharedInstance;
}

@end
