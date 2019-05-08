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
}

@end
@implementation AFRecordView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        __time = CACurrentMediaTime();
        self.backgroundColor = [UIColor grayColor];
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    [super drawRect:rect];
    
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
    UIGraphicsBeginImageContextWithOptions(self.bounds.size, NO, [UIScreen mainScreen].scale);
    [self.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *screenShotImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return screenShotImage;
}

- (void)dis {
    UIGraphicsBeginImageContext(self.bounds.size);
    
    CFTimeInterval bt = CACurrentMediaTime();
    [self drawViewHierarchyInRect:self.bounds afterScreenUpdates:false];
    CFTimeInterval et = CACurrentMediaTime();
    
    CGImageRef i = CGBitmapContextCreateImage(UIGraphicsGetCurrentContext());
    UIGraphicsEndImageContext();
    
    UIImage *img = [UIImage imageWithCGImage:i];
    CGImageRelease(i);
    
    CFTimeInterval t = CACurrentMediaTime();
    
    NSLog(@"%lf", et - bt);
    
    __times ++;
    
    if (t - __time > 1) {
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
