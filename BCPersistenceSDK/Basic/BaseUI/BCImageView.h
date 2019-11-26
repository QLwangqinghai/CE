//
//  BCImageView.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BCImageView : UIImageView


- (void)setImageWithURLString:(NSString *)URLString placeholderImage:(UIImage *)placeholder completed:(void(^)(UIImage *image, NSError *error, NSURL *imageURL))completedBlock;


- (void)setImageWithURL:(NSURL *)URL placeholderImage:(UIImage *)placeholder completed:(void(^)(UIImage *image, NSError *error, NSURL * imageURL))completedBlock;

@end
