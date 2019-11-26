//
//  BCImageUtil.m
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCImageUtil.h"

@implementation BCImageUtil

+ (UIImage *)imageNamed:(NSString *)name {
    if (nil == name) {
        return nil;
    }
    return [UIImage imageNamed:name];
}

+ (UIImage *)imageWithData:(NSData *)data {
    if (nil == data) {
        return nil;
    }
    return [UIImage imageWithData:data];
}

+ (UIImage *)imageWithData:(NSData *)data scale:(CGFloat)scale {
    if (nil == data) {
        return nil;
    }
    return [UIImage imageWithData:data scale:scale];
}

+ (UIImage *)imageWithContentsOfFile:(NSString *)filePath {
    if (nil == filePath) {
        return nil;
    }
    return [UIImage imageWithContentsOfFile:filePath];
}

+ (UIImage *)imageNamed:(NSString *)name inBundle:(NSBundle *)bundle {
    if (nil == name) {
        return nil;
    }
    return [UIImage imageNamed:name inBundle:bundle compatibleWithTraitCollection:nil];
}

+ (UIImage *)imageWithColor:(UIColor *)color {
    CGRect rect = CGRectMake(0.0f, 0.0f, 1.0f, 1.0f);
    UIGraphicsBeginImageContext(rect.size);
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetFillColorWithColor(context, [color CGColor]);
    CGContextFillRect(context, rect);
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
//    CGContextRelease(context);
    return image;
}

+ (UIImage *)imageWithColor:(UIColor *)color size:(CGSize)size {
    CGRect rect = CGRectMake(0.0f, 0.0f, size.width, size.height);
    UIGraphicsBeginImageContext(rect.size);
    
    CGContextRef context =UIGraphicsGetCurrentContext();
    CGContextSetFillColorWithColor(context, [color CGColor]);
    CGContextFillRect(context, rect);
    
    UIImage *image =UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return image;
}

+ (UIImage *)qrImageWithMessage:(NSString *)message size:(CGFloat)size {
    return [self qrImageWithData:[message dataUsingEncoding:NSUTF8StringEncoding] leve:(BCQrImageCorrectionLevelMiddle) size:size];
}

+ (UIImage *)rescaleImage:(UIImage *)image toSize:(CGSize)size {
    return [self rescaleImage:image toSize:size scale:[[UIScreen mainScreen] scale]];
}

+ (UIImage *)rescaleImage:(UIImage *)image toSize:(CGSize)size scale:(CGFloat)scale {
    CGRect rect = CGRectMake(0.0, 0.0, size.width * scale, size.height * scale);
    UIGraphicsBeginImageContext(rect.size);
    [image drawInRect:rect];  // scales image to rect
    UIImage *resImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return resImage;
}

+ (UIImage *)qrImageWithData:(NSData *)data leve:(BCQrImageCorrectionLevel)level size:(CGFloat)size {
    //1. 实例化二维码滤镜
    CIFilter *filter = [CIFilter filterWithName:@"CIQRCodeGenerator"];
    // 2. 恢复滤镜的默认属性
    [filter setDefaults];
    
    // 4. 通过KVC设置滤镜inputMessage数据
    [filter setValue:data forKey:@"inputMessage"];
    
    NSString *inputCorrectionLevel = @"Q";
    switch (level) {
        case BCQrImageCorrectionLevelLow:
            inputCorrectionLevel = @"L";
            break;
        case BCQrImageCorrectionLevelMiddle:
            inputCorrectionLevel = @"M";
            break;
        case BCQrImageCorrectionLevelQ:
            inputCorrectionLevel = @"Q";
            break;
        case BCQrImageCorrectionLevelHigh:
            inputCorrectionLevel = @"H";
            break;
        default:
            break;
    }
    
    //设置容错率
    [filter setValue:inputCorrectionLevel forKey:@"inputCorrectionLevel"];//L，M，Q，H
    // 5. 获得滤镜输出的图像
    CIImage *outputImage = [filter outputImage];
    
    // 6. 将CIImage转换成UIImage，并显示于imageView上 (此时获取到的二维码比较模糊,所以需要用下面的createNonInterpolatedUIImageFormCIImage方法重绘二维码)
    return [BCImageUtil createUIImageFormCIImage:outputImage withSize:size];//重绘二维码,使其显示清晰
}



/**
 * 根据CIImage生成指定大小的UIImage
 *
 * @param image CIImage
 * @param size 图片宽度
 */
+ (UIImage *)createUIImageFormCIImage:(CIImage *)image withSize:(CGFloat) size {
    CGRect extent = CGRectIntegral(image.extent);
    CGFloat scale = MIN(size/CGRectGetWidth(extent), size/CGRectGetHeight(extent));
    // 1.创建bitmap;
    size_t width = CGRectGetWidth(extent) * scale;
    size_t height = CGRectGetHeight(extent) * scale;
    CGColorSpaceRef cs = CGColorSpaceCreateDeviceGray();
    CGContextRef bitmapRef = CGBitmapContextCreate(nil, width, height, 8, 0, cs, (CGBitmapInfo)kCGImageAlphaNone);
    CIContext *context = [CIContext contextWithOptions:nil];
    CGImageRef bitmapImage = [context createCGImage:image fromRect:extent];
    CGContextSetInterpolationQuality(bitmapRef, kCGInterpolationNone);
    CGContextScaleCTM(bitmapRef, scale, scale);
    CGContextDrawImage(bitmapRef, extent, bitmapImage);
    // 2.保存bitmap到图片
    CGImageRef scaledImage = CGBitmapContextCreateImage(bitmapRef);
    CGContextRelease(bitmapRef);
    CGImageRelease(bitmapImage);
    return [UIImage imageWithCGImage:scaledImage];
}

+ (UIImage *)placeholderImage {
    static UIImage * __placeholderImage = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __placeholderImage = [BCImageUtil imageWithColor:[UIColor colorWithRed:51.0/255 green:51.0/255 blue:51.0/255 alpha:1.0]];
    });
    return __placeholderImage;
}

+ (UIImage *)walletLogoPlaceholderImage {
    static UIImage * __walletLogoPlaceholderImage = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __walletLogoPlaceholderImage = [BCImageUtil imageNamed:@"icon_wallet_logo_placeholder"];
    });
    return __walletLogoPlaceholderImage;
}
@end
