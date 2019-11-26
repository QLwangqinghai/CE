//
//  BCIntrinsicContentSizeView.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/9.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCView.h"



/**
 固定宽高的view, 一般用于自定义导航栏的view （如： titleView， BarItem的customView）
 */
@interface BCIntrinsicContentSizeView : UIView

@property (nonatomic, assign) CGSize contentSize;

@end
