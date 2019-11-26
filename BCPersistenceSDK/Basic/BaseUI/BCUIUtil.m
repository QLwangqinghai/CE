//
//  BCUIUtil.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/6.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCUIUtil.h"
#include <sys/sysctl.h>


@interface BCUIUtil ()

@property (nonatomic, assign) CGSize screenSize;
@property (nonatomic, assign) BOOL isPad;
@property (nonatomic, assign) BOOL isIPhoneX;
@property (nonatomic, assign) BOOL isIPhone;

@end

@implementation BCUIUtil

- (instancetype)initPrivate {
    self = [self init];
    if (self) {
        self.isPad = UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad;
        
        CGSize size = [UIScreen mainScreen].bounds.size;
        if (size.width > size.height) {
            CGFloat tmp = size.width;
            size.width = size.height;
            size.height = tmp;
        }
        if (CGSizeEqualToSize(size, CGSizeMake(375, 812))) {
            self.isIPhoneX = true;
        }
        self.screenSize = size;
        
        NSString *deviceType = [UIDevice currentDevice].model;
        self.isIPhone = [deviceType isEqualToString:@"iPhone"];

    }
    return self;
}



+ (BOOL)isPad {
    return [BCUIUtil sharedInstance].isPad;
}

+ (BOOL)isIPhoneX {
    return [BCUIUtil sharedInstance].isIPhoneX;
}

+ (BOOL)isIPhone {
    return [BCUIUtil sharedInstance].isIPhone;
}
+ (CGSize)screenSize {
    return [BCUIUtil sharedInstance].screenSize;
}
+ (CGFloat)screenWidth {
    return [BCUIUtil sharedInstance].screenSize.width;
}
+ (CGFloat)screenHeight {
    return [BCUIUtil sharedInstance].screenSize.height;
}

+ (BCUIUtil *)sharedInstance {
    static BCUIUtil *__sharedInstance = nil;
    static dispatch_once_t one;
    dispatch_once(&one, ^{
        __sharedInstance = [[BCUIUtil alloc] initPrivate];
    });
    return __sharedInstance;
}
+ (CGFloat)navigationBarHeight {
    return [[UIApplication sharedApplication] statusBarFrame].size.height + 44;
}

+ (CGFloat)statusBarHeight {
    return [[UIApplication sharedApplication] statusBarFrame].size.height;
}

@end


