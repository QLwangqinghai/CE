//
//  CustomButton.m
//  CustomButton
//
//  Created by vector on 2019/10/16.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "CustomButton.h"

@interface CustomButton ()

@property (nonatomic, strong, readonly) UIImageView * imageView;
@property (nonatomic, strong, readonly) UILabel * titleLabel;
@property (nonatomic, strong, readonly) UIView * contentView;

@property (nonatomic, assign) CGSize _intrinsicContentSize;

@property (nonatomic, assign) BOOL isIntrinsicContentSizeInvalid;


@end
@implementation CustomButton

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        CGRect contentFrame = frame;
        contentFrame.origin = CGPointZero;
        
        UIView * contentView = [[UIView alloc] initWithFrame:contentFrame];
        contentView.userInteractionEnabled = false;
        [self addSubview:contentView];
        _contentView = contentView;
        _imageView = [[UIImageView alloc] init];
        _titleLabel = [[UILabel alloc] init];
        _titleLabel.numberOfLines = 1;
    }
    return self;
}

#pragma mark - getter & setter

- (void)setContentDirection:(CustomButtonContentDirection)contentDirection {
    if (_contentDirection != contentDirection) {
        _contentDirection = contentDirection;
        [self _onChanged];
    }
}
- (void)setContentStyle:(CustomButtonStyle)contentStyle {
    if (_contentStyle != contentStyle) {
        _contentStyle = contentStyle;
        [self _onChanged];
    }
}
- (void)setContentAlignment:(CustomButtonAlignment)contentAlignment {
    if (_contentAlignment != contentAlignment) {
        _contentAlignment = contentAlignment;
        [self _onChanged];
    }
}
- (void)setContentInsets:(UIEdgeInsets)contentInsets {
    if (!UIEdgeInsetsEqualToEdgeInsets(_contentInsets, contentInsets)) {
        _contentInsets = contentInsets;
        [self _onChanged];
    }
}
- (void)setTitleInsets:(UIEdgeInsets)titleInsets {
    if (!UIEdgeInsetsEqualToEdgeInsets(_titleInsets, titleInsets)) {
        _titleInsets = titleInsets;
        [self _onChanged];
    }
}
- (void)setImageInsets:(UIEdgeInsets)imageInsets {
    if (!UIEdgeInsetsEqualToEdgeInsets(_imageInsets, imageInsets)) {
        _imageInsets = imageInsets;
        [self _onChanged];
    }
}
- (void)setMinSize:(CGSize)minSize {
    if (!CGSizeEqualToSize(_minSize, minSize)) {
        _minSize = minSize;
        [self _onChanged];
    }
}
- (void)setTitle:(NSString *)title {
    self.titleLabel.text = title;
    if (title && nil == self.titleLabel.superview) {
        [self.contentView addSubview:self.titleLabel];
    } else if (nil == title && nil != self.titleLabel.superview) {
        [self.titleLabel removeFromSuperview];
    }
    [self _onChanged];
}
- (NSString *)title {
    return self.titleLabel.text;
}
- (void)setTitleColor:(UIColor *)titleColor {
    self.titleLabel.textColor = titleColor;
    [self _onChanged];
}
- (UIColor *)titleColor {
    return self.titleLabel.textColor;
}
- (void)setTitleFont:(UIFont *)titleFont {
    self.titleLabel.font = titleFont;
    [self _onChanged];
}
- (UIFont *)titleFont {
    return self.titleLabel.font;
}
- (void)setImage:(UIImage *)image {
    self.imageView.image = image;
    if (image && nil == self.imageView.superview) {
        [self.contentView addSubview:self.imageView];
    } else if (nil == image && nil != self.imageView.superview) {
        [self.imageView removeFromSuperview];
    }
    [self _onChanged];
}
- (UIImage *)image {
    return self.imageView.image;
}
- (void)setLineSpacing:(CGFloat)lineSpacing {
    if (_lineSpacing != lineSpacing) {
        _lineSpacing = lineSpacing;
        [self _onChanged];
    }
}

- (void)_onChanged {
    self.isIntrinsicContentSizeInvalid = true;
    [self invalidateIntrinsicContentSize];
    [self setNeedsLayout];
}

- (void)layoutSubviews {
    [super layoutSubviews];
    [self _layout];
}

