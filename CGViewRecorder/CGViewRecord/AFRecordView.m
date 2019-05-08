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
    }
    return self;
}


- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx {
    [super drawLayer:layer inContext:ctx];
    
    CGImageRef img = CGBitmapContextCreateImage(ctx);
    
    CFTimeInterval t = CACurrentMediaTime();
    
    
    __times ++;

    if (t - __time > 1) {
        __time = t;
        
        NSLog(@"123- %ld", __times);
        __times = 0;
    }
}

- (UIImage *)screenShot {
    UIGraphicsBeginImageContextWithOptions(self.bounds.size, NO, [UIScreen mainScreen].scale);
    [self.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *screenShotImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return screenShotImage;
}
@end
