//
//  BCBaseBCAppDelegate.h
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <UIKit/UIKit.h>

/*
 AppDelegate 的基类，用于减少主工程中AppDelete 中的代码， 解决子模块调用AppDelegate的问题
 */
@interface BCBaseAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;


- (UIViewController *)loadWindowRootViewController;


@end
