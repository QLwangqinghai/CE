//
//  BCProgressHUD.m
//  BichanWallet
//
//  Created by 陈少楠 on 2018/8/7.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCProgressHUD.h"
#import "BCColorUtil.h"
#import "BCImageUtil.h"
#import <Masonry/Masonry.h>

#import <MBProgressHUD/MBProgressHUD.h>


@implementation BCProgressHUD

+ (instancetype)sharedInstance {
    
    static BCProgressHUD *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[BCProgressHUD alloc] init];
    });
    
    return instance;
    
}

+ (void)show:(NSString *)msg inView:(UIView *)view mode:(BCProgressMode)myMode{
    
    if (msg.length == 0) {
        msg = @"未知错误";
    }
    //如果已有弹框，先消失
    if ([BCProgressHUD sharedInstance].hud != nil) {
        [[BCProgressHUD sharedInstance].hud hideAnimated:YES];
        [BCProgressHUD sharedInstance].hud = nil;
    }
    
    //4\4s屏幕避免键盘存在时遮挡
    if ([UIScreen mainScreen].bounds.size.height == 480) {
        [view endEditing:YES];
    }
    
    [BCProgressHUD sharedInstance].hud = [MBProgressHUD showHUDAddedTo:view animated:YES];
    //[BCProgressHUD sharedInstance].hud.dimBackground = YES;    //是否显示透明背景
    [BCProgressHUD sharedInstance].hud.bezelView.color = [UIColor blackColor];

    [[BCProgressHUD sharedInstance].hud setMargin:10];
    [[BCProgressHUD sharedInstance].hud setUserInteractionEnabled:NO];
    [[BCProgressHUD sharedInstance].hud setRemoveFromSuperViewOnHide:YES];
    [BCProgressHUD sharedInstance].hud.detailsLabel.text = msg;
    [BCProgressHUD sharedInstance].hud.contentColor = [UIColor whiteColor];
    [BCProgressHUD sharedInstance].hud.detailsLabel.font = [UIFont systemFontOfSize:14];
    switch ((NSInteger)myMode) {
        case BCProgressModeOnlyText:
            [BCProgressHUD sharedInstance].hud.mode = MBProgressHUDModeText;
            break;
            
        case BCProgressModeLoading:
            [BCProgressHUD sharedInstance].hud.mode = MBProgressHUDModeIndeterminate;
            break;
            
        case BCProgressModeCircleLoading:
            [BCProgressHUD sharedInstance].hud.mode = MBProgressHUDModeDeterminate;
            break;
            
        case BCProgressModeSuccess:
            [BCProgressHUD sharedInstance].hud.mode = MBProgressHUDModeCustomView;
            [BCProgressHUD sharedInstance].hud.customView = [[UIImageView alloc] initWithImage:[BCImageUtil imageNamed:@"success"]];
            break;
            
        default:
            break;
    }
}


+(void)hide{
    if ([BCProgressHUD sharedInstance].hud != nil) {
        [[BCProgressHUD sharedInstance].hud hideAnimated:YES];
    }
}


+(void)showMessage:(NSString *)msg inView:(UIView *)view{
    if (view == nil)
    view = [[UIApplication sharedApplication].windows lastObject];
    [self show:msg inView:view mode:BCProgressModeOnlyText];
    [[BCProgressHUD sharedInstance].hud hideAnimated:YES afterDelay:1.5];
}


+(void)showOnlyText:(NSString *)msg inView:(UIView *)view{
    [self show:msg inView:view mode:BCProgressModeOnlyText];
}

+(void)showMessage:(NSString *)msg inView:(UIView *)view afterDelayTime:(NSInteger)delay{
    [self show:msg inView:view mode:BCProgressModeOnlyText];
    [[BCProgressHUD sharedInstance].hud hideAnimated:YES afterDelay:delay];
}

+(void)showSuccess:(NSString *)msg inview:(UIView *)view{
    [self show:msg inView:view mode:BCProgressModeSuccess];
    [[BCProgressHUD sharedInstance].hud hideAnimated:YES afterDelay:1.0];
}


+(void)showProgress:(NSString *)msg inView:(UIView *)view{
    [self show:msg inView:view mode:BCProgressModeLoading];
}

