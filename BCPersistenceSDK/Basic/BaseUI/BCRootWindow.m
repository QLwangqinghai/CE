//
//  BCRootWindow.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/16.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCRootWindow.h"
#import <MBProgressHUD/MBProgressHUD.h>
#import <Masonry/Masonry.h>

@implementation BCWindow


@end





@interface BCRootWindow ()

@property (nonatomic, strong) MBProgressHUD *hud;

@end
@implementation BCRootWindow

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        MBProgressHUD *hud = [[MBProgressHUD alloc] initWithFrame:[UIScreen mainScreen].bounds];
        hud.bezelView.color = [UIColor blackColor];
        hud.contentColor = [UIColor whiteColor];
        hud.backgroundView.color = [UIColor clearColor];
        hud.removeFromSuperViewOnHide = false;

        [self addSubview:hud];
        [hud mas_makeConstraints:^(MASConstraintMaker *make) {
            make.edges.equalTo(self).insets(UIEdgeInsetsZero);
        }];

        self.hud = hud;
    }
    return self;
}


- (void)showLoading {
    [self.hud showAnimated:true];
}
- (void)cancelLoading {
    [self.hud hideAnimated:true];
}

+ (instancetype)sharedInstance {
    static BCRootWindow *__sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __sharedInstance = [[BCRootWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    });
    return __sharedInstance;
}

@end