- (void)_layout {
    CGRect bounds = self.bounds;
    
    CGSize contentSize = self.bounds.size;
    contentSize.width -= self.contentInsets.left + self.contentInsets.right;
    contentSize.height -= self.contentInsets.top + self.contentInsets.bottom;
    
    CGSize imageSize = self.imageView.intrinsicContentSize;
    CGRect contentFrame = CGRectZero;
    CGRect imageFrame = CGRectZero;
    imageFrame.size = imageSize;
    
    CGRect titleFrame = CGRectZero;
    
    CGSize preferredContentSize = [self _calculationContentSize];
    
    CustomButtonAlignment contentAlignment = self.contentAlignment;
    if (contentAlignment > CustomButtonAlignmentBothSizesBottomOrRight) {//无效数据
        contentAlignment = CustomButtonAlignmentCenter;
    }
    if (contentSize.height < preferredContentSize.height) {
        contentSize.height = preferredContentSize.height;
    }
    if (contentSize.width < imageSize.width) {
        contentSize.width = imageSize.width;
    }
    contentFrame.size = contentSize;

    contentFrame.origin.x = (bounds.size.width - contentSize.width - self.contentInsets.left - self.contentInsets.right) / 2 + self.contentInsets.left;
    contentFrame.origin.y = (bounds.size.height - contentSize.height - self.contentInsets.top - self.contentInsets.bottom) / 2 + self.contentInsets.top;
    self.contentView.frame = contentFrame;

    CGFloat titleMaxWidth = 0;
    if (CustomButtonContentDirectionVertical == self.contentDirection) {
        titleMaxWidth = contentSize.width - self.titleInsets.left - self.titleInsets.right;
    } else {
        titleMaxWidth = contentSize.width - self.imageInsets.left - self.imageInsets.right - imageSize.width - self.titleInsets.left - self.titleInsets.right;
    }
    if (titleMaxWidth < 0) {
        titleMaxWidth = 0;
    }
    CGSize titleSize = self.titleLabel.intrinsicContentSize;
    if (titleSize.width > titleMaxWidth) {
        titleSize.width = titleMaxWidth;
    }
    titleFrame.size = titleSize;

    NSInteger xAlignment = 0;
    NSInteger yAlignment = 0;
    BOOL isBothSides = false;
    switch (contentAlignment) {
        case CustomButtonAlignmentTop: {
            yAlignment = -1;
        }
            break;
        case CustomButtonAlignmentLeft: {
            xAlignment = -1;
        }
            break;
        case CustomButtonAlignmentBottom: {
            yAlignment = 1;
        }
            break;
        case CustomButtonAlignmentRight: {
            xAlignment = 1;
        }
            break;
        case CustomButtonAlignmentTopLeft: {
            xAlignment = -1;
            yAlignment = -1;
        }
            break;
        case CustomButtonAlignmentBottomLeft: {
            xAlignment = -1;
            yAlignment = 1;
        }
            break;
        case CustomButtonAlignmentBottomRight: {
            xAlignment = 1;
            yAlignment = 1;
        }
            break;
        case CustomButtonAlignmentTopRight: {
            xAlignment = 1;
            yAlignment = -1;
        }
            break;
        case CustomButtonAlignmentBothSizes: {
            isBothSides = true;
            
            if (CustomButtonContentDirectionVertical == self.contentDirection) {
                titleFrame.origin.x = (contentSize.width - self.titleInsets.left - self.titleInsets.right - titleFrame.size.width) / 2 + self.titleInsets.left;
                imageFrame.origin.x = (contentSize.width - self.imageInsets.left - self.imageInsets.right - imageFrame.size.width) / 2 + self.imageInsets.left;
                if (CustomButtonStyleTitleAndImage == self.contentStyle) {
                    titleFrame.origin.y = self.titleInsets.top;
                    imageFrame.origin.y = contentSize.height - imageFrame.size.height - self.imageInsets.bottom;
                } else {
                    imageFrame.origin.y = self.imageInsets.top;
                    titleFrame.origin.y = contentSize.height - titleFrame.size.height - self.titleInsets.bottom;
                }
            } else {
                titleFrame.origin.y = (contentSize.height - self.titleInsets.top - self.titleInsets.bottom - titleFrame.size.height) / 2 + self.titleInsets.top;
                imageFrame.origin.y = (contentSize.height - self.imageInsets.top - self.imageInsets.bottom - imageFrame.size.height) / 2 + self.imageInsets.top;
                if (CustomButtonStyleTitleAndImage == self.contentStyle) {
                    titleFrame.origin.x = self.titleInsets.left;
                    imageFrame.origin.x = contentSize.width - imageFrame.size.width - self.imageInsets.right;
                } else {
                    imageFrame.origin.x = self.imageInsets.top;
                    titleFrame.origin.x = contentSize.width - titleFrame.size.width - self.titleInsets.right;
                }
            }
        }
            break;
        case CustomButtonAlignmentBothSizesTopOrLeft: {
            isBothSides = true;

            if (CustomButtonContentDirectionVertical == self.contentDirection) {//上下两边对齐， 内容左对齐
                titleFrame.origin.x = self.titleInsets.left;
                imageFrame.origin.x = self.imageInsets.left;
                if (CustomButtonStyleTitleAndImage == self.contentStyle) {
                    titleFrame.origin.y = self.titleInsets.top;
                    imageFrame.origin.y = contentSize.height - imageFrame.size.height - self.imageInsets.bottom;
                } else {
                    imageFrame.origin.y = self.imageInsets.top;
                    titleFrame.origin.y = contentSize.height - titleFrame.size.height - self.titleInsets.bottom;
                }
            } else {//左右两边对齐， 内容上对齐
                titleFrame.origin.y = self.titleInsets.top;
                imageFrame.origin.y = self.imageInsets.top;
                if (CustomButtonStyleTitleAndImage == self.contentStyle) {
                    titleFrame.origin.x = self.titleInsets.left;
                    imageFrame.origin.x = contentSize.width - imageFrame.size.width - self.imageInsets.right;
                } else {
                    imageFrame.origin.x = self.imageInsets.top;
                    titleFrame.origin.x = contentSize.width - titleFrame.size.width - self.titleInsets.right;
                }
            }
        }
            break;
        case CustomButtonAlignmentBothSizesBottomOrRight: {
            isBothSides = true;

            if (CustomButtonContentDirectionVertical == self.contentDirection) {//上下两边对齐， 内容右对齐
                titleFrame.origin.x = contentFrame.size.width - self.titleInsets.right - titleFrame.size.width;
                imageFrame.origin.x = contentFrame.size.width - self.imageInsets.right - imageFrame.size.width;
                if (CustomButtonStyleTitleAndImage == self.contentStyle) {
                    titleFrame.origin.y = self.titleInsets.top;
                    imageFrame.origin.y = contentSize.height - imageFrame.size.height - self.imageInsets.bottom;
                } else {
                    imageFrame.origin.y = self.imageInsets.top;
                    titleFrame.origin.y = contentSize.height - titleFrame.size.height - self.titleInsets.bottom;
                }
            } else {//左右两边对齐， 内容下对齐
                titleFrame.origin.y = contentFrame.size.height - self.titleInsets.bottom - titleFrame.size.height;
                imageFrame.origin.y = contentFrame.size.height - self.imageInsets.bottom - imageFrame.size.height;

                if (CustomButtonStyleTitleAndImage == self.contentStyle) {
                    titleFrame.origin.x = self.titleInsets.left;
                    imageFrame.origin.x = contentSize.width - imageFrame.size.width - self.imageInsets.right;
                } else {
                    imageFrame.origin.x = self.imageInsets.top;
                    titleFrame.origin.x = contentSize.width - titleFrame.size.width - self.titleInsets.right;
                }
            }
        }
            break;
        default:
            break;
    }
    if (!isBothSides) {
        if (CustomButtonContentDirectionVertical == self.contentDirection) {
            if (xAlignment < 0) {
                titleFrame.origin.x = self.titleInsets.left;
                imageFrame.origin.x = self.imageInsets.left;
            } else if (xAlignment == 0) {
                titleFrame.origin.x = (contentSize.width - self.titleInsets.left - self.titleInsets.right - titleFrame.size.width) / 2 + self.titleInsets.left;
                imageFrame.origin.x = (contentSize.width - self.imageInsets.left - self.imageInsets.right - imageFrame.size.width) / 2 + self.imageInsets.left;
            } else {
                titleFrame.origin.x = contentFrame.size.width - self.titleInsets.right - titleFrame.size.width;
                imageFrame.origin.x = contentFrame.size.width - self.imageInsets.right - imageFrame.size.width;
            }
            
            CGFloat offset = 0;
            if (yAlignment < 0) {
            } else if (yAlignment == 0) {
                offset = (contentFrame.size.height - self.titleInsets.top - self.titleInsets.bottom - self.lineSpacing - titleFrame.size.height - self.imageInsets.top - self.imageInsets.bottom - imageFrame.size.height) / 2;
            } else {
                offset = (contentFrame.size.height - self.titleInsets.top - self.titleInsets.bottom - self.lineSpacing - titleFrame.size.height - self.imageInsets.top - self.imageInsets.bottom - imageFrame.size.height);
            }
            
            if (CustomButtonStyleTitleAndImage == self.contentStyle) {
                titleFrame.origin.y = self.titleInsets.top + offset;
                imageFrame.origin.y = titleFrame.origin.y + titleFrame.size.height + self.titleInsets.bottom + self.lineSpacing + self.imageInsets.top;
            } else {
                imageFrame.origin.y = self.imageInsets.top + offset;
                titleFrame.origin.y = imageFrame.origin.y + imageFrame.size.height + self.imageInsets.bottom + self.lineSpacing + self.titleInsets.top;
            }
        } else {
            if (yAlignment < 0) {
                titleFrame.origin.y = self.titleInsets.top;
                imageFrame.origin.y = self.imageInsets.top;
            } else if (yAlignment == 0) {
                titleFrame.origin.y = (contentSize.height - self.titleInsets.top - self.titleInsets.bottom - titleFrame.size.height) / 2 + self.titleInsets.top;
                imageFrame.origin.y = (contentSize.height - self.imageInsets.top - self.imageInsets.bottom - imageFrame.size.height) / 2 + self.imageInsets.top;
            } else {
                titleFrame.origin.y = contentFrame.size.height - self.titleInsets.bottom - titleFrame.size.height;
                imageFrame.origin.y = contentFrame.size.height - self.imageInsets.bottom - imageFrame.size.height;
            }
            
            CGFloat offset = 0;
            if (xAlignment < 0) {
            } else if (xAlignment == 0) {
                offset = (contentFrame.size.width - self.titleInsets.left - self.titleInsets.right - self.lineSpacing - titleFrame.size.width - self.imageInsets.left - self.imageInsets.right - imageFrame.size.width) / 2;
            } else {
                offset = (contentFrame.size.width - self.titleInsets.left - self.titleInsets.right - self.lineSpacing - titleFrame.size.width - self.imageInsets.left - self.imageInsets.right - imageFrame.size.width);
            }
            
            if (CustomButtonStyleTitleAndImage == self.contentStyle) {
                titleFrame.origin.x = self.titleInsets.left + offset;
                imageFrame.origin.x = titleFrame.origin.x + titleFrame.size.width + self.titleInsets.right + self.lineSpacing + self.imageInsets.left;
            } else {
                imageFrame.origin.x = self.imageInsets.left + offset;
                titleFrame.origin.x = imageFrame.origin.x + imageFrame.size.width + self.imageInsets.right + self.lineSpacing + self.titleInsets.left;
            }
        }
    }
    self.titleLabel.frame = titleFrame;
    self.imageView.frame = imageFrame;
}

