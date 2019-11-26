//
//  BCToastUtil.m
//  BichanWallet
//
//  Created by 王青海 on 2018/9/25.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCToastUtil.h"

@implementation BCToastUtil


#if Debug
+ (NSString *)toastTextWithError:(BCError *)error {
    if (error.code == 10000) {
        return @"未知错误";
    } else if (error.code == 404) {
        return @"未知错误";
    } else if (error.code == 500) {
        return @"未知错误";
    }
    
    return error.message;
}

#else

+ (NSString *)toastTextWithError:(BCError *)error {
    if (error.code == 10000) {
        return @"未知错误";
    } else if (error.code == 404) {
        return @"未知错误";
    } else if (error.code == 500) {
        return @"未知错误";
    } else if (error.code == BCNetworkErrorNoNet) {
        return @"网络错误";
    } else if (error.code == BCNetworkErrorCanceled) {
        return @"请求被取消";
    } else if (error.code == BCNetworkErrorTimeout) {
        return @"请求超时";
    } else if (error.code < 0) {
        return @"未知错误";
    }
    
    return error.message;
}

#endif






@end
