//
//  BCViewController.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCViewController.h"
#import <Masonry/Masonry.h>
#import "BCImageUtil.h"
#import <MBProgressHUD/MBProgressHUD.h>
#import "BCNoDataView.h"
#import <BCUserCenterSDK.h>

@interface BCViewController ()

@property (nonatomic, assign) BOOL isShowingErrorView;
@property (nonatomic, assign) BOOL isShowingNoNetView;
@property (nonatomic, assign) BCViewControllerLoadingType loadingType;

@property (nonatomic, strong) BCErrorView *errorView;
@property (nonatomic, strong) UIView *loadingView;
@property (nonatomic, strong) UIView *fullLoadingView;
@property (nonatomic, strong) UIView *noDataView;

@property (nonatomic, strong) UIBarButtonItem *navigationBarLeftItem;

@end

@implementation BCViewController

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.navigationController.navigationBar.barTintColor = self.barConfig.backgroundColor;
    [self.navigationController.navigationBar setTitleTextAttributes:@{NSFontAttributeName:[UIFont boldSystemFontOfSize:16],NSForegroundColorAttributeName:[UIColor colorWithRed:55/255.0f green:55/255.0f blue:55/255.0f alpha:1.0]}];
    

}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.view.backgroundColor = [UIColor whiteColor];
//        if (@available(iOS 11.0, *)) {
//            self.automaticallyAdjustsScrollViewInsets = YES;
//        } else {
//            self.automaticallyAdjustsScrollViewInsets = NO;
//        }
    [self p_refreshNavigationBarLeftItem];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}



#pragma mark - getter & setter

- (void)p_refreshNavigationBarLeftItem {
    UIBarButtonItem *item = nil;
    if (_navigationBarLeftItemType != BCNavigationBarLeftItemTypeCustom) {
        if (nil == self.navigationBarLeftItem) {
            self.navigationBarLeftItem = [self makeNavigationBarDefaultLeftItem];
        }
        item = _navigationBarLeftItem;
    } else {
        item = _customNavigationBarLeftItem;
    }
    if (item != self.navigationItem.leftBarButtonItem) {
        self.navigationItem.leftBarButtonItem = item;
    }
}

- (void)setNavigationBarLeftItem:(UIBarButtonItem *)navigationBarLeftItem {
    if (_navigationBarLeftItem != navigationBarLeftItem) {
        _navigationBarLeftItem = navigationBarLeftItem;
        [self p_refreshNavigationBarLeftItem];
    }
}
- (void)setCustomNavigationBarLeftItem:(UIBarButtonItem *)customNavigationBarLeftItem {
    if (_customNavigationBarLeftItem != customNavigationBarLeftItem) {
        _customNavigationBarLeftItem = customNavigationBarLeftItem;
        [self p_refreshNavigationBarLeftItem];
    }
}
- (void)setNavigationBarLeftItemType:(BCNavigationBarLeftItemType)navigationBarLeftItemType {
    if (_navigationBarLeftItemType != navigationBarLeftItemType) {
        _navigationBarLeftItemType = navigationBarLeftItemType;
        if (navigationBarLeftItemType != BCNavigationBarLeftItemTypeCustom) {
            if (nil == self.navigationBarLeftItem) {
                self.navigationBarLeftItem = [self makeNavigationBarDefaultLeftItem];
            }
        }
        [self p_refreshNavigationBarLeftItem];
    }
}

- (BOOL)isLoading {
    return self.loadingType != BCViewControllerLoadingTypeNone;
}


#pragma mark - 对外API
- (void)showErrorViewWithError:(BCError *)error {
    if (error.code == BCNetworkErrorNoNet) {
        [self showErrorViewWithModel:[BCErrorViewModel defaultNoNetModel] reloadAction:nil];
    } else {
        [self showErrorViewWithModel:[BCErrorViewModel defaultErrorModel] reloadAction:nil];
    }
}
- (void)showErrorViewWithError:(BCError *)error reloadAction:(BOOL(^)(BCErrorView *netView, __kindof BCViewController *controller))action {
    if (error.code == BCNetworkErrorNoNet) {
        [self showErrorViewWithModel:[BCErrorViewModel defaultNoNetModel] reloadAction:action];
    } else {
        [self showErrorViewWithModel:[BCErrorViewModel defaultErrorModel] reloadAction:action];
    }
}
- (void)showErrorViewWithModel:(BCErrorViewModel *)model reloadAction:(BOOL(^)(BCErrorView *netView, __kindof BCViewController *controller))action {
    BCErrorView *errorView = self.errorView;

    if (!self.isShowingErrorView) {
        if (nil == errorView) {
            errorView = [self makeErrorView];
            self.errorView = errorView;
        } else {
            [errorView removeFromSuperview];
        }
        if (errorView) {
            [self.view addSubview:errorView];
            [errorView mas_makeConstraints:^(MASConstraintMaker *make) {
                make.edges.equalTo(self.view).insets(UIEdgeInsetsZero);
            }];
            self.isShowingErrorView = true;
        }
    }
    errorView.title = model.title;
    [errorView setButtonTitle:model.buttonTitle forState:(UIControlStateNormal)];
    
    
    if (action) {
        WeakSelf;
        errorView.handleReLoad = ^(BCErrorView *netView) {
            WeakSelfToStrongSelfGuardSelf;
            
            if (action(netView, strongSelf)) {
                [strongSelf hideErrorView];
            }
        };
    } else {
        WeakSelf;
        errorView.handleReLoad = ^(BCErrorView *netView) {
            WeakSelfToStrongSelfGuardSelf;
            
            BOOL result = [strongSelf handleErrorViewReloadAction];
            
            if (result) {
                [strongSelf hideErrorView];
            }
        };
    }
}


