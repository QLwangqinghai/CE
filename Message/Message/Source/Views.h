//
//  Views.h
//  Message
//
//  Created by vector on 2019/6/27.
//  Copyright © 2019 vector. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@protocol CustomMaskViewMaskerProtocol <NSObject>

- (void)updateMaskWithBounds:(CGRect)bounds handler: (void(^)(CGPathRef path)) handler;

@end

@interface CustomMaskView : UIView

@property (nonatomic, weak, readonly) UIView * contentView;
@property (nonatomic, weak, readonly) UIImageView * backgroundImageView;
@property (nonatomic) UIImage * image;

@property (nonatomic, strong) id<CustomMaskViewMaskerProtocol> masker;

- (void)makeSubViews;
- (void)setNeedsUpdateMask;
- (void)refreshMask;

@end


@interface MessageBubbleMasker: NSObject <CustomMaskViewMaskerProtocol>

@property (nonatomic, assign) BOOL isLeft;
@property (nonatomic, assign) CGRect bounds;

- (void)updateMaskWithBounds:(CGRect)bounds handler: (void(^)(CGPathRef path)) handler;

@end

@class MessageBubbleView;
typedef void(^MessageBubbleViewBlock)(MessageBubbleView * view);

@interface MessageBubbleView : CustomMaskView

@property (nonatomic, assign) BOOL isLeft;
@property (nonatomic, assign, readonly) UIEdgeInsets contentInsets;

@property (nonatomic, copy) MessageBubbleViewBlock taped;
+ (MessageBubbleView *)maskedBubbleViewWithFrame:(CGRect)frame isLeft: (BOOL) isLeft;

+ (MessageBubbleView *)rectBubbleViewWithFrame:(CGRect)frame isLeft: (BOOL) isLeft;

@end





@interface TimeTipView : UIView
@property (nonatomic, copy) NSString * timeString;

@end


@interface MessageContentView : UIView

- (void)configWith:(IMAMsg *)message conversation:(IMAConversation *)conversation;
- (void)setNeedsUpdateFromMessage;
@end


@interface MessageFooterView : UIView

@end


@interface C2CMessageHeaderContentView : MessageContentView
{
    BOOL _isLeft;
}

@property (nonatomic, assign, readonly) BOOL isLeft;
@property (nonatomic, weak, readonly) WebImageView *avatarImageView;
@property (nonatomic, weak) MessageSendStateView * sendStatesView;
@property (nonatomic, strong) IMAUser * sender;

- (instancetype) initWithDetailContentView: (MessageDetailContentView *)contentView isLeft: (BOOL) isLeft;

- (void)makeSubViews;

@end
NS_ASSUME_NONNULL_END
