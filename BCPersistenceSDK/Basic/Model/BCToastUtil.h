//
//  BCToastUtil.h
//  BichanWallet
//
//  Created by 王青海 on 2018/9/25.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <BCNetworkSDK/BCNetworkSDK.h>

/*
 * 网络接口BCError=>toast文案工具类
 */
@interface BCToastUtil : NSObject

+ (NSString *)toastTextWithError:(BCError *)error;





@end
