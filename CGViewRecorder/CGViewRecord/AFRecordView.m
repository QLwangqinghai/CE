//
//  AFRecordView.m
//  CGViewRecorder
//
//  Created by vector on 2019/5/7.
//  Copyright © 2019 vector. All rights reserved.
//

#import "AFRecordView.h"

@interface AFRecordView ()
{
    CFTimeInterval __time;
    
    NSInteger __times;
    
    CVPixelBufferPoolRef _pixelBufferPool;
    CGColorSpaceRef _colorSpace;
    
}
@property (nonatomic, readonly) CVPixelBufferPoolRef pixelBufferPool;
@property (nonatomic, readonly) CGColorSpaceRef colorSpace;
@property (nonatomic, assign) CGRect disBounds;

@end
@implementation AFRecordView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        __time = CACurrentMediaTime();
        self.backgroundColor = [UIColor grayColor];
        
        self.disBounds = CGRectMake(0, 0, frame.size.width, frame.size.height);

    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    [super drawRect:rect];
    
}

- (void)layoutSubviews {
    [super layoutSubviews];
}

//- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx {
//    UIGraphicsBeginImageContext(self.bounds.size);
//
//
//    [super drawLayer:layer inContext:ctx];
//    CGImageRef i = CGBitmapContextCreateImage(UIGraphicsGetCurrentContext());
//    UIGraphicsEndImageContext();
//
////    CGContextSaveGState(ctx);
////    CGContextRestoreGState(ctx);
////    CGImageRef img = CGBitmapContextCreateImage(ctx);
//
//    UIImage *img = [UIImage imageWithCGImage:i];
//
//    CFTimeInterval t = CACurrentMediaTime();
//
//
//    __times ++;
//
//    if (t - __time > 1) {
//        __time = t;
//
//        NSLog(@"times %ld", __times);
//        __times = 0;
//
//
//        UIView *view = [self snapshotViewAfterScreenUpdates:YES];
//
//
//        if (self.onImage) {
//            self.onImage(img, __time);
//        }
//
//    }
//}

- (UIImage *)screenShot {
    UIGraphicsBeginImageContextWithOptions(self.disBounds.size, NO, [UIScreen mainScreen].scale);
    [self.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *screenShotImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return screenShotImage;
}

- (CVPixelBufferPoolRef)pixelBufferPool {
    if (NULL == _pixelBufferPool) {
        CGSize videoSize = self.disBounds.size;
        NSDictionary *bufferAttributes = @{(id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA),
                                           (id)kCVPixelBufferCGBitmapContextCompatibilityKey : @(true),
                                           (id)kCVPixelBufferWidthKey : @(videoSize.width),
                                           (id)kCVPixelBufferHeightKey : @(videoSize.height),
                                           (id)kCVPixelBufferBytesPerRowAlignmentKey : @(videoSize.width * 4)
                                           };
        
        CVPixelBufferPoolCreate(NULL, NULL, (__bridge CFDictionaryRef)(bufferAttributes), &_pixelBufferPool);
    }
    return _pixelBufferPool;
}

- (CGColorSpaceRef)colorSpace {
    if (NULL == _colorSpace) {
        _colorSpace = CGColorSpaceCreateDeviceRGB();
    }
    return _colorSpace;
}

- (CGContextRef)creatBitmapContextWithPixelBuffer:(CVPixelBufferRef *)pixelBuffer {
    CVPixelBufferPoolCreatePixelBuffer(NULL, self.pixelBufferPool, pixelBuffer);
    CVPixelBufferLockBaseAddress(*pixelBuffer, 0);
    CGContextRef bitmapContext = NULL;
    CGSize _viewSize = self.disBounds.size;
    bitmapContext = CGBitmapContextCreate(CVPixelBufferGetBaseAddress(*pixelBuffer),
                                          CVPixelBufferGetWidth(*pixelBuffer),
                                          CVPixelBufferGetHeight(*pixelBuffer),
                                          8, CVPixelBufferGetBytesPerRow(*pixelBuffer), self.colorSpace,
                                          kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst
                                          );
    CGAffineTransform flipVertical = CGAffineTransformMake(1, 0, 0, -1, 0, _viewSize.height);
    CGContextConcatCTM(bitmapContext, flipVertical);
    
    return bitmapContext;
}

- (void)dis {
    usleep(30);
    
    CVPixelBufferRef pixelBuffer = nil;
    CGContextRef bitmapContext = [self creatBitmapContextWithPixelBuffer:&pixelBuffer];
    UIGraphicsPushContext(bitmapContext);

    CFTimeInterval bt = CACurrentMediaTime();
    
    @try {
        [self drawViewHierarchyInRect:self.disBounds afterScreenUpdates:false];
    } @catch (NSException *exception) {
//        CGContextRef context = UIGraphicsGetCurrentContext();
//        CGContextSetFillColorWithColor(context, [UIColorHex(0xFFFFFF) CGColor]);
//        CGContextFillRect(context, [UIScreen mainScreen].bounds);
//        SCLog(@"viewToImage - 截屏闪退Cache - %@", self.strLogRoom);
    } @finally {
        
    }
    CFTimeInterval et = CACurrentMediaTime();
    CGImageRef i = CGBitmapContextCreateImage(bitmapContext);

    UIGraphicsPopContext();
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);

    
//    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
//    void *baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);
//    size_t width = CVPixelBufferGetWidth(pixelBuffer);
//    size_t height = CVPixelBufferGetHeight(pixelBuffer);
//    size_t bufferSize = CVPixelBufferGetDataSize(pixelBuffer);
//    size_t bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
//
//    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
//    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, baseAddress, bufferSize, NULL);
//
//    CGImageRef cgImage = CGImageCreate(width, height, 8, 32, bytesPerRow, rgbColorSpace, kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrderDefault, provider, NULL, true, kCGRenderingIntentDefault);
//    UIImage *image = [UIImage imageWithCGImage:cgImage];
//    CGImageRelease(cgImage);
//    CGDataProviderRelease(provider);
//    CGColorSpaceRelease(rgbColorSpace);
//    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);

    UIImage *image = [UIImage imageWithCGImage:i];
    CGImageRelease(i);
    CGContextRelease(bitmapContext);

    CFTimeInterval t = CACurrentMediaTime();
    
    NSLog(@"%lf", et - bt);
    
    CFRelease(pixelBuffer);


    
    __times ++;
    
    if (t - __time > 3) {
        __time = t;
        
        NSLog(@"times %ld", __times);
        __times = 0;
        dispatch_async(dispatch_get_main_queue(), ^{
            if (self.onImage) {
                self.onImage(image, __time);
            }
        });
    }
}

