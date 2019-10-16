//
//  SCButton.h
//  SC_Lecture
//
//  Created by vector on 2019/7/26.
//  Copyright © 2019 Shituo Han. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

//值不要随便改， 有特殊含义
typedef NS_ENUM(uint64_t, SCButtonStyle) {
    SCButtonStyleTitleOnly = 0,//default
    SCButtonStyleImageOnly = 1,
    SCButtonStyleVerticalImageAndTitle = 0x10,//垂直方向 image title
    SCButtonStyleVerticalTitleAndImage = 0x11,//垂直方向 title image
    SCButtonStyleHorizontalImageAndTitle = 0x12,//水平方向 image title
    SCButtonStyleHorizontalTitleAndImage = 0x13,//水平方向 title image
    
    SCButtonStyleVerticalImageAndTitleBothSides = 0x80000010,//垂直方向 两边对齐 image title
    SCButtonStyleVerticalTitleAndImageBothSides = 0x80000011,//垂直方向 两边对齐 title image
    SCButtonStyleHorizontalImageAndTitleBothSides = 0x80000012,//水平方向 两边对齐 image title
    SCButtonStyleHorizontalTitleAndImageBothSides = 0x80000013,//水平方向 两边对齐 title image
};

//center 0 top/left 1 right/bottom 2
//左右 关系 [0, 4)
//上下 关系 [4, 8)
typedef NS_ENUM(NSUInteger, SCButtonContentAlignment) {
    SCButtonContentAlignmentCenter = 0x0,
    SCButtonContentAlignmentTop = 0x10,
    SCButtonContentAlignmentBottom = 0x20,
    SCButtonContentAlignmentLeft = 0x1,
    SCButtonContentAlignmentRight = 0x2,
    SCButtonContentAlignmentTopLeft = 0x11,
    SCButtonContentAlignmentTopRight = 0x12,
    SCButtonContentAlignmentBottomLeft = 0x21,
    SCButtonContentAlignmentBottomRight = 0x22,
    
//    SCButtonContentAlignmentLeftRight = 0x100,//左右对齐
//    SCButtonContentAlignmentTopBottom = 0x200,//上下对齐
};


//space
@interface SCButton : UIControl

@property (nonatomic, weak, readonly) UIView * contentView;
@property (nonatomic, weak, readonly, nullable) UIImageView * imageView;
@property (nonatomic, weak, readonly, nullable) UILabel * titleLabel;

@property (nonatomic, assign, readonly) SCButtonStyle style;
@property (nonatomic, assign, readonly) SCButtonContentAlignment alignment;
@property (nonatomic, assign, readonly) SCButtonContentAlignment imageAlignment;
@property (nonatomic, assign, readonly) SCButtonContentAlignment titleAlignment;
@property (nonatomic, assign, readonly) CGPoint contentOffset;
@property (nonatomic, assign, readonly) CGFloat contentSpace;

@property (nonatomic, assign, readonly) CGFloat inset0;
@property (nonatomic, assign, readonly) CGFloat inset1;


/**
 图层结构
 
 Button -> content -> [image title]
 style 描述的是content内容
 offset 描述的是content 的偏移
 alignment 描述的是content在button中的布局方式
 imageAlignment 描述的是image在content中的布局方式
 titleAlignment 描述的是title在content中的布局方式
 
 imageAlignment、titleAlignment 有 SCButtonContentAlignmentTop, SCButtonContentAlignmentBottom, SCButtonContentAlignmentLeft, SCButtonContentAlignmentRight 4个取值， style为垂直方向时SCButtonContentAlignmentLeft, SCButtonContentAlignmentRight 有效， style为水平方向时SCButtonContentAlignmentTop, SCButtonContentAlignmentBottom 有效

 备注：宽高不会自适应
 
 */
- (instancetype)initWithFrame:(CGRect)frame style:(SCButtonStyle)style alignment:(SCButtonContentAlignment)alignment contentOffset:(CGPoint)contentOffset contentSpace:(CGFloat)contentSpace imageAlignment:(SCButtonContentAlignment)imageAlignment titleAlignment:(SCButtonContentAlignment)titleAlignment;

- (instancetype)initWithFrame:(CGRect)frame style:(SCButtonStyle)style alignment:(SCButtonContentAlignment)alignment contentOffset:(CGPoint)contentOffset contentSpace:(CGFloat)contentSpace inset0:(CGFloat)inset0 inset1:(CGFloat)inset1 imageAlignment:(SCButtonContentAlignment)imageAlignment titleAlignment:(SCButtonContentAlignment)titleAlignment;

@end

NS_ASSUME_NONNULL_END