- (void)hideErrorView {
    [self.errorView removeFromSuperview];
    self.isShowingErrorView = false;
}

- (void)fullLoading {
    switch (self.loadingType) {
        case BCViewControllerLoadingTypeFullLoading:
        {
            //不做任何事
        }
            break;
        case BCViewControllerLoadingTypeLoading:
        {
            [self.loadingView removeFromSuperview];
        }
        default:
        {
            UIView *fullLoadingView = self.fullLoadingView;
            if (nil == fullLoadingView) {
                fullLoadingView = [self makeFullLoadingView];
                self.fullLoadingView = fullLoadingView;
            } else {
                [self.fullLoadingView removeFromSuperview];
            }
            if (fullLoadingView) {
                [self.view addSubview:fullLoadingView];
                [fullLoadingView mas_makeConstraints:^(MASConstraintMaker *make) {
                    make.edges.equalTo(self.view).insets(UIEdgeInsetsZero);
                }];
                self.loadingType = BCViewControllerLoadingTypeFullLoading;
                if ([fullLoadingView isKindOfClass:[MBProgressHUD class]]) {
                    [(MBProgressHUD *)fullLoadingView showAnimated:true];
                }
            }
        }
            break;
    }
}
- (void)loading {
    switch (self.loadingType) {
        case BCViewControllerLoadingTypeLoading:
        {
            //不做任何事
        }
            break;
        case BCViewControllerLoadingTypeFullLoading:
        {
            [self.fullLoadingView removeFromSuperview];
        }
        default:
        {
            UIView *loadingView = self.loadingView;
            if (nil == loadingView) {
                loadingView = [self makeLoadingView];
                self.loadingView = loadingView;
            } else {
                [self.loadingView removeFromSuperview];
            }
            if (loadingView) {
                [self.view addSubview:loadingView];
                [loadingView mas_makeConstraints:^(MASConstraintMaker *make) {
//                    make.centerX.equalTo(self.view.mas_centerX);
//                    make.centerY.equalTo(self.view.mas_centerY);
                    make.edges.equalTo(self.view).insets(UIEdgeInsetsZero);
                }];
                self.loadingType = BCViewControllerLoadingTypeLoading;
                if ([loadingView isKindOfClass:[MBProgressHUD class]]) {
                    [(MBProgressHUD *)loadingView showAnimated:true];
                }
            }
        }
            break;
    }
}
- (void)cancelLoading {
    switch (self.loadingType) {
        case BCViewControllerLoadingTypeFullLoading:
        {
            [self.fullLoadingView removeFromSuperview];
            self.loadingType = BCViewControllerLoadingTypeNone;
        }
            break;
        case BCViewControllerLoadingTypeLoading:
        {
            [self.loadingView removeFromSuperview];
            self.loadingType = BCViewControllerLoadingTypeNone;
        }
            break;
        default:
            break;
    }
}


- (void)showSuccess:(NSString *)success time:(double)time {
    MBProgressHUD *hud = [[MBProgressHUD alloc] initWithView:self.view];
    hud.bezelView.color = [UIColor blackColor];
    hud.contentColor = [UIColor whiteColor];
    hud.backgroundView.color = [UIColor clearColor];
    
    hud.mode = MBProgressHUDModeText;
    hud.label.text = nil == success ? @"" : success;
    [self.view addSubview:hud];
    hud.removeFromSuperViewOnHide = true;
    [hud showAnimated:true];
    [hud hideAnimated:true afterDelay:time];
}

