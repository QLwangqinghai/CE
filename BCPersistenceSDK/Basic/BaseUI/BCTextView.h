//
//  BCTextView.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BCTextView : UITextView

/**
 占位字符串的 paragraphStyle
 */
@property (nonatomic, strong) NSMutableParagraphStyle *placeholderParagraphStyle;


/**
 占位字符串
 */
@property (nonatomic, copy) NSString *placeholder;

/**
 占位字符串字体
 */
@property (nonatomic, copy) UIFont *placeholderFont;

/**
 占位字符串颜色
 */
@property (strong, nonatomic) UIColor *placeholderTextColor;

/**
 占位字符串相对于TextView偏移量，默认 x=6 y=8
 */
@property (assign, nonatomic) CGPoint placeholderOffset;


@end
