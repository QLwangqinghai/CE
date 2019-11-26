//
//  BCAlertController.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/7.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>


/**
 BCAlertController， UIAlertController的简易封装， 兼容 iPad
 */
@interface BCAlertController : UIAlertController

+ (instancetype)alertControllerWithTitle:(NSString *)title message:(NSString *)message preferredStyle:(UIAlertControllerStyle)preferredStyle;

@end
