//
//  BCInputCodeAlertController.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/8.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCCustomAlertController.h"
#import "BCView.h"



/**
 * 输入支付密码、验证码等页面
 */

@interface BCInputCodeView : UIView

@property (nonatomic, copy) NSString *alertTitle;


@end



@interface BCInputCodeAlertController : BCCustomAlertController

@property (nonatomic, readonly) BCInputCodeView *inputCodeView;
@property (nonatomic, copy) void(^onCancel)(BCInputCodeAlertController *alert);
@property (nonatomic, copy) BOOL(^willSuccess)(BCInputCodeAlertController *alert, NSString *key);
@property (nonatomic, copy) void(^onSuccess)(BCInputCodeAlertController *alert, NSString *key);


- (void)showAnimate:(BOOL)animate;

- (void)showAnimate:(BOOL)animate completion:(void (^)(void))completion;


@end


