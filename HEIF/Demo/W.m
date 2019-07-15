//
//  W.m
//  Demo
//
//  Created by vector on 2019/7/15.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "W.h"
#import <SDWebImageHEIFCoder/SDWebImageHEIFCoder.h>

@implementation W

+ (void)go {
    UIImage * image = [UIImage imageNamed:@"p.png"];
    UIImage * image1 = [UIImage imageNamed:@"png1.png"];
    
    NSData * d = [[SDImageHEIFCoder sharedCoder] encodedDataWithImage:image format:SDImageFormatHEIF options:NULL];
    NSData * d1 = [[SDImageHEIFCoder sharedCoder] encodedDataWithImage:image1 format:SDImageFormatHEIF options:NULL];
    NSLog(@"%ld, %ld", d.length, d1.length);
    
    
    
}

@end
