//
//  BCTableViewHeaderFooterView.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/9.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCTableViewHeaderFooterView.h"
#import "BCUIKit.h"



@interface BCTableViewHeaderFooterView ()

@property (nonatomic, strong) UIView *fitView;
@property (nonatomic, strong) NSLayoutConstraint *contentHeightConstraint;

@end
@implementation BCTableViewHeaderFooterView


- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
//        UIView *fitView = [[UIView alloc] init];
//        [self.contentView addSubview:fitView];
//
//        NSLayoutConstraint *topConstraint = [NSLayoutConstraint constraintWithItem:fitView attribute:(NSLayoutAttributeTop) relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:(NSLayoutAttributeTop) multiplier:1.0 constant:0];
//        [fitView addConstraint:topConstraint];
//
//        NSLayoutConstraint *bottomConstraint = [NSLayoutConstraint constraintWithItem:fitView attribute:(NSLayoutAttributeBottom) relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:(NSLayoutAttributeBottom) multiplier:1.0 constant:0];
//        [fitView addConstraint:bottomConstraint];
//
//        NSLayoutConstraint *leftConstraint = [NSLayoutConstraint constraintWithItem:fitView attribute:(NSLayoutAttributeLeading) relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:(NSLayoutAttributeLeading) multiplier:1.0 constant:0];
//        [fitView addConstraint:leftConstraint];
//
//        NSLayoutConstraint *widthConstraint = [NSLayoutConstraint constraintWithItem:fitView attribute:(NSLayoutAttributeWidth) relatedBy:NSLayoutRelationEqual toItem:nil attribute:(NSLayoutAttributeWidth) multiplier:1.0 constant:0];
//        [fitView addConstraint:widthConstraint];
//
//        NSLayoutConstraint *heightConstraint = [NSLayoutConstraint constraintWithItem:fitView attribute:(NSLayoutAttributeHeight) relatedBy:NSLayoutRelationEqual toItem:nil attribute:(NSLayoutAttributeHeight) multiplier:1.0 constant:40];
//        [fitView addConstraint:heightConstraint];
//        self.contentHeightConstraint = heightConstraint;
//

        
//        NSLayoutConstraint *heightConstraint = [NSLayoutConstraint constraintWithItem:self attribute:(NSLayoutAttributeHeight) relatedBy:NSLayoutRelationEqual toItem:nil attribute:(NSLayoutAttributeHeight) multiplier:1.0 constant:frame.size.height];
//        [self addConstraint:heightConstraint];
//        self.contentHeightConstraint = heightConstraint;
        
    }
    return self;
}

- (void)setContentHeight:(CGFloat)contentHeight {
    self.contentHeightConstraint.constant = contentHeight;
    [self setNeedsLayout];
}

- (CGFloat)contentHeight {
    return self.contentHeightConstraint.constant;
}

@end