- (void)dis222 {
    UIGraphicsBeginImageContextWithOptions(self.bounds.size, false, [UIScreen mainScreen].scale);
    
    CFTimeInterval bt = CACurrentMediaTime();
    [self drawViewHierarchyInRect:self.bounds afterScreenUpdates:true];
    CFTimeInterval et = CACurrentMediaTime();
    
    CGImageRef i = CGBitmapContextCreateImage(UIGraphicsGetCurrentContext());
    UIGraphicsEndImageContext();
    
    UIImage *img = [UIImage imageWithCGImage:i];
    CGImageRelease(i);
    
    CFTimeInterval t = CACurrentMediaTime();
    
    NSLog(@"%lf", et - bt);
    
    __times ++;
    
    if (t - __time > 3) {
        __time = t;
        
        NSLog(@"times %ld", __times);
        __times = 0;
        dispatch_async(dispatch_get_main_queue(), ^{
            if (self.onImage) {
                self.onImage(img, __time);
            }
        });
    }
}


@end





@interface AFRecorder ()
{
    CFTimeInterval __time;
    
    NSInteger __times;
    
    CVPixelBufferPoolRef _pixelBufferPool;
    CGColorSpaceRef _colorSpace;
    
}
@property (nonatomic, readonly) CVPixelBufferPoolRef pixelBufferPool;
@property (nonatomic, readonly) CGColorSpaceRef colorSpace;
@property (nonatomic, assign) CGRect bounds;
@property (nonatomic, strong) UIView * view;

@end
@implementation AFRecorder

- (instancetype)initWithView:(UIView *)view {
    self = [super init];
    if (self) {
        self.view = view;
        self.bounds = view.bounds;
        __time = CACurrentMediaTime();
    }
    return self;
}

//
//- (UIImage *)screenShot {
//    UIGraphicsBeginImageContextWithOptions(self.disBounds.size, NO, [UIScreen mainScreen].scale);
//    [self.layer renderInContext:UIGraphicsGetCurrentContext()];
//    UIImage *screenShotImage = UIGraphicsGetImageFromCurrentImageContext();
//    UIGraphicsEndImageContext();
//    return screenShotImage;
//}

