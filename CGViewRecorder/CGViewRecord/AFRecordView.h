//
//  AFRecordView.h
//  CGViewRecorder
//
//  Created by vector on 2019/5/7.
//  Copyright © 2019 vector. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN



@interface AFRecordView : UIView

@property (nonatomic, copy) void(^onImage)(UIImage * image, CFTimeInterval time);

- (UIImage *)screenShot;

- (void)dis;

@end


@interface AFRecorder : NSObject

@property (nonatomic, copy) void(^onImage)(UIImage * image, CFTimeInterval time);

- (instancetype)initWithView:(UIView *)view;

- (UIImage *)screenShot;
- (void)disprint;

@end
NS_ASSUME_NONNULL_END
