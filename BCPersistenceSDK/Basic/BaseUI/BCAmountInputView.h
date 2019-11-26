//
//  BCAmountInputView.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/17.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCBaseInputView.h"

@class BCTextView;



/**
 金额输入框
 */
@interface BCAmountInputView : BCBaseInputView

@property (nonatomic, assign) BOOL allowDecimalsInput;//允许输入小数, 默认值 YES
@property (nonatomic, assign) NSUInteger integerNumberCount;//允许输入整数的个数 0 表示不限制， 默认值0
@property (nonatomic, assign) NSUInteger decimalsNumberCount;//允许输入小数的个数 0 表示不限制， 默认值0


+ (BOOL)isValidInputs:(NSString *)input maxIntegerNumberCount:(NSUInteger)maxIntegerNumberCount allowDecimalsInput:(BOOL)allowDecimalsInput maxDecimalsNumberCount:(NSUInteger)maxDecimalsNumberCount;

@end
