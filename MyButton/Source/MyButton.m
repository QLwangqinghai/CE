//
//  MyButton.m
//  MyButton
//
//  Created by vector on 2019/10/16.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "MyButton.h"

@interface MyButton ()

@property (nonatomic, strong, readonly) UIImageView * imageView;
@property (nonatomic, strong, readonly) UILabel * titleLabel;
@property (nonatomic, strong, readonly) UIView * contentView;


@property (nonatomic, assign) CGFloat inset0;
@property (nonatomic, assign) CGFloat inset1;

@property (nonatomic, assign) CGSize _intrinsicContentSize;

@end
@implementation MyButton

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
    }
    return self;
}


- (instancetype)initWithFrame:(CGRect)frame style:(MyButtonStyle)style alignment:(MyButtonAlignment)alignment contentOffset:(CGPoint)contentOffset contentSpace:(CGFloat)contentSpace inset0:(CGFloat)inset0 inset1:(CGFloat)inset1 imageAlignment:(MyButtonAlignment)imageAlignment titleAlignment:(MyButtonAlignment)titleAlignment {
    self = [super initWithFrame:frame];
    if (self) {
        _style = style;
        _alignment = alignment;
        _imageAlignment = imageAlignment;
        _titleAlignment = titleAlignment;
        _contentOffset = contentOffset;
        _contentSpace = contentSpace;
        _inset0 = inset0;
        _inset1 = inset1;
        
        BOOL bothSides = ((0x80000000UL & style) == 0x80000000UL);
        style = (style & (~0x80000000UL));
        UIView * contentView = [[UIView alloc] init];
        contentView.userInteractionEnabled = false;
        [self addSubview:contentView];
        self.contentView = contentView;
        
        if (bothSides) {
            BOOL isVertical = (MyButtonStyleVerticalImageAndTitle == style || MyButtonStyleVerticalTitleAndImage == style);
            if (isVertical) {//垂直方向
                [contentView mas_makeConstraints:^(MASConstraintMaker *make) {
                    make.top.equalTo(self.mas_top).offset(inset0);
                    make.bottom.equalTo(self.mas_bottom).offset(-inset1);
                    make.centerX.equalTo(self.mas_centerX).offset(contentOffset.x);
                }];
            } else {
                [contentView mas_makeConstraints:^(MASConstraintMaker *make) {
                    make.left.equalTo(self.mas_left).offset(inset0);
                    make.right.equalTo(self.mas_right).offset(-inset1);
                    make.centerY.equalTo(self.mas_centerY).offset(contentOffset.y);
                }];
            }
        } else {
            uint32_t lr = alignment & 0xf;
            uint32_t tb = alignment & 0xf0;
            
            [contentView mas_makeConstraints:^(MASConstraintMaker *make) {
                if (1 == lr) {
                    //左对齐
                    make.left.equalTo(self.mas_left).offset(contentOffset.x);
                } else if (2 == lr) {
                    //右对齐
                    make.right.equalTo(self.mas_right).offset(contentOffset.x);
                } else {
                    make.centerX.equalTo(self.mas_centerX).offset(contentOffset.x);
                }
                if (0x10 == tb) {
                    //上对齐
                    make.top.equalTo(self.mas_top).offset(contentOffset.y);
                } else if (0x20 == tb) {
                    //下对齐
                    make.bottom.equalTo(self.mas_bottom).offset(contentOffset.y);
                } else {
                    make.centerY.equalTo(self.mas_centerY).offset(contentOffset.y);
                }
            }];
        }
        
        switch (style) {
            case MyButtonStyleImageOnly: {
                UIImageView * imageView = [[UIImageView alloc] init];
                [contentView addSubview:imageView];
                self.imageView = imageView;
                
                uint32_t lr = imageAlignment & 0xf;
                uint32_t tb = imageAlignment & 0xf0;
                [imageView mas_makeConstraints:^(MASConstraintMaker *make) {
                    if (1 == lr) {
                        //左对齐
                        make.left.equalTo(contentView.mas_left);
                    } else if (2 == lr) {
                        //右对齐
                        make.right.equalTo(contentView.mas_right);
                    } else {
                        make.centerX.equalTo(contentView.mas_centerX);
                    }
                    if (0x10 == tb) {
                        //上对齐
                        make.top.equalTo(contentView.mas_top);
                    } else if (0x20 == tb) {
                        //下对齐
                        make.bottom.equalTo(contentView.mas_bottom);
                    } else {
                        make.centerY.equalTo(contentView.mas_centerY);
                    }
                    make.width.equalTo(contentView.mas_width);
                    make.height.equalTo(contentView.mas_height);
                }];
            }
                break;
            case MyButtonStyleVerticalImageAndTitle://垂直方向 image title
            case MyButtonStyleVerticalTitleAndImage:
            case MyButtonStyleHorizontalImageAndTitle://水平方向 image title
            case MyButtonStyleHorizontalTitleAndImage: {
                BOOL isVertical = (MyButtonStyleVerticalImageAndTitle == style || MyButtonStyleVerticalTitleAndImage == style);
                BOOL imageIsFirst = (MyButtonStyleVerticalImageAndTitle == style || MyButtonStyleHorizontalImageAndTitle == style);

                UIImageView * imageView = [[UIImageView alloc] init];
                UILabel * titleLabel = [[UILabel alloc] init];
                UIView * spaceView = [[UIView alloc] init];
                if (imageIsFirst) {
                    [contentView addSubview:imageView];
                    [contentView addSubview:spaceView];
                    [contentView addSubview:titleLabel];
                } else {
                    [contentView addSubview:titleLabel];
                    [contentView addSubview:spaceView];
                    [contentView addSubview:imageView];
                }
                self.titleLabel = titleLabel;
                self.imageView = imageView;

                //垂直方向
                if (isVertical) {
                    if (imageIsFirst) {
                        {
                            uint32_t lr = imageAlignment & 0xf;
                            [imageView mas_makeConstraints:^(MASConstraintMaker *make) {
                                if (1 == lr) {
                                    //左对齐
                                    make.left.equalTo(contentView.mas_left);
                                } else if (2 == lr) {
                                    //右对齐
                                    make.right.equalTo(contentView.mas_right);
                                } else {
                                    make.centerX.equalTo(contentView.mas_centerX);
                                }
                                make.top.equalTo(contentView.mas_top);
                                make.width.lessThanOrEqualTo(contentView.mas_width);
                            }];
                        }
                        [spaceView mas_makeConstraints:^(MASConstraintMaker *make) {
                            make.left.equalTo(contentView.mas_left);
                            make.right.equalTo(contentView.mas_right);
                            make.top.equalTo(imageView.mas_bottom);
                            if (!bothSides) {
                                make.height.equalTo(@(contentSpace));
                            }
                        }];
                        {
                            uint32_t lr = titleAlignment & 0xf;
                            [titleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
                                if (1 == lr) {
                                    //左对齐
                                    make.left.equalTo(contentView.mas_left);
                                } else if (2 == lr) {
                                    //右对齐
                                    make.right.equalTo(contentView.mas_right);
                                } else {
                                    make.centerX.equalTo(contentView.mas_centerX);
                                }
                                make.top.equalTo(spaceView.mas_bottom);
                                make.bottom.equalTo(contentView.mas_bottom);
                                make.width.lessThanOrEqualTo(contentView.mas_width);
                            }];
                        }
                    } else {
                        {
                            uint32_t lr = titleAlignment & 0xf;
                            [titleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
                                if (1 == lr) {
                                    //左对齐
                                    make.left.equalTo(contentView.mas_left);
                                } else if (2 == lr) {
                                    //右对齐
                                    make.right.equalTo(contentView.mas_right);
                                } else {
                                    make.centerX.equalTo(contentView.mas_centerX);
                                }
                                make.top.equalTo(contentView.mas_top);
                                make.width.lessThanOrEqualTo(contentView.mas_width);
                            }];
                        }
                        
                        [spaceView mas_makeConstraints:^(MASConstraintMaker *make) {
                            make.left.equalTo(contentView.mas_left);
                            make.right.equalTo(contentView.mas_right);
                            make.top.equalTo(titleLabel.mas_bottom);
                            if (!bothSides) {
                                make.height.equalTo(@(contentSpace));
                            }
                        }];
                        
                        {
                            uint32_t lr = imageAlignment & 0xf;
                            [imageView mas_makeConstraints:^(MASConstraintMaker *make) {
                                if (1 == lr) {
                                    //左对齐
                                    make.left.equalTo(contentView.mas_left);
                                } else if (2 == lr) {
                                    //右对齐
                                    make.right.equalTo(contentView.mas_right);
                                } else {
                                    make.centerX.equalTo(contentView.mas_centerX);
                                }
                                make.top.equalTo(spaceView.mas_bottom);
                                make.bottom.equalTo(contentView.mas_bottom);
                                make.width.lessThanOrEqualTo(contentView.mas_width);
                            }];
                        }
                    }
                } else {
                    if (imageIsFirst) {
                        {
                            uint32_t tb = imageAlignment & 0xf0;
                            [imageView mas_makeConstraints:^(MASConstraintMaker *make) {
                                if (0x10 == tb) {
                                    //上对齐
                                    make.top.equalTo(contentView.mas_top);
                                } else if (0x20 == tb) {
                                    //下对齐
                                    make.bottom.equalTo(contentView.mas_bottom);
                                } else {
                                    make.centerY.equalTo(contentView.mas_centerY);
                                }
                                make.left.equalTo(contentView.mas_left);
                                make.height.lessThanOrEqualTo(contentView.mas_height);
                            }];
                        }
                        [spaceView mas_makeConstraints:^(MASConstraintMaker *make) {
                            make.top.equalTo(contentView.mas_top);
                            make.bottom.equalTo(contentView.mas_bottom);
                            make.left.equalTo(imageView.mas_right);
                            make.width.equalTo(@(contentSpace));
                        }];
                        {
                            uint32_t tb = titleAlignment & 0xf0;
                            [titleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
                                if (0x10 == tb) {
                                    //上对齐
                                    make.top.equalTo(contentView.mas_top);
                                } else if (0x20 == tb) {
                                    //下对齐
                                    make.bottom.equalTo(contentView.mas_bottom);
                                } else {
                                    make.centerY.equalTo(contentView.mas_centerY);
                                }
                                make.left.equalTo(spaceView.mas_right);
                                make.right.equalTo(contentView.mas_right);
                                make.height.lessThanOrEqualTo(contentView.mas_height);
                            }];
                        }
                    } else {
                        {
                            uint32_t tb = titleAlignment & 0xf0;
                            [titleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
                                if (0x10 == tb) {
                                    //上对齐
                                    make.top.equalTo(contentView.mas_top);
                                } else if (0x20 == tb) {
                                    //下对齐
                                    make.bottom.equalTo(contentView.mas_bottom);
                                } else {
                                    make.centerY.equalTo(contentView.mas_centerY);
                                }
                                make.left.equalTo(contentView.mas_left);
                                make.height.lessThanOrEqualTo(contentView.mas_height);
                            }];
                        }

                        [spaceView mas_makeConstraints:^(MASConstraintMaker *make) {
                            make.top.equalTo(contentView.mas_top);
                            make.bottom.equalTo(contentView.mas_bottom);
                            make.left.equalTo(titleLabel.mas_right);
                            if (!bothSides) {
                                make.width.equalTo(@(contentSpace));
                            }
                        }];
                        {
                            uint32_t tb = imageAlignment & 0xf0;
                            [imageView mas_makeConstraints:^(MASConstraintMaker *make) {
                                if (0x10 == tb) {
                                    //上对齐
                                    make.top.equalTo(contentView.mas_top);
                                } else if (0x20 == tb) {
                                    //下对齐
                                    make.bottom.equalTo(contentView.mas_bottom);
                                } else {
                                    make.centerY.equalTo(contentView.mas_centerY);
                                }
                                make.left.equalTo(spaceView.mas_right);
                                make.right.equalTo(contentView.mas_right);
                                make.height.lessThanOrEqualTo(contentView.mas_height);
                            }];
                        }
                    }
                }
            }
                break;
            case MyButtonStyleVerticalImageAndTitleBothSides://垂直方向 image title
            case MyButtonStyleVerticalTitleAndImageBothSides:
            case MyButtonStyleHorizontalImageAndTitleBothSides://水平方向 image title
            case MyButtonStyleHorizontalTitleAndImageBothSides: {
                //已经被过滤
            }
                break;
            case MyButtonStyleTitleOnly:
            default: {
                UILabel * titleLabel = [[UILabel alloc] init];
                [contentView addSubview:titleLabel];
                self.titleLabel = titleLabel;
                
                uint32_t lr = titleAlignment & 0xf;
                uint32_t tb = titleAlignment & 0xf0;
                [titleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
                    if (1 == lr) {
                        //左对齐
                        make.left.equalTo(contentView.mas_left);
                    } else if (2 == lr) {
                        //右对齐
                        make.right.equalTo(contentView.mas_right);
                    } else {
                        make.centerX.equalTo(contentView.mas_centerX);
                    }
                    if (0x10 == tb) {
                        //上对齐
                        make.top.equalTo(contentView.mas_top);
                    } else if (0x20 == tb) {
                        //下对齐
                        make.bottom.equalTo(contentView.mas_bottom);
                    } else {
                        make.centerY.equalTo(contentView.mas_centerY);
                    }
                    make.width.equalTo(contentView.mas_width);
                    make.height.equalTo(contentView.mas_height);
                }];
                
            }
                break;
        }
    }
    return self;
}

