//
//  BCErrorView.m
//  BichanWallet
//
//  Created by 王青海 on 2018/9/4.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCErrorView.h"
#import "BCUIKit.h"
#import <Masonry/Masonry.h>


@interface BCErrorView ()

@property (nonatomic, weak) BCLabel *label;
@property (nonatomic, weak) UIButton *button;

@end
@implementation BCErrorView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor whiteColor];
        
        BCLabel *label = [[BCLabel alloc] init];
        label.numberOfLines = 1;
        [label setFontSize:14];
        label.textColor = [UIColor colorWithRed:141.0/255 green:141.0/255 blue:141.0/255 alpha:1.0];
        [self addSubview:label];
        
        [label mas_makeConstraints:^(MASConstraintMaker *make) {
            make.centerX.equalTo(self.mas_centerX);
            make.centerY.equalTo(self.mas_centerY).offset(-30);
        }];
        
        UIButton *button = [[UIButton alloc] init];
        [self addSubview:button];
        [button setTitleColor:[UIColor colorWithRed:255.0f/255.0f green:160.0f/255.0f blue:0.0f/255.0f alpha:1.0f] forState:UIControlStateNormal];
        [button setTitle:@"重新加载" forState:(UIControlStateNormal)];
        [button mas_makeConstraints:^(MASConstraintMaker *make) {
            make.centerX.equalTo(self.mas_centerX);
            make.centerY.equalTo(label.mas_centerY).offset(50);
            make.width.equalTo(@80);
            make.height.equalTo(@30);
        }];
        button.titleLabel.font = [UIFont systemFontOfSize:14];
        [button addTarget:self action:@selector(onButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
        
        button.layer.borderWidth = 1.0 / [UIScreen mainScreen].scale;
        button.layer.borderColor = [UIColor colorWithRed:255.0f/255.0f green:160.0f/255.0f blue:0.0f/255.0f alpha:1.0f].CGColor;
        
        self.label = label;
        self.button = button;
        
    }
    return self;
}

- (void)onButtonClicked:(UIButton *)button {
    if (self.handleReLoad) {
        self.handleReLoad(self);
    }
}

- (void)setTitle:(NSString *)title {
    _title = [title copy];
    self.label.text = title;
}

- (void)setButtonTitle:(NSString *)buttonTitle forState:(UIControlState)state {
    [self.button setTitle:buttonTitle forState:(UIControlStateNormal)];
}

@end
