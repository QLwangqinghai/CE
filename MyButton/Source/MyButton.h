//
//  MyButton.h
//  MyButton
//
//  Created by vector on 2019/10/16.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN



typedef NS_ENUM(NSUInteger, MyButtonContentDirection) {
    MyButtonContentDirectionHorizontal = 0,//水平方向
    MyButtonContentDirectionVertical = 1,//垂直方向
};

typedef NS_ENUM(NSUInteger, MyButtonStyle) {
    MyButtonStyleImageAndTitle = 0,//image title
    MyButtonStyleTitleAndImage = 1,//title image
};

//center 0 top/left 1 right/bottom 2
//左右 关系 [0, 4)
//上下 关系 [4, 8)
typedef NS_ENUM(NSUInteger, MyButtonAlignment) {
    MyButtonAlignmentCenter = 0x0,
    MyButtonAlignmentTop,
    MyButtonAlignmentBottom,
    MyButtonAlignmentLeft,
    MyButtonAlignmentRight,
    MyButtonAlignmentTopLeft,
    MyButtonAlignmentTopRight,
    MyButtonAlignmentBottomLeft,
    MyButtonAlignmentBottomRight,
    
    MyButtonAlignmentBothSizes,
    MyButtonAlignmentBothSizesTopOrLeft,
    MyButtonAlignmentBothSizesBottomOrRight,
};

typedef NS_ENUM(NSUInteger, MyButtonItemAlignment) {
    MyButtonItemAlignmentCenter = 0x0,
    MyButtonItemAlignmentTopOrLeft,
    MyButtonItemAlignmentBottomOrRight,
};


//space
@interface MyButton : UIControl

@property (nonatomic, assign) MyButtonContentDirection contentDirection;//内容方向 默认值为水平方向
@property (nonatomic, assign) MyButtonStyle contentStyle;//内容样式 默认 图片+title
@property (nonatomic, assign) MyButtonAlignment contentAlignment;//默认中心对齐
@property (nonatomic, assign) MyButtonItemAlignment imageAlignment;//默认中心对齐
@property (nonatomic, assign) MyButtonItemAlignment titleAlignment;//默认中心对齐

@property (nonatomic, assign) UIEdgeInsets contentInsets;
@property (nonatomic, assign) UIEdgeInsets imageInsets;
@property (nonatomic, assign) UIEdgeInsets titleInsets;

@property (nonatomic, copy, nullable) NSString * title;
@property (nonatomic, copy, nullable) UIColor * titleColor;
@property (nonatomic, copy, nullable) UIFont * titleFont;
@property (nonatomic, copy, nullable) UIImage * image;


/**
 图层结构
 
 Button -> content -> [image title]
 style 描述的是content内容
 offset 描述的是content 的偏移
 alignment 描述的是content在button中的布局方式
 imageAlignment 描述的是image在content中的布局方式
 titleAlignment 描述的是title在content中的布局方式
 
 imageAlignment、titleAlignment 有 MyButtonAlignmentTop, MyButtonAlignmentBottom, MyButtonAlignmentLeft, MyButtonAlignmentRight 4个取值， style为垂直方向时MyButtonAlignmentLeft, MyButtonAlignmentRight 有效， style为水平方向时MyButtonAlignmentTop, MyButtonAlignmentBottom 有效

 备注：宽高不会自适应
 
 */



@end

NS_ASSUME_NONNULL_END
