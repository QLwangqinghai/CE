//
//  BCErrorViewModel.m
//  BichanWallet
//
//  Created by 王青海 on 2018/9/4.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCErrorViewModel.h"


@interface BCErrorViewModel ()
@property (nonatomic, strong) NSMutableDictionary *userInfo;
@end
@implementation BCErrorViewModel

- (NSMutableDictionary *)userInfo {
    if (nil == _userInfo) {
        _userInfo = [NSMutableDictionary dictionary];
    }
    return _userInfo;
}


/**
 默认的错误model
 */
+ (instancetype)defaultErrorModel {
    static BCErrorViewModel *__defaultErrorModel = nil;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        BCErrorViewModel *model = [[BCErrorViewModel alloc] init];
        model.title = @"币产正在努力，还请小主手动刷新一下~";
        model.buttonTitle = @"刷新";
        __defaultErrorModel = model;
    });
    return __defaultErrorModel;
}

/**
 默认的无网model
 */
+ (instancetype)defaultNoNetModel {
    static BCErrorViewModel *__defaultNoNetModel = nil;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        BCErrorViewModel *model = [[BCErrorViewModel alloc] init];
        model.title = @"网络连接异常, 请重新尝试";
        model.buttonTitle = @"重新加载";
        __defaultNoNetModel = model;
    });
    return __defaultNoNetModel;
}

@end

