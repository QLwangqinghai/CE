//
//  BCImageView.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCImageView.h"
#import <SDWebImage/UIImageView+WebCache.h>

@implementation BCImageView


- (void)setImageWithURLString:(nullable NSString *)URLString placeholderImage:(UIImage *)placeholder completed:(void(^)(UIImage * _Nullable image, NSError * _Nullable error, NSURL * _Nullable imageURL))completedBlock {
    NSURL *URL = nil;
    if (URLString) {
        URL = [NSURL URLWithString:URLString];
    }
    [self setImageWithURL:URL placeholderImage:placeholder completed:completedBlock];
}

- (void)setImageWithURL:(nullable NSURL *)URL placeholderImage:(UIImage *)placeholder completed:(void(^)(UIImage * _Nullable image, NSError * _Nullable error, NSURL * _Nullable imageURL))completedBlock {
    [self sd_setImageWithURL:URL placeholderImage:placeholder completed:^(UIImage * _Nullable image, NSError * _Nullable error, SDImageCacheType cacheType, NSURL * _Nullable imageURL) {
        if (completedBlock) {
            completedBlock(image, error, imageURL);
        }
    }];
}


@end