+(void)showMsgWithoutView:(NSString *)msg{
    if (msg.length == 0) {
        msg = @"未知错误";
    }
    UIWindow *view = [[UIApplication sharedApplication].windows lastObject];
    [self show:msg inView:view mode:BCProgressModeOnlyText];
    [[BCProgressHUD sharedInstance].hud hideAnimated:YES afterDelay:2];
}


+ (void)show:(NSString *)text image:(UIImage *)image view:(UIView *)view {
    if (view == nil)
        view = [[UIApplication sharedApplication].windows lastObject];
    MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:view animated:YES];
    hud.label.text = text;
    
    hud.label.textColor = [BCColorUtil colorWithHexString:@"#333333"];
    hud.label.font = [UIFont systemFontOfSize:17.0];
    hud.userInteractionEnabled= NO;
    
    hud.customView = [[UIImageView alloc] initWithImage:image];
    hud.bezelView.backgroundColor = [UIColor colorWithRed:0/255.0f green:0/255.0f blue:0/255.0f alpha:0.5];
    
    hud.mode = MBProgressHUDModeCustomView;
    
    hud.removeFromSuperViewOnHide = YES;
    [hud hideAnimated:YES afterDelay:2];
}


+ (void)showSuccess:(NSString *)success toView:(UIView *)view time:(double)time {
    if (success.length <= 0) {
        return;
    }
    UIView *targetView = view;
    if (nil == targetView) {
        targetView = [[UIApplication sharedApplication].windows lastObject];
    }
    
    MBProgressHUD *hud = [[MBProgressHUD alloc] initWithView:targetView];
    hud.bezelView.color = [UIColor blackColor];
    hud.contentColor = [UIColor whiteColor];
    hud.backgroundView.color = [UIColor clearColor];
    
    hud.mode = MBProgressHUDModeText;
    hud.label.text = nil == success ? @"" : success;
    [targetView addSubview:hud];
    hud.removeFromSuperViewOnHide = true;
    [hud showAnimated:true];
    [hud hideAnimated:true afterDelay:time];
}

+ (void)showError:(NSString *)error toView:(UIView *)view time:(double)time {
    if (error.length <= 0) {
        return;
    }
    UIView *targetView = view;
    if (nil == targetView) {
        targetView = [[UIApplication sharedApplication].windows lastObject];
    }
    
    MBProgressHUD *hud = [[MBProgressHUD alloc] initWithView:targetView];
    hud.bezelView.color = [UIColor blackColor];
    hud.contentColor = [UIColor whiteColor];
    hud.backgroundView.color = [UIColor clearColor];
    
    hud.mode = MBProgressHUDModeText;
    hud.label.text = nil == error ? @"未知错误" : error;
    [targetView addSubview:hud];
    hud.removeFromSuperViewOnHide = true;
    [hud showAnimated:true];
    [hud hideAnimated:true afterDelay:time];
}

+ (void)showSuccess:(NSString *)success {
    [self showSuccess:success toView:nil time:2];
}

+ (void)showError:(NSString *)error {
    [self showError:error toView:nil time:2];
}


//+ (void)showSuccess:(NSString *)success {
//    [self showSuccess:success toView:nil];
//}

+ (void)showSuccess:(NSString *)success toView:(UIView *)view {
    [self showSuccess:success toView:view time:2];
}

//+ (void)showError:(NSString *)error {
//    [self showError:error toView:nil];
//}

+ (void)showError:(NSString *)error toView:(UIView *)view {
    [self showError:error toView:view time:2];
}

+ (MBProgressHUD *)showMessage:(NSString *)message {
    return [self showMessage:message toView:nil];
}

+ (MBProgressHUD *)showMessage:(NSString *)message toView:(UIView *)view {
    if (view == nil) view = [[UIApplication sharedApplication].windows lastObject];
    MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:view animated:YES];
    hud.label.text = message;
    hud.removeFromSuperViewOnHide = YES;
    
    hud.backgroundView.style = MBProgressHUDBackgroundStyleSolidColor;
    hud.backgroundView.color = [UIColor clearColor];
    hud.removeFromSuperViewOnHide = YES;
    [hud hideAnimated:YES afterDelay:2];

    return hud;
}

+ (void)hideHUD {
    [self hideHUDForView:nil];
}

+ (void)hideHUDForView:(UIView *)view {
    if (view == nil) {
        view = [[UIApplication sharedApplication].windows lastObject];
    }
    [MBProgressHUD hideHUDForView:view animated:YES];
}



@end
