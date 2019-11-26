//
//  BCViewControllerUtil.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/8.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>


/**
 ViewController 工具类
 */
@interface BCViewControllerUtil : NSObject



/**
 获取栈顶控制器

 @return 栈顶控制器
 */
+ (UIViewController *)currentViewController;


/**
 获取vc栈顶控制器
 
 @return vc栈顶控制器
 */
+ (UIViewController *)findBestViewController:(UIViewController *)vc;



+ (UIViewController *)topNoAlertViewController;
+ (UIViewController *)findUpNoAlertViewController:(UIViewController *)vc;

@end
