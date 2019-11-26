//
//  BCAlertController.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/7.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCAlertController.h"
#import "BCUIUtil.h"


@interface BCAlertController ()
@property(nonatomic,strong) UIWindow *alertWindow;

@end
@implementation BCAlertController

+ (instancetype)alertControllerWithTitle:(NSString *)title message:(NSString *)message preferredStyle:(UIAlertControllerStyle)preferredStyle {
    BCAlertController *alertController = [super alertControllerWithTitle:title message:message preferredStyle:preferredStyle];
    if(![BCUIUtil isIPhone]) {
        alertController.popoverPresentationController.sourceView = [UIApplication sharedApplication].delegate.window.rootViewController.view;
        alertController.popoverPresentationController.sourceRect = CGRectMake(0, 0, 1.0, 1.0);
    }
    return alertController;
}
- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

//- (void)showAnimated:(BOOL)animated {
//    [self showAnimated:animated completion:nil];
//}
//
//- (void)showAnimated:(BOOL)animated completion:(void (^ __nullable)(void))completion {
//    
//    UIWindow *window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
//    [window setBackgroundColor:[UIColor clearColor]];
//    UIViewController*rootViewController = [[UIViewController alloc] init];
//    [[rootViewController view] setBackgroundColor:[UIColor clearColor]];
//    
//    [window setWindowLevel:UIWindowLevelAlert + 1];
//    [window makeKeyAndVisible];
//    [self setAlertWindow:window];
//    [window setRootViewController:rootViewController];
//    
//    [rootViewController presentViewController:self animated:animated completion:completion];
//}

@end
