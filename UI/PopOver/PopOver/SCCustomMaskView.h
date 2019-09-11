//
//  SCCustomMaskView.h
//  SC_Lecture
//
//  Created by vector on 2019/9/11.
//  Copyright © 2019 Shituo Han. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@protocol SCCustomMaskViewMaskerProtocol <NSObject>

- (void)updateMaskWithBounds:(CGRect)bounds handler: (void(^)(CGPathRef path)) handler;

@end
@interface SCCustomMaskView : UIView

@property (nonatomic, weak, readonly) UIView * contentView;
@property (nonatomic, weak, readonly) UIImageView * backgroundImageView;
@property (nonatomic) UIImage * image;

@property (nonatomic, strong) id<SCCustomMaskViewMaskerProtocol> masker;

- (void)makeSubViews;
- (void)setNeedsUpdateMask;
- (void)refreshMask;

@end







NS_ASSUME_NONNULL_END