- (CVPixelBufferPoolRef)pixelBufferPool {
    if (NULL == _pixelBufferPool) {
        CGSize videoSize = self.bounds.size;
        NSDictionary *bufferAttributes = @{(id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA),
                                           (id)kCVPixelBufferCGBitmapContextCompatibilityKey : @(true),
                                           (id)kCVPixelBufferWidthKey : @(videoSize.width),
                                           (id)kCVPixelBufferHeightKey : @(videoSize.height),
                                           (id)kCVPixelBufferBytesPerRowAlignmentKey : @(videoSize.width * 4)
                                           };
        
        CVPixelBufferPoolCreate(NULL, NULL, (__bridge CFDictionaryRef)(bufferAttributes), &_pixelBufferPool);
    }
    return _pixelBufferPool;
}

- (CGColorSpaceRef)colorSpace {
    if (NULL == _colorSpace) {
        _colorSpace = CGColorSpaceCreateDeviceRGB();
    }
    return _colorSpace;
}


- (CGContextRef)creatBitmapContextWithPixelBuffer:(CVPixelBufferRef *)pixelBuffer {
    CVPixelBufferPoolCreatePixelBuffer(NULL, self.pixelBufferPool, pixelBuffer);
    CVPixelBufferLockBaseAddress(*pixelBuffer, 0);
    CGContextRef bitmapContext = NULL;
    CGSize _viewSize = self.bounds.size;
    bitmapContext = CGBitmapContextCreate(CVPixelBufferGetBaseAddress(*pixelBuffer),
                                          CVPixelBufferGetWidth(*pixelBuffer),
                                          CVPixelBufferGetHeight(*pixelBuffer),
                                          8, CVPixelBufferGetBytesPerRow(*pixelBuffer), self.colorSpace,
                                          kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst
                                          );
    CGAffineTransform flipVertical = CGAffineTransformMake(1, 0, 0, -1, 0, _viewSize.height);
    CGContextConcatCTM(bitmapContext, flipVertical);
    assert(bitmapContext);
    return bitmapContext;
}

- (void)disprint {
    usleep(30);
    
    CVPixelBufferRef pixelBuffer = nil;
    CGContextRef bitmapContext = [self creatBitmapContextWithPixelBuffer:&pixelBuffer];
    assert(pixelBuffer);
    UIGraphicsPushContext(bitmapContext);
    
    CFTimeInterval bt = CACurrentMediaTime();
    
    @try {
        [self.view drawViewHierarchyInRect:self.bounds afterScreenUpdates:false];
    } @catch (NSException *exception) {
        //        CGContextRef context = UIGraphicsGetCurrentContext();
        //        CGContextSetFillColorWithColor(context, [UIColorHex(0xFFFFFF) CGColor]);
        //        CGContextFillRect(context, [UIScreen mainScreen].bounds);
        //        SCLog(@"viewToImage - 截屏闪退Cache - %@", self.strLogRoom);
    } @finally {
        
    }
    CFTimeInterval et = CACurrentMediaTime();
//    CGImageRef i = CGBitmapContextCreateImage(bitmapContext);
    
    UIGraphicsPopContext();
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
    
        CVPixelBufferLockBaseAddress(pixelBuffer, 0);
        void *baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);
        size_t width = CVPixelBufferGetWidth(pixelBuffer);
        size_t height = CVPixelBufferGetHeight(pixelBuffer);
        size_t bufferSize = CVPixelBufferGetDataSize(pixelBuffer);
        size_t bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
    
        CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, baseAddress, bufferSize, NULL);
    
        CGImageRef cgImage = CGImageCreate(width, height, 8, 32, bytesPerRow, rgbColorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst, provider, NULL, true, kCGRenderingIntentDefault);
        UIImage *image = [UIImage imageWithCGImage:cgImage];
        CGImageRelease(cgImage);
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(rgbColorSpace);
        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
//    UIImage *image = [UIImage imageWithCGImage:i scale:[UIScreen mainScreen].scale orientation:UIImageOrientationUp];
//    CGImageRelease(i);
    CGContextRelease(bitmapContext);
    
    CFTimeInterval t = CACurrentMediaTime();
    
    NSLog(@"%lf", et - bt);
    
    CFRelease(pixelBuffer);
    
    
    
    __times ++;
    
    if (t - __time > 3) {
        __time = t;
        
        NSLog(@"times %ld", __times);
        __times = 0;
        dispatch_async(dispatch_get_main_queue(), ^{
            if (self.onImage) {
                self.onImage(image, __time);
            }
        });
    }
}

@end
