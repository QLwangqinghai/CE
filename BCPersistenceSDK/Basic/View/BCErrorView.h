//
//  BCErrorView.h
//  BichanWallet
//
//  Created by 王青海 on 2018/9/4.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCView.h"

@interface BCErrorView : UIView

@property (nonatomic, copy) void(^handleReLoad)(BCErrorView *netView);

@property (nonatomic, copy) NSString *title;

- (void)setButtonTitle:(NSString *)buttonTitle forState:(UIControlState)state;

@end
