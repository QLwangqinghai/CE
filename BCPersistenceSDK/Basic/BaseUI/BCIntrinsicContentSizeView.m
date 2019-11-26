//
//  BCIntrinsicContentSizeView.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/9.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCIntrinsicContentSizeView.h"

@implementation BCIntrinsicContentSizeView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.contentSize = frame.size;
    }
    return self;
}

- (CGSize)intrinsicContentSize {
    return self.contentSize;
}

@end
