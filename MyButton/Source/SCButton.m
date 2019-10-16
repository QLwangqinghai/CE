//
//  SCButton.m
//  SC_Lecture
//
//  Created by vector on 2019/7/26.
//  Copyright © 2019 Shituo Han. All rights reserved.
//

#import "SCButton.h"

@interface SCButton ()

@property (nonatomic, weak) UIView * contentView;

@property (nonatomic, weak) UIView * spaceView;

@property (nonatomic, weak) UIImageView * imageView;
@property (nonatomic, weak) UILabel * titleLabel;


@property (nonatomic, assign) CGFloat inset0;
@property (nonatomic, assign) CGFloat inset1;

@end
@implementation SCButton

- (instancetype)initWithFrame:(CGRect)frame style:(SCButtonStyle)style alignment:(SCButtonContentAlignment)alignment contentOffset:(CGPoint)contentOffset contentSpace:(CGFloat)contentSpace imageAlignment:(SCButtonContentAlignment)imageAlignment titleAlignment:(SCButtonContentAlignment)titleAlignment {
    return [self initWithFrame:frame style:style alignment:alignment contentOffset:contentOffset contentSpace:contentSpace inset0:0 inset1:0 imageAlignment:imageAlignment titleAlignment:titleAlignment];
}

- (instancetype)initWithFrame:(CGRect)frame style:(SCButtonStyle)style alignment:(SCButtonContentAlignment)alignment contentOffset:(CGPoint)contentOffset contentSpace:(CGFloat)contentSpace inset0:(CGFloat)inset0 inset1:(CGFloat)inset1 imageAlignment:(SCButtonContentAlignment)imageAlignment titleAlignment:(SCButtonContentAlignment)titleAlignment {
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
            BOOL isVertical = (SCButtonStyleVerticalImageAndTitle == style || SCButtonStyleVerticalTitleAndImage == style);
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
            case SCButtonStyleImageOnly: {
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
            case SCButtonStyleVerticalImageAndTitle://垂直方向 image title
            case SCButtonStyleVerticalTitleAndImage:
            case SCButtonStyleHorizontalImageAndTitle://水平方向 image title
            case SCButtonStyleHorizontalTitleAndImage: {
                BOOL isVertical = (SCButtonStyleVerticalImageAndTitle == style || SCButtonStyleVerticalTitleAndImage == style);
                BOOL imageIsFirst = (SCButtonStyleVerticalImageAndTitle == style || SCButtonStyleHorizontalImageAndTitle == style);

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
            case SCButtonStyleVerticalImageAndTitleBothSides://垂直方向 image title
            case SCButtonStyleVerticalTitleAndImageBothSides:
            case SCButtonStyleHorizontalImageAndTitleBothSides://水平方向 image title
            case SCButtonStyleHorizontalTitleAndImageBothSides: {
                //已经被过滤
            }
                break;
            case SCButtonStyleTitleOnly:
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

@end