- (CGSize)_calculationContentSize {
    CGSize contentSize = CGSizeZero;
    if (nil == self.image && nil == self.title) {
        return contentSize;
    } else if (nil == self.image && nil != self.title) {
        CGSize titleSize = self.titleLabel.intrinsicContentSize;
        titleSize.width += self.titleInsets.left + self.titleInsets.right;
        titleSize.height += self.titleInsets.top + self.titleInsets.bottom;
        contentSize.width += titleSize.width;
        contentSize.height += titleSize.height;
    } else if (nil != self.image && nil == self.title) {
        CGSize imageSize = self.imageView.intrinsicContentSize;
        imageSize.width += self.imageInsets.left + self.imageInsets.right;
        imageSize.height += self.imageInsets.top + self.imageInsets.bottom;
        contentSize.width += imageSize.width;
        contentSize.height += imageSize.height;
    } else {
        CGSize titleSize = self.titleLabel.intrinsicContentSize;
        titleSize.width += self.titleInsets.left + self.titleInsets.right;
        titleSize.height += self.titleInsets.top + self.titleInsets.bottom;
        CGSize imageSize = self.imageView.intrinsicContentSize;
        imageSize.width += self.imageInsets.left + self.imageInsets.right;
        imageSize.height += self.imageInsets.top + self.imageInsets.bottom;

        if (CustomButtonContentDirectionVertical == self.contentDirection) {
            contentSize.width += MAX(titleSize.width, imageSize.width);
            contentSize.height += titleSize.height + imageSize.height + self.lineSpacing;
        } else {
            contentSize.width += titleSize.width + imageSize.width + self.lineSpacing;
            contentSize.height += MAX(titleSize.height, imageSize.height);
        }
    }
    return contentSize;
}

- (CGSize)_calculationIntrinsicContentSize {
    CGSize size = CGSizeZero;
    size.width += self.contentInsets.left + self.contentInsets.right;
    size.height += self.contentInsets.top + self.contentInsets.bottom;

    CGSize contentSize = [self _calculationContentSize];
    size.width += contentSize.width;
    size.height += contentSize.height;
    
    CGSize min = self.minSize;
    if (size.width < min.width) {
        size.width = min.width;
    }
    if (size.height < min.height) {
        size.height = min.height;
    }
    CGFloat maxWidth = self.maxWidth;
    if (maxWidth > 0.1 && maxWidth > min.width) {
        if (size.width > maxWidth) {
            size.width = maxWidth;
        }
    }
    return size;
}

- (void)_updateIntrinsicContentSize {
    self._intrinsicContentSize = [self _calculationIntrinsicContentSize];
}

- (CGSize)intrinsicContentSize {
    if (self.isIntrinsicContentSizeInvalid) {
        [self _updateIntrinsicContentSize];
    }
    return self._intrinsicContentSize;
}

@end
