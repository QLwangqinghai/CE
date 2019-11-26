//
//  BCNoDataView.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/10.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCNoDataView.h"
#import "BCUIKit.h"

@implementation BCNoDataView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        BCLabel *label = [[BCLabel alloc] init];
        [self addSubview:label];
        label.text = @"当前页面是空的";
        self.label = label;
        [label mas_makeConstraints:^(MASConstraintMaker *make) {
            make.centerX.equalTo(self.mas_centerX);
            make.centerY.equalTo(self.mas_centerY);
        }];
    }
    return self;
}

@end
