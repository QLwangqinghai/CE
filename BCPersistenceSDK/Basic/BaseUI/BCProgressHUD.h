//
//  BCProgressHUD.h
//  BichanWallet
//
//  Created by 陈少楠 on 2018/8/7.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>


typedef NS_ENUM(NSUInteger, BCProgressMode) {
    BCProgressModeOnlyText = 10,           //文字
    BCProgressModeLoading,              //加载菊花
    BCProgressModeCircleLoading,         //加载圆形
    BCProgressModeSuccess               //成功
};


@class MBProgressHUD;
@interface BCProgressHUD : NSObject
/*===============================   属性   ================================================*/

@property (nonatomic,strong) MBProgressHUD  *hud;


/*===============================   方法   ================================================*/

+ (instancetype)sharedInstance;

//显示
+ (void)show:(NSString *)msg inView:(UIView *)view mode:(BCProgressMode)mode;

//隐藏
+ (void)hide;

//只显示文字
+ (void)showOnlyText:(NSString *)msg inView:(UIView *)view;

//显示进度
+ (void)showProgress:(NSString *)msg inView:(UIView *)view;

//显示成功提示
+ (void)showSuccess:(NSString *)msg inview:(UIView *)view;

//显示提示（1秒后消失）
+ (void)showMessage:(NSString *)msg inView:(UIView *)view;

//显示提示（N秒后消失）
+ (void)showMessage:(NSString *)msg inView:(UIView *)view afterDelayTime:(NSInteger)delay;

//在最上层显示
+ (void)showMsgWithoutView:(NSString *)msg;

+ (void)showSuccess:(NSString *)success;
+ (void)showError:(NSString *)error;
+ (MBProgressHUD *)showMessage:(NSString *)message;
+ (void)hideHUD;

+ (void)showSuccess:(NSString *)success toView:(UIView *)view;
+ (void)showError:(NSString *)error toView:(UIView *)view;
+ (MBProgressHUD *)showMessage:(NSString *)message toView:(UIView *)view;
+ (void)hideHUDForView:(UIView *)view;


@end
