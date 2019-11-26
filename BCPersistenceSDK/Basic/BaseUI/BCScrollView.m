//
//  BCScrollView.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/13.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCScrollView.h"

@implementation BCScrollView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.showsHorizontalScrollIndicator = false;
    }
    return self;
}
@end