- (void)showError:(NSString *)error time:(double)time {
    if (error.length <= 0) {
        return;
    }
    MBProgressHUD *hud = [[MBProgressHUD alloc] initWithView:self.view];
    hud.bezelView.color = [UIColor blackColor];
    hud.contentColor = [UIColor whiteColor];
    hud.backgroundView.color = [UIColor clearColor];
    
    hud.mode = MBProgressHUDModeText;
    hud.label.text = nil == error ? @"未知错误" : error;
    [self.view addSubview:hud];
    hud.removeFromSuperViewOnHide = true;
    [hud showAnimated:true];
    [hud hideAnimated:true afterDelay:time];
}

- (void)showSuccess:(NSString *)success {
    [self showSuccess:success time:2];
}

- (void)showError:(NSString *)error {
    [self showError:error time:2];
}

- (NSString *)tostTextWithError:(BCError *)error {
//    if (error.code == 10000) {
//        return @"未知错误";
//    }
    return error.message;
}

- (void)showTostWithError:(BCError *)error {
    [self showError:[self tostTextWithError:error]];
}

- (void)showNoDataViewToView:(UIView *)view {
    UIView *targetView = view;
    if (nil == targetView) {
        targetView = self.view;
    }
    
    UIView *noDataView = self.noDataView;
    if (nil == noDataView) {
        noDataView = [self makeNoDataView];
        self.noDataView = noDataView;
    } else {
        [self.noDataView removeFromSuperview];
    }
    if (noDataView) {
        if ([targetView isKindOfClass:[UIScrollView class]]) {
            [targetView addSubview:noDataView];
            noDataView.frame = targetView.bounds;
        } else {
            [targetView addSubview:noDataView];
            [noDataView mas_makeConstraints:^(MASConstraintMaker *make) {
                make.left.equalTo(targetView.mas_left);
                make.right.equalTo(targetView.mas_right);
                make.top.equalTo(targetView.mas_top);
                make.bottom.equalTo(targetView.mas_bottom);
            }];
        }
    }
}
- (void)hideNoDataView {
    [self.noDataView removeFromSuperview];
}

- (UIView *)makeNoDataView {
    BCNoDataView *noNetView = [[BCNoDataView alloc] initWithFrame:self.view.bounds];
    return noNetView;
}
- (BCErrorView *)makeErrorView {
    BCErrorView *noNetView = [[BCErrorView alloc] init];
    return noNetView;
}

- (BOOL)handleErrorViewReloadAction {
    return false;
}


- (UIView *)makeFullLoadingView {
    MBProgressHUD *hud = [[MBProgressHUD alloc] initWithFrame:[UIScreen mainScreen].bounds];
    hud.bezelView.color = [UIColor blackColor];
    hud.contentColor = [UIColor whiteColor];
    hud.backgroundView.color = [UIColor clearColor];
    return hud;
}

- (UIView *)makeLoadingView {
    MBProgressHUD *hud = [[MBProgressHUD alloc] initWithFrame:CGRectMake(0, 0, 100, 100)];
    hud.bezelView.color = [UIColor blackColor];
    hud.contentColor = [UIColor whiteColor];
    hud.backgroundView.color = [UIColor clearColor];
    return hud;
}

- (UIBarButtonItem *)makeNavigationBarDefaultLeftItem {
    BCIntrinsicContentSizeView * itemView = [[BCIntrinsicContentSizeView alloc] initWithWidth:44 height:44];
    itemView.backgroundColor = [UIColor clearColor];
    UIButton *btn = [[UIButton alloc] init];
    btn.exclusiveTouch = YES;
    btn.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
    btn.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
    [btn setImage:[BCImageUtil imageNamed:@"icon_nav_back"] forState:UIControlStateNormal];
    [btn setImageEdgeInsets:UIEdgeInsetsMake(0, 6, 0, 0)];
    [itemView addSubview:btn];
    [btn addTarget:self action:@selector(p_handleLeftItemAction:) forControlEvents:UIControlEventTouchUpInside];
    [btn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.equalTo(itemView.mas_left);
        make.centerY.equalTo(itemView.mas_centerY);
        make.width.equalTo(itemView.mas_width);
        make.height.equalTo(itemView.mas_height);
    }];
    
    UIBarButtonItem *buttonItem = [[UIBarButtonItem alloc] initWithCustomView:itemView];
    return buttonItem;
}

- (void)p_handleLeftItemAction:(UIButton *)button {
    [self handleNavigationBarLeftItemAction];
}

- (void)handleNavigationBarLeftItemAction {
    if (self.onWillBackBlock) {
        self.onWillBackBlock(self);
    }
    if ([[self.navigationController viewControllers] firstObject] == self) {
        [self.view endEditing:YES];
        [self dismissViewControllerAnimated:YES completion:nil];
    } else {
        [self.view endEditing:YES];
        [self.navigationController popViewControllerAnimated:YES];
    }
}



@end






//extern NSString *const BCErrorViewAttributeKeyType;
//NSString *const BCErrorViewAttributeKeyType = @"BCErrorViewAttributeKeyType";




