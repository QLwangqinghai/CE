//
//  BCView.m
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCView.h"

@implementation UIView (base)

- (instancetype)initWithSize:(CGSize)size {
    self = [self initWithFrame:CGRectMake(0, 0, size.width, size.height)];
    return self;
}
- (instancetype)initWithWidth:(CGFloat)width height:(CGFloat)height {
    self = [self initWithFrame:CGRectMake(0, 0, width, width)];
    return self;
}


@end
