//
//  BCLabel.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BCLabel : UILabel

@property (nonatomic, assign) BOOL enableCopy;
@property (nonatomic, readonly) UILongPressGestureRecognizer *longPressGestureRecognizer;


- (void)setFontSize:(CGFloat)size;

@end


