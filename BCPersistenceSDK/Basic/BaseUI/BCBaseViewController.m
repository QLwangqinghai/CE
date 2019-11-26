//
//  BCBaseViewController.m
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCBaseViewController.h"

@interface BCBaseViewController ()
{
    NSMutableArray *__subControllersStore;
}
@property (nonatomic, readonly) NSMutableArray *subControllersStore;

@end

@implementation BCBaseViewController

- (NSMutableArray *)subControllersStore {
    if (nil == __subControllersStore) {
        __subControllersStore = [NSMutableArray array];
    }
    return __subControllersStore;
}

- (NSArray *)subControllers {
    return [self.subControllersStore copy];
}
- (void)addSubController:(UIViewController *)controller {
    if (controller) {
        [self.subControllersStore addObject:controller];
    }
}


- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.hidesBottomBarWhenPushed = true;
    }
    return self;
}

#pragma mark - 生命周期函数

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    if (self.navigationController == self.parentViewController) {
        [self.navigationController setNavigationBarHidden:self.barConfig.barHidden animated:animated];
    }
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}


#pragma mark - StatusBar状态
- (UIStatusBarStyle)preferredStatusBarStyle {
    return self.barConfig.statusBarStyle;
}
- (BOOL)prefersStatusBarHidden {
    return self.barConfig.statusBarHidden;
}

- (void)setNeedsStatusBarAppearanceUpdate {
    [super setNeedsStatusBarAppearanceUpdate];
}



#pragma mark - BCBaseViewController API
- (BCNavigationBarConfig *)barConfig {
    return [BCNavigationBarConfig defaultConfig];
}


#pragma mark - memory warning

- (void)didReceiveMemoryWarning {    
    
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
