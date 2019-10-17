//
//  CustomButton.h
//  CustomButton
//
//  Created by vector on 2019/10/16.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN



typedef NS_ENUM(NSUInteger, CustomButtonContentDirection) {
    CustomButtonContentDirectionHorizontal = 0,//水平方向
    CustomButtonContentDirectionVertical = 1,//垂直方向
};

typedef NS_ENUM(NSUInteger, CustomButtonStyle) {
    CustomButtonStyleImageAndTitle = 0,//image title
    CustomButtonStyleTitleAndImage = 1,//title image
};

typedef NS_ENUM(NSUInteger, CustomButtonAlignment) {
    CustomButtonAlignmentCenter = 0x0,
    CustomButtonAlignmentTop,
    CustomButtonAlignmentBottom,
    CustomButtonAlignmentLeft,
    CustomButtonAlignmentRight,
    CustomButtonAlignmentTopLeft,
    CustomButtonAlignmentTopRight,
    CustomButtonAlignmentBottomLeft,
    CustomButtonAlignmentBottomRight,
    
    CustomButtonAlignmentBothSizes,//图片 title 分割， 布局方向上两侧对齐
    CustomButtonAlignmentBothSizesTopOrLeft,//图片 title 分割， 布局方向上两侧对齐， 另一个方向左对齐 或者 上对齐
    CustomButtonAlignmentBothSizesBottomOrRight,//图片 title 分割， 布局方向上两侧对齐， 另一个方向右对齐 或者 下对齐
};


//space
@interface CustomButton : UIControl

@property (nonatomic, assign) CustomButtonContentDirection contentDirection;//内容方向 默认值为水平方向
@property (nonatomic, assign) CustomButtonStyle contentStyle;//内容样式 默认 图片+title
@property (nonatomic, assign) CustomButtonAlignment contentAlignment;//默认中心对齐

@property (nonatomic, assign) UIEdgeInsets contentInsets;
@property (nonatomic, assign) UIEdgeInsets imageInsets;
@property (nonatomic, assign) UIEdgeInsets titleInsets;

@property (nonatomic, copy, nullable) NSString * title;
@property (nonatomic, strong, nullable) UIColor * titleColor;
@property (nonatomic, strong, nullable) UIFont * titleFont;
@property (nonatomic, strong, nullable) UIImage * image;

@property (nonatomic, assign) CGSize minSize;
@property (nonatomic, assign) CGFloat maxWidth;//> 0.1 && > minSize.width时有效

@property (nonatomic, assign) CGFloat lineSpacing;


@end

NS_ASSUME_NONNULL_END
