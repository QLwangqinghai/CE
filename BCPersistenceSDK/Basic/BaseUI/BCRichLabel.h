//
//  BCRichLabel.h
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCRichModel.h"

/**
 富文本label
 */
@interface BCRichLabel : UIView

@property (nonatomic, weak) YYLabel *label;
@property (nonatomic, strong) BCRichModel *model;
@property (nonatomic, assign) CGPoint labelOffset;

@property (nonatomic, assign) BOOL enableCopy;
@property (nonatomic, readonly) UILongPressGestureRecognizer *longPressGestureRecognizer;

@end
