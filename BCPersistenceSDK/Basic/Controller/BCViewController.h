//
//  BCViewController.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCUIKit.h"

#import "BCErrorViewModel.h"
#import "BCErrorView.h"
#import <BCNetworkSDK/BCNetworkSDK.h>
#import "BCToastUtil.h"


typedef NS_ENUM(NSUInteger, BCViewControllerLoadingType) {
    BCViewControllerLoadingTypeNone,//没有加载
    BCViewControllerLoadingTypeLoading,//普通加载
    BCViewControllerLoadingTypeFullLoading,//
};

typedef NS_ENUM(NSUInteger, BCNavigationBarLeftItemType) {
    BCNavigationBarLeftItemTypeDefault,//默认有个返回按钮
    BCNavigationBarLeftItemTypeCustom,//子类自定义导航栏左侧按钮
};


@interface BCViewController : BCBaseViewController
{
    @protected
    UIView *_loadingView;
    UIView *_fullLoadingView;
    BCErrorView *_errorView;
    UIBarButtonItem *_navigationBarLeftItem;
}

//是否正在显示错误页, 支持KVO
@property (nonatomic, assign, readonly) BOOL isShowingErrorView;

//是否正在显示无网页, 支持KVO
@property (nonatomic, assign, readonly) BOOL isShowingNoNetView;

//是否正在加载， 注意loadingView 和fullLoadingView 是互斥的关系, 支持KVO
@property (nonatomic, assign, readonly) BCViewControllerLoadingType loadingType;
@property (nonatomic, assign, readonly) BOOL isLoading;

@property (nonatomic, strong, readonly) BCErrorView *errorView;//lazy 默认布局四边对齐, 支持KVO
@property (nonatomic, strong, readonly) UIView *loadingView;//lazy 默认中心对齐, 支持KVO
@property (nonatomic, strong, readonly) UIView *fullLoadingView;//lazy 默认布局四边对齐, 支持KVO
@property (nonatomic, strong, readonly) UIView *noDataView;//lazy 默认布局四边对齐, 支持KVO


@property (nonatomic, strong, readonly) UIBarButtonItem *navigationBarLeftItem;
@property (nonatomic, assign) BCNavigationBarLeftItemType navigationBarLeftItemType;
@property (nonatomic, strong) UIBarButtonItem *customNavigationBarLeftItem;

@property (nonatomic, copy) void(^onWillBackBlock)(BCViewController *controller);



#pragma mark - UI
//创建导航栏左边item， 子类可以重写
- (UIBarButtonItem *)makeNavigationBarDefaultLeftItem;


#pragma mark - 对外API

- (void)showErrorViewWithError:(BCError *)error;
- (void)showErrorViewWithError:(BCError *)error reloadAction:(BOOL(^)(BCErrorView *netView, __kindof BCViewController *controller))action;
- (void)showErrorViewWithModel:(BCErrorViewModel *)model reloadAction:(BOOL(^)(BCErrorView *netView, __kindof BCViewController *controller))action;


- (void)hideErrorView;

- (BOOL)handleErrorViewReloadAction;

/**
 全屏的loading， 一般用于页面第一次加载或者 页面为空白页面时的加载
 */
- (void)fullLoading;


/**
 用于刷新页面 (页面已经展示， 刷新数据)
 */
- (void)loading;


/**
 取消loading，包括 fullLoading、loading
 */
- (void)cancelLoading;

- (void)showSuccess:(NSString *)success time:(double)time;
- (void)showError:(NSString *)error time:(double)time;

/**
 tost

 @param success 成功的文案
 */
- (void)showSuccess:(NSString *)success;

/**
 tost
 
 @param error 错误的文案
 */
- (void)showError:(NSString *)error;

- (NSString *)tostTextWithError:(BCError *)error;

- (void)showTostWithError:(BCError *)error;


- (void)showNoDataViewToView:(UIView *)view;
- (void)hideNoDataView;



//处理导航栏左边item事件， 子类可以重写
- (void)handleNavigationBarLeftItemAction;


//
////创建一个加载页(全屏的， 一般用于第一次进入页面， 主要用于空页面方式看起来很空)
//- (UIView *)makeFullLoadingView;
//
////创建一个加载页(一般用于页面局部刷新，eg.小菊花)
//- (UIView *)makeLoadingView;


@end
