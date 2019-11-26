//
//  BCImageUtil.h
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSUInteger, BCQrImageCorrectionLevel) {
    BCQrImageCorrectionLevelLow,
    BCQrImageCorrectionLevelMiddle,
    BCQrImageCorrectionLevelQ,//Q 不知道是啥的缩写
    BCQrImageCorrectionLevelHigh,
};


/**
 图片加载工具
 */
@interface BCImageUtil : NSObject

+ (UIImage *)imageNamed:(NSString *)name;

+ (UIImage *)imageWithData:(NSData *)data;

+ (UIImage *)imageWithData:(NSData *)data scale:(CGFloat)scale;

+ (UIImage *)imageWithContentsOfFile:(NSString *)filePath;

+ (UIImage *)imageNamed:(NSString *)name inBundle:(NSBundle *)bundle;

+ (UIImage *)imageWithColor:(UIColor *)color;

+ (UIImage *)imageWithColor:(UIColor *)color size:(CGSize)size;

+ (UIImage *)placeholderImage;

+ (UIImage *)walletLogoPlaceholderImage;

+ (UIImage *)rescaleImage:(UIImage *)image toSize:(CGSize)size scale:(CGFloat)scale;
+ (UIImage *)rescaleImage:(UIImage *)image toSize:(CGSize)size;


//二维码生成
+ (UIImage *)qrImageWithMessage:(NSString *)str size:(CGFloat)size;


/**
 生成二维码

 @param data 二维码内容
 @param level 容错率
 @param size 图片大小
 @return 二维码图片
 */
+ (UIImage *)qrImageWithData:(NSData *)data leve:(BCQrImageCorrectionLevel)level size:(CGFloat)size;


@end









