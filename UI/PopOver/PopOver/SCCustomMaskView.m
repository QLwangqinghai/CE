//
//  SCCustomMaskView.m
//  SC_Lecture
//
//  Created by vector on 2019/9/11.
//  Copyright © 2019 Shituo Han. All rights reserved.
//

#import "SCCustomMaskView.h"

@interface SCCustomMaskView ()
{
    CAShapeLayer *_maskShapeLayer;
}

@property (nonatomic, weak) UIImageView * realContentView;

@end

@implementation SCCustomMaskView

- (UIView *)contentView {
    return self.realContentView;
}

- (UIImageView *)backgroundImageView {
    return self.realContentView;
}

- (void)setImage:(UIImage *)image {
    self.realContentView.image = image;
}

- (UIImage *)image {
    return self.realContentView.image;
}

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self makeSubViewsPrivate];
        [self makeSubViews];
    }
    return self;
}

- (void)makeSubViewsPrivate {
    UIImageView * contentView = [[UIImageView alloc] init];
    [self addSubview:contentView];
    contentView.userInteractionEnabled = true;
    contentView.frame = self.bounds;
    self.realContentView = contentView;
    [contentView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self).insets(UIEdgeInsetsZero);
    }];
}
- (void)makeSubViews {
}

- (void)layoutSubviews {
    [super layoutSubviews];
    [self.masker updateMaskWithBounds:self.bounds handler:^(CGPathRef path) {
        [self setMaskWithCGPath:path];
        self.layer.shadowPath = path;
    }];
}

- (void)setMasker:(id<SCCustomMaskViewMaskerProtocol>)masker {
    if (_masker != masker) {
        _masker = masker;
        [self setNeedsUpdateMask];
    }
}

- (void)setNeedsUpdateMask {
    [self setNeedsLayout];
}

- (void)setMaskWithCGPath:(CGPathRef)path{
    if (!_maskShapeLayer) {
        //蒙版
        CAShapeLayer *maskLayer = [CAShapeLayer layer];
        maskLayer.path = path;
        maskLayer.fillColor = [[UIColor whiteColor] CGColor];
        maskLayer.frame = self.bounds;
        self.contentView.layer.mask = maskLayer;
        _maskShapeLayer = maskLayer;
    } else {
        _maskShapeLayer.path = path;
        _maskShapeLayer.frame = self.bounds;
    }
}

- (void)refreshMask {
    [self.masker updateMaskWithBounds:self.bounds handler:^(CGPathRef path) {
        [self setMaskWithCGPath:path];
        self.layer.shadowPath = path;
    }];
}

@end
