//
//  BCRootWindow.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/16.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface BCWindow : UIWindow

@end


@interface BCRootWindow : BCWindow


- (void)showLoading;
- (void)cancelLoading;



+ (instancetype)sharedInstance;

@end