#pragma mark - getter & setter

- (void)setContentDirection:(MyButtonContentDirection)contentDirection {
    if (_contentDirection != contentDirection) {
        _contentDirection = contentDirection;
        [self _onChanged];
    }
}
- (void)setContentStyle:(MyButtonStyle)contentStyle {
    if (_contentStyle != contentStyle) {
        _contentStyle = contentStyle;
        [self _onChanged];
    }
}
- (void)setContentAlignment:(MyButtonAlignment)contentAlignment {
    if (_contentAlignment != contentAlignment) {
        _contentAlignment = contentAlignment;
        [self _onChanged];
    }
}
- (void)setImageAlignment:(MyButtonItemAlignment)imageAlignment {
    if (_imageAlignment != imageAlignment) {
        _imageAlignment = imageAlignment;
        [self _onChanged];
    }
}

- (void)setTitleAlignment:(MyButtonItemAlignment)titleAlignment {
    if (_titleAlignment != titleAlignment) {
        _titleAlignment = titleAlignment;
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

- (void)set_intrinsicContentSize:(CGSize)_intrinsicContentSize {
    if (!CGSizeEqualToSize(__intrinsicContentSize, _intrinsicContentSize)) {
        __intrinsicContentSize = _intrinsicContentSize;
        [self invalidateIntrinsicContentSize];
    }
}

- (void)_onChanged {
    [self _updateIntrinsicContentSize];
    [self setNeedsLayout];
    [self.superview setNeedsLayout];
}

- (void)_layout {
    CGRect bounds = self.bounds;
    
    CGSize size = self.bounds.size;
    size.width -= self.contentInsets.left + self.contentInsets.right;
    size.height -= self.contentInsets.top + self.contentInsets.bottom;
    
    
    
    CGRect contentFrame = CGRectZero;
    CGRect imageFrame = CGRectZero;
    CGRect titleFrame = CGRectZero;
    
    CGSize contentSize = CGSizeZero;
    if (nil == self.image && nil == self.title) {
        //do nothing
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

        if (MyButtonContentDirectionVertical == self.contentDirection) {
            contentSize.width += MAX(titleSize.width, imageSize.width);
            contentSize.height += titleSize.height + imageSize.height;
        } else {
            contentSize.width += titleSize.width + imageSize.width;
            contentSize.height += MAX(titleSize.height, imageSize.height);
        }
    }
    
    MyButtonAlignment contentAlignment = self.contentAlignment;
    if (contentAlignment > MyButtonAlignmentBothSizesBottomOrRight) {//无效数据
        contentAlignment = MyButtonAlignmentCenter;
    }
    
    
    BOOL isBothSides = ((contentAlignment == MyButtonAlignmentBothSizes) || (contentAlignment == MyButtonAlignmentBothSizesTopOrLeft) || (contentAlignment == MyButtonAlignmentBothSizesBottomOrRight));
    
    if (isBothSides) {
        if (MyButtonContentDirectionVertical == self.contentDirection) {
            if (contentSize.height > size.height) {
                contentSize.height = size.height;
            }
        } else {
            if (contentSize.width < size.width) {
                contentSize.width = size.width;
            }
        }
    } else {
        if (contentSize.width < size.width) {
            contentSize.width = size.width;
        }
        if (contentSize.height > size.height) {
            contentSize.height = size.height;
        }
    }
    
    contentFrame.size = contentSize;
    
//    MyButtonAlignmentCenter = 0x0,
//    MyButtonAlignmentTop,
//    MyButtonAlignmentBottom,
//    MyButtonAlignmentLeft,
//    MyButtonAlignmentRight,
//    MyButtonAlignmentTopLeft,
//    MyButtonAlignmentTopRight,
//    MyButtonAlignmentBottomLeft,
//    MyButtonAlignmentBottomRight,
//
//    MyButtonAlignmentBothSizes,
//    MyButtonAlignmentBothSizesTopOrLeft,
//    MyButtonAlignmentBothSizesBottomOrRight,
    
    switch (contentAlignment) {
        case MyButtonAlignmentLeft:
        case MyButtonAlignmentTopLeft:
        case MyButtonAlignmentBottomLeft: {
            contentFrame.origin.x = self.contentInsets.left;
        }
            break;
        case MyButtonAlignmentRight:
        case MyButtonAlignmentTopRight:
        case MyButtonAlignmentBottomRight: {
            contentFrame.origin.x = bounds.size.width - self.contentInsets.right - contentSize.width;
        }
            break;
        case MyButtonAlignmentBothSizesTopOrLeft: {
            if (MyButtonContentDirectionVertical == self.contentDirection) {
                contentFrame.origin.x = self.contentInsets.left;
            } else {
                
            }
        }
            break;
        case MyButtonAlignmentBothSizesBottomOrRight: {
            if (MyButtonContentDirectionVertical == self.contentDirection) {
                contentFrame.origin.x = bounds.size.width - self.contentInsets.right - contentSize.width;
            } else {
                
            }
        }
            break;
        default: {
            contentFrame.origin.x = (bounds.size.width - contentSize.width) / 2;
        }
            break;
    }
    
    switch (contentAlignment) {
        case MyButtonAlignmentTop:
        case MyButtonAlignmentTopLeft:
        case MyButtonAlignmentTopRight: {
            contentFrame.origin.y = self.contentInsets.top;
        }
            break;
        case MyButtonAlignmentBottom:
        case MyButtonAlignmentBottomLeft:
        case MyButtonAlignmentBottomRight: {
            contentFrame.origin.y = bounds.size.height - self.contentInsets.bottom - contentSize.height;
        }
            break;
        case MyButtonAlignmentBothSizesTopOrLeft: {
            if (MyButtonContentDirectionVertical == self.contentDirection) {
            } else {
                contentFrame.origin.y = self.contentInsets.top;
            }
        }
            break;
        case MyButtonAlignmentBothSizesBottomOrRight: {
            if (MyButtonContentDirectionVertical == self.contentDirection) {
            } else {
                contentFrame.origin.y = bounds.size.height - self.contentInsets.bottom - contentSize.height;
            }
        }
            break;
        default: {
            contentFrame.origin.y = (bounds.size.height - contentSize.height) / 2;
        }
            break;
    }
    
    self.contentView.frame = contentFrame;
    
    /*
     typedef NS_ENUM(NSUInteger, MyButtonContentDirection) {
         MyButtonContentDirectionHorizontal = 0,//水平方向
         MyButtonContentDirectionVertical = 1,//垂直方向
     };

     typedef NS_ENUM(NSUInteger, MyButtonStyle) {
         MyButtonStyleImageAndTitle = 0,//image title
         MyButtonStyleTitleAndImage = 1,//title image
     };
     */
    
    if (MyButtonContentDirectionVertical == self.contentDirection) {
        if (MyButtonStyleTitleAndImage == self.contentStyle) {
            CGSize titleSize = self.titleLabel.intrinsicContentSize;
            titleFrame.size.height = titleSize.height;
            
            CGFloat titleSizeMaxWidth = contentFrame.size.width - self.titleInsets.left - self.titleInsets.right;
            if (titleSize.width > titleSizeMaxWidth) {
                titleSize.width = titleSizeMaxWidth;
            }
            titleFrame.size.width = titleSize.width;

            if (MyButtonItemAlignmentTopOrLeft == self.titleAlignment) {
                titleFrame.origin.x = self.titleInsets.left;
            } else if (MyButtonItemAlignmentBottomOrRight == self.titleAlignment) {
                titleFrame.origin.x = contentFrame.size.width - self.titleInsets.left - titleSize.width;
            } else {
                titleFrame.origin.x = self.titleInsets.left;
            }
            
            
            titleFrame.origin.y = self.titleInsets.top;

            imageFrame.origin.y = titleFrame.origin.y + titleFrame.size.height + self.titleInsets.top + self.titleInsets.bottom + self.imageInsets.top;
            imageFrame.size = self.imageView.intrinsicContentSize;
            imageFrame.origin.x =
            
        } else {

        }
    } else {
        if (MyButtonStyleTitleAndImage == self.contentStyle) {
            
        } else {

        }
    }
    
    
    
}

- (void)_updateIntrinsicContentSize {
    CGSize size = CGSizeZero;
    size.width += self.contentInsets.left + self.contentInsets.right;
    size.height += self.contentInsets.top + self.contentInsets.bottom;

    CGSize contentSize = CGSizeZero;

    if (nil == self.image && nil == self.title) {
        return size;
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

        if (MyButtonContentDirectionVertical == self.contentDirection) {
            contentSize.width += MAX(titleSize.width, imageSize.width);
            contentSize.height += titleSize.height + imageSize.height;
        } else {
            contentSize.width += titleSize.width + imageSize.width;
            contentSize.height += MAX(titleSize.height, imageSize.height);
        }
    }
    size.width += contentSize.width;
    size.height += contentSize.height;
    self._intrinsicContentSize = size;
}

- (CGSize)intrinsicContentSize {
    return self._intrinsicContentSize;
}

@end
