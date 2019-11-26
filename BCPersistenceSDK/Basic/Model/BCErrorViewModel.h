//
//  BCErrorViewModel.h
//  BichanWallet
//
//  Created by 王青海 on 2018/9/4.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BCErrorViewModel : NSObject

@property (nonatomic, copy) NSString *title;
@property (nonatomic, copy) NSString *buttonTitle;

@property (nonatomic, strong, readonly) NSMutableDictionary *userInfo;



/**
 默认的错误model
 */
+ (instancetype)defaultErrorModel;

/**
 默认的无网model
 */
+ (instancetype)defaultNoNetModel;

@end
