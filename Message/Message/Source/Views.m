//
//  Views.m
//  Message
//
//  Created by vector on 2019/6/27.
//  Copyright © 2019 vector. All rights reserved.
//

#import "Views.h"

@interface CustomMaskView ()
{
    CAShapeLayer *_maskShapeLayer;
}

@property (nonatomic, weak) UIImageView * realContentView;

@end

@implementation CustomMaskView

- (UIView *)contentView {
    return self.realContentView;
}

- (UIImageView *)backgroundImageView {
    return self.realContentView;
}

- (void)setImage:(UIImage *)image {
    self.realContentView.image = image;
}

- (UIImage *)image {
    return self.realContentView.image;
}

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self makeSubViewsPrivate];
        [self makeSubViews];
    }
    return self;
}

- (void)makeSubViewsPrivate {
    UIImageView * contentView = [[UIImageView alloc] init];
    [self addSubview:contentView];
    contentView.userInteractionEnabled = true;
    contentView.frame = self.bounds;
    self.realContentView = contentView;
    [contentView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self).insets(UIEdgeInsetsZero);
    }];
}
- (void)makeSubViews {
}

- (void)layoutSubviews {
    [super layoutSubviews];
    [self.masker updateMaskWithBounds:self.bounds handler:^(CGPathRef path) {
        [self setMaskWithCGPath:path];
        self.layer.shadowPath = path;
    }];
    
}

- (void)setMasker:(id<CustomMaskViewMaskerProtocol>)masker {
    if (_masker != masker) {
        _masker = masker;
        [self setNeedsUpdateMask];
    }
}


- (void)setNeedsUpdateMask {
    [self setNeedsLayout];
}


- (void)setMaskWithCGPath:(CGPathRef)path{
    if (!_maskShapeLayer) {
        //蒙版
        CAShapeLayer *maskLayer = [CAShapeLayer layer];
        maskLayer.path = path;
        maskLayer.fillColor = [[UIColor whiteColor] CGColor];
        maskLayer.frame = self.bounds;
        self.contentView.layer.mask = maskLayer;
        _maskShapeLayer = maskLayer;
    } else {
        _maskShapeLayer.path = path;
        _maskShapeLayer.frame = self.bounds;
    }
}

- (void)refreshMask {
    [self.masker updateMaskWithBounds:self.bounds handler:^(CGPathRef path) {
        [self setMaskWithCGPath:path];
        self.layer.shadowPath = path;
    }];
}

@end


#define MessageBubbleMaskerArrowTipWidth 5

@implementation MessageBubbleMasker

- (void)updateMaskWithBounds:(CGRect)bounds handler: (void(^)(CGPathRef path)) handler {
    self.bounds = bounds;
    if (_isLeft) {
        [self setupLeftWithHandler:handler];
    } else {
        [self setupRightWithHandler:handler];
    }
}

- (void)setupRightWithHandler: (void(^)(CGPathRef path)) handler
{
    CGMutablePathRef bubblePath = CGPathCreateMutable();
    CGPoint origin = self.bounds.origin;
    CGSize size = self.bounds.size;
    CGFloat cornerRadius = 5;
    
    CGFloat arrowTipRadiusA = 5;//
    CGFloat arrowTipRadiusB = 1;//尖部分
    CGFloat arrowTipWidth = arrowTipRadiusA;
    CGFloat arrowTipToTop = 14;
    
    CGPathMoveToPoint(bubblePath, NULL, origin.x, origin.y + cornerRadius);
    //左上角半圆
    CGPathAddArcToPoint(bubblePath, NULL, origin.x, origin.y, origin.x + cornerRadius, origin.y, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - cornerRadius - arrowTipWidth, origin.y);
    
    //右上角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + size.width - arrowTipWidth, origin.y, origin.x + size.width - arrowTipWidth, origin.y + cornerRadius, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - arrowTipWidth, origin.y + arrowTipToTop);
    
    //尖 - begin
    CGPathAddArc(bubblePath, NULL, origin.x + size.width, origin.y + arrowTipToTop, arrowTipRadiusA, M_PI, M_PI * 2.0 / 3, 1);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - arrowTipRadiusB, origin.y + arrowTipToTop + arrowTipRadiusA);
    CGPathAddArc(bubblePath, NULL, origin.x + size.width - arrowTipRadiusB, origin.y + arrowTipToTop + arrowTipRadiusA + arrowTipRadiusB, arrowTipRadiusB, - M_PI_2 , M_PI_2, 0);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - arrowTipRadiusA/2, origin.y + arrowTipToTop + arrowTipRadiusA + arrowTipRadiusB * 2 + (arrowTipRadiusA - sqrt(3) * arrowTipRadiusA/2));
    CGPathAddArc(bubblePath, NULL, origin.x + size.width, origin.y + arrowTipToTop + arrowTipRadiusA * 2 + arrowTipRadiusB * 2, arrowTipRadiusA, -(M_PI_2 + M_PI / 3), M_PI, 1);
    //尖 - end
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - arrowTipWidth, origin.y + size.height - cornerRadius);
    
    //右下角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + size.width - arrowTipWidth, origin.y + size.height, origin.x + size.width - cornerRadius - arrowTipWidth, origin.y + size.height, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + cornerRadius, origin.y + size.height);
    
    //左下角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x, origin.y + size.height, origin.x, origin.y + size.height - cornerRadius, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x, origin.y + cornerRadius);
    
    CGPathCloseSubpath(bubblePath);
    if (handler) {
        handler(bubblePath);
    }
    CGPathRelease(bubblePath);
}

- (void)setupLeftWithHandler: (void(^)(CGPathRef path)) handler
{
    CGMutablePathRef bubblePath = CGPathCreateMutable();
    CGPoint origin = self.bounds.origin;
    CGSize size = self.bounds.size;
    CGFloat cornerRadius = 5;
    
    CGFloat arrowTipRadiusA = 5;//
    CGFloat arrowTipRadiusB = 1;//尖部分
    CGFloat arrowTipWidth = arrowTipRadiusA;
    CGFloat arrowTipToTop = 14;
    
    CGPathMoveToPoint(bubblePath, NULL, origin.x + arrowTipWidth, origin.y + cornerRadius);
    
    //左上角半圆
    CGPathAddArcToPoint(bubblePath, NULL, origin.x + arrowTipWidth, origin.y, origin.x + cornerRadius + arrowTipWidth, origin.y, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - cornerRadius, origin.y);
    
    //右上角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + size.width, origin.y, origin.x + size.width , origin.y + cornerRadius, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width, origin.y + size.height - cornerRadius);
    
    //右下角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + size.width, origin.y + size.height, origin.x + size.width - cornerRadius, origin.y + size.height, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + cornerRadius + arrowTipWidth, origin.y + size.height);
    
    //左下角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + arrowTipWidth, origin.y + size.height, origin.x + arrowTipWidth, origin.y + size.height - cornerRadius, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + arrowTipWidth, origin.y + arrowTipRadiusA * 2 + arrowTipRadiusB * 2 + arrowTipToTop);
    
    //尖 - begin
    CGPathAddArc(bubblePath, NULL, origin.x, origin.y + arrowTipToTop + arrowTipRadiusA * 2 + arrowTipRadiusB * 2, arrowTipRadiusA, 0, -(M_PI / 3), 1);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + arrowTipRadiusB, origin.y + arrowTipToTop + arrowTipRadiusA + arrowTipRadiusB * 2);
    CGPathAddArc(bubblePath, NULL, origin.x + arrowTipRadiusB, origin.y + arrowTipToTop + arrowTipRadiusA + arrowTipRadiusB, arrowTipRadiusB, M_PI_2 , 3 * M_PI_2, 0);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + arrowTipRadiusA/2, origin.y + arrowTipToTop + arrowTipRadiusA * sqrt(3) / 2);
    CGPathAddArc(bubblePath, NULL, origin.x, origin.y + arrowTipToTop, arrowTipRadiusA, M_PI / 3, 0, 1);
    //尖 - end
    
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + arrowTipWidth, origin.y + cornerRadius);
    CGPathCloseSubpath(bubblePath);
    if (handler) {
        handler(bubblePath);
    }
    CGPathRelease(bubblePath);
}
@end


@interface MessageBubbleView ()
@property (nonatomic, assign) UIEdgeInsets contentInsets;
@property (nonatomic, assign) BOOL willTapped;

@end
@implementation MessageBubbleView

- (void)setIsLeft:(BOOL)isLeft {
    if (_isLeft != isLeft) {
        _isLeft = isLeft;
        [self setNeedsUpdateMask];
    }
}

- (instancetype)initWithFrame:(CGRect)frame isLeft: (BOOL) isLeft {
    if (self = [super initWithFrame:frame]) {
        _isLeft = isLeft;
        
        
        self.layer.shadowOffset = CGSizeMake(0, 3);
        self.layer.shadowOpacity = 0.4;//0.4;
        self.layer.shadowRadius = 4;
        //        if (isLeft) {
        self.layer.shadowColor = [UIColor colorWithRed:238.0/255 green:238.0/255 blue:238.0/255 alpha:1.0].CGColor;
    }
    return self;
}

+ (MessageBubbleView *)maskedBubbleViewWithFrame:(CGRect)frame isLeft: (BOOL) isLeft {
    CGFloat left = 0;
    CGFloat right = 0;
    if (isLeft) {
        left = MessageBubbleMaskerArrowTipWidth;
    } else {
        right = MessageBubbleMaskerArrowTipWidth;
    }
    
    MessageBubbleView * bubbleView = [[MessageBubbleView alloc] initWithFrame:frame isLeft:isLeft];
    bubbleView.contentInsets = UIEdgeInsetsMake(0, left, 0, right);
    
    MessageBubbleMasker * masker = [[MessageBubbleMasker alloc] init];
    masker.isLeft = isLeft;
    bubbleView.masker = masker;
    return bubbleView;
}

+ (MessageBubbleView *)rectBubbleViewWithFrame:(CGRect)frame isLeft: (BOOL) isLeft {
    MessageBubbleView * bubbleView = [[MessageBubbleView alloc] initWithFrame:frame isLeft:isLeft];
    bubbleView.contentInsets = UIEdgeInsetsMake(0, 0, 0, 0);
    return bubbleView;
}



- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    NSLog(@"touchesBegantouchesBegantouchesBegantouchesEndedtouchesEnded");
    self.willTapped = (touches.count == 1);
    
    [super touchesBegan:touches withEvent:event];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [super touchesMoved:touches withEvent:event];
    NSLog(@"touchesMoved------move------move");
    self.willTapped = false;
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    NSLog(@"touchesEndedtouchesEndedtouchesEndedtouchesEndedtouchesEnded");
    [super touchesEnded:touches withEvent:event];
    
    if (self.willTapped) {
        NSLog(@"self.willTappedself.willTappedself.willTapped");
        if (self.taped) {
            self.taped(self);
        }
        self.willTapped = false;
    }
}
@end






@interface TimeTipView()

@property (nonatomic, weak) UILabel * timeLabel;

@end
@implementation TimeTipView
- (instancetype) initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        self.backgroundColor = [UIColor whiteColor];
        
        UILabel * timeLabel = [[UILabel alloc] init];
        timeLabel.textAlignment = NSTextAlignmentCenter;
        timeLabel.lineBreakMode = NSLineBreakByWordWrapping;
        timeLabel.numberOfLines = 0;
        [self addSubview:timeLabel];
        timeLabel.font = [UIFont systemFontOfSize:10];
        self.timeLabel = timeLabel;
        [timeLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.centerX.equalTo(self.mas_centerX);
            make.bottom.equalTo(self.mas_bottom).offset(0);
            make.width.equalTo(@200);
        }];
        self.clipsToBounds = true;
    }
    return self;
}

- (void)setTimeString:(NSString *)timeString {
    self.timeLabel.text = timeString;
}

- (NSString *)timeString {
    return self.timeLabel.text;
}
@end


@implementation MessageContentView
- (void)configWith:(IMAMsg *)message conversation:(IMAConversation *)conversation {
}
- (void)setNeedsUpdateFromMessage {
}

@end


@interface MessageFooterView()

@property (nonatomic, weak) YYLabel * tipLabel;

@end
@implementation MessageFooterView

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        YYLabel * tipLabel = [[YYLabel alloc] init];
        [self addSubview:tipLabel];
        self.tipLabel = tipLabel;
        [tipLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.centerX.equalTo(self.mas_centerX);
            make.top.equalTo(self.mas_top).offset(2);
            make.width.equalTo(@250);
        }];
        self.clipsToBounds = true;
    }
    return self;
}

@end






@interface MessageHeaderContentView()
@property (nonatomic, strong) MessageDetailContentView * contentView;
@property (nonatomic, weak) UIImageView * funsIconImageView;
@property (nonatomic, weak) UILabel * nickLabel;
@property (nonatomic, strong) IMAMsg * message;

@property (nonatomic, strong) GroupMemberInfo * member;

@property (nonatomic, strong) FBKVOController * userKVO;

@end
@implementation MessageHeaderContentView

- (void)setMember:(GroupMemberInfo *)member {
    if (_member != member) {
        _member = member;
        
        __weak typeof(self) weakSelf = self;
        [self.userKVO unobserveAll];
        if (member) {
            [self.userKVO observe:member keyPath:@"showName" options:NSKeyValueObservingOptionNew block:^(id observer, id object, NSDictionary *change) {
                [weakSelf refreshUIFromUserInfo];
            }];
            [self.userKVO observe:member keyPath:@"faceUrl" options:NSKeyValueObservingOptionNew block:^(id observer, id object, NSDictionary *change) {
                [weakSelf refreshUIFromUserInfo];
            }];
            [self.userKVO observe:member keyPath:@"showFunsIcon" options:NSKeyValueObservingOptionNew block:^(id observer, id object, NSDictionary *change) {
                [weakSelf refreshUIFromUserInfo];
            }];
        }
    }
}

- (instancetype)initWithFrame:(CGRect)frame {
    return nil;
}

- (instancetype) initWithDetailContentView: (MessageDetailContentView *)contentView isLeft: (BOOL) isLeft {
    if (self = [super initWithFrame:CGRectZero]) {
        _isLeft = isLeft;
        self.contentView = contentView;
        __weak typeof(self) weakSelf = self;
        
        // 可以 加长按手势事件
        
        contentView.actionClosure = ^(MessageContentViewAction *actionInfo) {
            [weakSelf handleContentViewAction:actionInfo];
        };
        _userKVO = [FBKVOController controllerWithObserver:self];
        [self makeSubViews];
    }
    return self;
}

- (void)makeSubViews {
    self.backgroundColor = [MessageViewConfig messageBackgroundColor];
    
    //头像
    WebImageView *avatarImageView = [[WebImageView alloc] init];
    [self addSubview:avatarImageView];
    _avatarImageView = avatarImageView;
    avatarImageView.layer.cornerRadius = [MessageViewConfig headerSize]/2;
    avatarImageView.layer.masksToBounds = true;
    
    [avatarImageView mas_makeConstraints:^(MASConstraintMaker *make) {
        if (self.isLeft) {
            make.left.equalTo(self.mas_left).offset([MessageViewConfig headerToBoundary]);
        } else {
            make.right.equalTo(self.mas_right).offset(-[MessageViewConfig headerToBoundary]);
        }
        make.top.equalTo(self.mas_top).offset([MessageViewConfig headerToTop]);
        make.width.equalTo(@([MessageViewConfig headerSize]));
        make.height.equalTo(@([MessageViewConfig headerSize]));
    }];
    
    //粉丝牌
    UIImageView * funsIconImageView = [[UIImageView alloc] init];
    [self addSubview:funsIconImageView];
    self.funsIconImageView = funsIconImageView;
    
    //    [funsIconImageView setContentHuggingPriority:UILayoutPriorityDefaultHigh forAxis:(UILayoutConstraintAxisHorizontal)];
    //    [funsIconImageView setContentHuggingPriority:UILayoutPriorityDefaultHigh forAxis:(UILayoutConstraintAxisVertical)];
    //
    //    [funsIconImageView setContentCompressionResistancePriority:UILayoutPriorityDefaultHigh forAxis:(UILayoutConstraintAxisHorizontal)];
    //    [funsIconImageView setContentCompressionResistancePriority:UILayoutPriorityDefaultHigh forAxis:(UILayoutConstraintAxisVertical)];
    
    
    //昵称
    UILabel * nickLabel = [[UILabel alloc] init];
    [self addSubview:nickLabel];
    nickLabel.numberOfLines = 1;
    nickLabel.textColor = [MessageViewConfig nickTextColor];
    nickLabel.font = [MessageViewConfig nickTextFont];
    self.nickLabel = nickLabel;
    if (self.isLeft) {
        nickLabel.textAlignment = NSTextAlignmentLeft;
    } else {
        nickLabel.textAlignment = NSTextAlignmentRight;
    }
    //粉丝牌一行 占位的空白
    UIView * spaceView = [[UIView alloc] init];
    [self addSubview:spaceView];
    
    
    //    spaceView.backgroundColor = [CommonColor randomRGBColor];
    //    funsIconImageView.backgroundColor = [CommonColor randomRGBColor];
    
    
    [funsIconImageView mas_makeConstraints:^(MASConstraintMaker *make) {
        //        make.height.lessThanOrEqualTo(@19);
        make.height.equalTo(@20);
        make.width.equalTo(@0);
        
        //        make.width.lessThanOrEqualTo(@80);
        make.centerY.equalTo(avatarImageView.mas_top).offset([MessageViewConfig nickYCenterToHeaderTop]);
        if (self.isLeft) {
            make.left.equalTo(avatarImageView.mas_right).offset([MessageViewConfig nickToHeader]);
        } else {
            make.right.equalTo(nickLabel.mas_left).offset(-2);
            make.left.greaterThanOrEqualTo(self.mas_left).offset(15);
        }
    }];
    [nickLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@19);
        make.centerY.equalTo(avatarImageView.mas_top).offset([MessageViewConfig nickYCenterToHeaderTop]);
        if (self.isLeft) {
            //            make.left.equalTo(avatarImageView.mas_right).offset([MessageViewConfig nickToHeader]);
            make.left.equalTo(funsIconImageView.mas_right).offset(7);
            make.right.lessThanOrEqualTo(self.mas_right).offset(-15);
        } else {
            make.right.equalTo(avatarImageView.mas_left).offset(-[MessageViewConfig nickToHeader]);
            //            make.right.equalTo(funsIconImageView.mas_left).offset(-2);
            //            make.left.greaterThanOrEqualTo(self.mas_left).offset(15);
        }
    }];
    [nickLabel setContentHuggingPriority:UILayoutPriorityFittingSizeLevel forAxis:(UILayoutConstraintAxisHorizontal)];
    [nickLabel setContentHuggingPriority:UILayoutPriorityFittingSizeLevel forAxis:(UILayoutConstraintAxisVertical)];
    
    [nickLabel setContentCompressionResistancePriority:UILayoutPriorityDefaultLow forAxis:(UILayoutConstraintAxisHorizontal)];
    [nickLabel setContentCompressionResistancePriority:UILayoutPriorityDefaultLow forAxis:(UILayoutConstraintAxisVertical)];
    
    [spaceView mas_makeConstraints:^(MASConstraintMaker *make) {
        //        make.height.lessThanOrEqualTo(@19);
        make.height.equalTo(@15);
        make.width.greaterThanOrEqualTo(@60);
        //        make.width.lessThanOrEqualTo(@80);
        make.centerY.equalTo(avatarImageView.mas_top).offset([MessageViewConfig nickYCenterToHeaderTop]);
        if (self.isLeft) {
            make.left.equalTo(nickLabel.mas_right);
            make.right.equalTo(self.mas_right);
        } else {
            make.right.equalTo(funsIconImageView.mas_left);
            make.left.equalTo(self.mas_left);
        }
    }];
    
    
    UIView * contentView = self.contentView;
    if (!contentView) {
        contentView = [[UIView alloc] init];
    }
    [self addSubview:contentView];
    
    //supeview 设置 左顶点 或者右顶点约束， contentView 的subview 给 contentView 设置宽高约束
    [contentView mas_makeConstraints:^(MASConstraintMaker *make) {
        if (self.isLeft) {
            make.left.equalTo(self.avatarImageView.mas_right).offset([MessageViewConfig headerToBubbleContainer]);
        } else {
            make.right.equalTo(self.avatarImageView.mas_left).offset(-[MessageViewConfig headerToBubbleContainer]);
        }
        make.top.equalTo(self.avatarImageView.mas_top).offset([MessageViewConfig groupHeaderTopToBubbleContainerTop]);
    }];
    
    if (!self.isLeft) {
        __weak typeof(self) weakSelf = self;
        MessageSendStateView * sendStatesView = [[MessageSendStateView alloc] initWithFrame:CGRectMake(0, 0, 30, 30)];
        [self addSubview:sendStatesView];
        sendStatesView.didResend = ^(MessageSendStateView * currentSendStatesView) {
            __strong typeof(weakSelf) strongSelf = weakSelf;
            if (strongSelf.actionClosure) {
                strongSelf.actionClosure([MessageContentViewAction actionWithType:MessageContentActionTypeResendMessage]);
            }
        };
        
        self.sendStatesView = sendStatesView;
        [sendStatesView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.right.equalTo(contentView.mas_left).offset(-[MessageViewConfig headerToBubbleContainer]);
            make.centerY.equalTo(contentView.mas_centerY);
            make.height.equalTo(@(40));
            make.width.equalTo(@(40));
        }];
    }
}

- (void)copyTextToPasetboard:(IMAMsg *)message {
    [self.contentView copyTextToPasetboard:message];
}

- (void)handleContentViewAction:(MessageContentViewAction *) actionInfo {
    [super handleContentViewAction:actionInfo];
}

- (void)setNeedsUpdateFromMessage {
    self.sendStatesView.status = [self.message showStatus];
    
    [self.contentView setNeedsUpdateFromMessage];
}

- (void)configWith:(IMAMsg *)message conversation:(IMAConversation *)conversation {
    self.message = message;
    [super configWith:message conversation:conversation];
    GroupMemberCache * memberCache = [IMUserCacheCenter queryGroupMemberCacheWithGroupId:[conversation receiver]];
    self.member = [memberCache memberWithUid:message.senderId initHandler:^(GroupMemberInfo *member) {
        if ([message.msg GetSenderGroupMemberProfile]) {
            member.groupNameCard = [message.msg GetSenderGroupMemberProfile].nameCard;
            if ([message.msg GetSenderGroupMemberProfile].role != 0) {
                member.role = [message.msg GetSenderGroupMemberProfile].role;
            }
            member.groupFansLevel = [[message.msg GetSenderGroupMemberProfile] groupFansLevel];
        }
        if ([message.msg GetSenderProfile]) {
            member.faceUrl = [message.msg GetSenderProfile].faceURL;
            member.nick = [message.msg GetSenderProfile].nickname;
        }
        
        if ([message.msg GetSenderProfile] && [message.msg GetSenderGroupMemberProfile]) {
            member.time = message.timestamp;
        }
    } updateHandler:^(GroupMemberInfo *member) {
        if ([message.msg GetSenderProfile] && [message.msg GetSenderGroupMemberProfile]) {
            if (member.time < message.timestamp) {
                member.groupNameCard = [message.msg GetSenderGroupMemberProfile].nameCard;
                member.faceUrl = [message.msg GetSenderProfile].faceURL;
                member.nick = [message.msg GetSenderProfile].nickname;
                member.time = message.timestamp;
                if ([message.msg GetSenderGroupMemberProfile].role != 0) {
                    member.role = [message.msg GetSenderGroupMemberProfile].role;
                }
                member.groupFansLevel = [[message.msg GetSenderGroupMemberProfile] groupFansLevel];
            }
        }
    }];
    [self.contentView configWithMessage:message conversation:conversation];
    self.sendStatesView.status = [self.message showStatus];
    
    [self refreshUIFromUserInfo];
}
- (void)configContext: (MessageContext *)context {
    [super configContext:context];
    [self.contentView configContext:context];
}
- (void)refreshUIFromUserInfo {
    __weak typeof(self) weakSelf = self;
    NSURL *iconURL = nil;
    if ([self.message isMineMsg]) {
        iconURL = [NSURL URLWithString:[LoginHelper avatar]];
    } else {
        iconURL = [NSURL URLWithString:self.member.faceUrl];
    }
    [self.avatarImageView setUrl:iconURL placeholderImage:[ImageUtil imageNamed:kstanceImage] completion:^(UIImage *image, NSURL *imageURL) {
        
    } tap:^(UITapGestureRecognizer *sender) {
        [weakSelf handleContentViewAction:[MessageContentViewAction actionWithType:MessageContentActionTypeHeaderTapped]];
    }];
    
    self.nickLabel.text = self.member.showName;
    self.funsIconImageView.image = self.member.showFunsIcon;
    
    CGFloat funsIconImageViewWidth = self.member.showFunsIcon.size.width;
    if (self.member.showFunsIcon) {
        funsIconImageViewWidth = 50;
    } else {
        funsIconImageViewWidth = 0;
    }
    //    if (funsIconImageViewWidth > 50) {
    //        funsIconImageViewWidth = 50;
    //    }
    
    [self.funsIconImageView mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(funsIconImageViewWidth));
    }];
    [self.superview setNeedsLayout];
    //    self.nickLabel.text = [self.message.msg GetSenderGroupMemberProfile].nameCard;
}

- (void)dealloc
{
    [self.userKVO unobserveAll];
    self.userKVO = nil;
}

@end



@interface C2CMessageHeaderContentView()
@property (nonatomic, strong) MessageDetailContentView * contentView;
@property (nonatomic, weak) UILabel * nickLabel;
@property (nonatomic, strong) IMAMsg * message;


@property (nonatomic, strong) FBKVOController * userKVO;

@end
@implementation C2CMessageHeaderContentView

- (instancetype)initWithFrame:(CGRect)frame {
    return nil;
}

- (void)setSender:(IMAUser *)sender {
    if (_sender != sender) {
        _sender = sender;
        __weak typeof(self) weakSelf = self;
        [self.userKVO unobserveAll];
        if (_sender) {
            [self.userKVO observe:_sender keyPaths:@[@"nickName", @"remark", @"icon"] options:NSKeyValueObservingOptionNew block:^(id observer, id object, NSDictionary *change) {
                [weakSelf refreshUIFromUserInfo];
            }];
        }
    }
}

- (instancetype) initWithDetailContentView: (MessageDetailContentView *)contentView isLeft: (BOOL) isLeft {
    if (self = [super initWithFrame:CGRectZero]) {
        _isLeft = isLeft;
        self.contentView = contentView;
        __weak typeof(self) weakSelf = self;
        contentView.actionClosure = ^(MessageContentViewAction *actionInfo) {
            [weakSelf handleContentViewAction:actionInfo];
        };
        _userKVO = [FBKVOController controllerWithObserver:self];
        [self makeSubViews];
    }
    return self;
}

- (void)makeSubViews {
    self.backgroundColor = [MessageViewConfig messageBackgroundColor];
    
    //头像
    WebImageView *avatarImageView = [[WebImageView alloc] init];
    [self addSubview:avatarImageView];
    _avatarImageView = avatarImageView;
    avatarImageView.layer.cornerRadius = [MessageViewConfig headerSize]/2;
    avatarImageView.layer.masksToBounds = true;
    
    [avatarImageView mas_makeConstraints:^(MASConstraintMaker *make) {
        if (self.isLeft) {
            make.left.equalTo(self.mas_left).offset([MessageViewConfig headerToBoundary]);
        } else {
            make.right.equalTo(self.mas_right).offset(-[MessageViewConfig headerToBoundary]);
        }
        make.top.equalTo(self.mas_top).offset([MessageViewConfig headerToTop]);
        make.width.equalTo(@([MessageViewConfig headerSize]));
        make.height.equalTo(@([MessageViewConfig headerSize]));
    }];
    
    UIView * contentView = self.contentView;
    if (!contentView) {
        contentView = [[UIView alloc] init];
    }
    [self addSubview:contentView];
    [contentView mas_makeConstraints:^(MASConstraintMaker *make) {
        if (self.isLeft) {
            make.left.equalTo(self.avatarImageView.mas_right).offset([MessageViewConfig headerToBubbleContainer]);
        } else {
            make.right.equalTo(self.avatarImageView.mas_left).offset(-[MessageViewConfig headerToBubbleContainer]);
        }
        make.top.equalTo(self.avatarImageView.mas_top).offset([MessageViewConfig headerTopToBubbleContainerTop]);
    }];
    if (!self.isLeft) {
        __weak typeof(self) weakSelf = self;
        MessageSendStateView * sendStatesView = [[MessageSendStateView alloc] initWithFrame:CGRectMake(0, 0, 30, 30)];
        [self addSubview:sendStatesView];
        sendStatesView.didResend = ^(MessageSendStateView * currentSendStatesView) {
            __strong typeof(weakSelf) strongSelf = weakSelf;
            if (strongSelf.actionClosure) {
                strongSelf.actionClosure([MessageContentViewAction actionWithType:MessageContentActionTypeResendMessage]);
            }
        };
        
        self.sendStatesView = sendStatesView;
        [sendStatesView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.right.equalTo(contentView.mas_left).offset(-[MessageViewConfig headerToBubbleContainer]);
            make.centerY.equalTo(contentView.mas_centerY);
            make.height.equalTo(@(40));
            make.width.equalTo(@(40));
        }];
    }
}

- (void)setNeedsUpdateFromMessage {
    self.sendStatesView.status = [self.message showStatus];
    
    [self.contentView setNeedsUpdateFromMessage];
}

- (void)configWith:(IMAMsg *)message conversation:(IMAConversation *)conversation {
    [super configWith:message conversation:conversation];
    
    [self.contentView configWithMessage:message conversation:conversation];
    self.sendStatesView.status = [self.message showStatus];
    [self refreshUIFromUserInfo];
}

- (void)refreshUIFromUserInfo {
    __weak typeof(self) weakSelf = self;
    UIImage * placeholderImage = [ImageUtil imageNamed:kstanceImage];
    NSURL *iconURL = nil;
    if ([self.message isMineMsg]) {
        iconURL = [NSURL URLWithString:[LoginHelper avatar]];
    } else {
        if ([self.sender.userId isEqualToString:[self.message getSender].userId]) {
            iconURL = [NSURL URLWithString:self.sender.icon];
            if (self.sender.isFishMail) {
                placeholderImage = [ImageUtil imageNamed:@"_friendlist_fishmail_icon"];
            }
        } else {
            if ([self.message getSender].isFishMail) {
                placeholderImage = [ImageUtil imageNamed:@"_friendlist_fishmail_icon"];
            }
            iconURL = [NSURL URLWithString:[self.message getSender].icon];
        }
    }
    
    self.nickLabel.text = [self.sender showTitle];
}


- (void)dealloc
{
    [self.userKVO unobserveAll];
    self.userKVO = nil;
}

@end




#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.viewModel.messageList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    IMAMsg *message = [self.viewModel.messageList objectAtIndex:indexPath.row];
    __weak typeof(self) weakSelf = self;

    ContentableMessageCell * cell = nil;
    switch (message.type) {
        case EIMAMSG_Text:
        {
            MessageCellType cellType = MessageCellTypeText;
            MessageShowStyle showStyle = MessageShowStyleNone;
            if (message) {
                if ([message isMineMsg]) {
                    showStyle = MessageShowStyleRight;
                } else {
                    showStyle = MessageShowStyleLeft;
                }
            }
            NSString * style = MessageShowStyleStringValue(showStyle);
            NSString * detailStyle = MessageCellTypeStringValue(cellType);
            NSString * identifier = [NSString stringWithFormat:@"%@-%@", style, detailStyle];
            cell = [tableView dequeueReusableCellWithIdentifier:identifier];
            if (!cell) {
                cell = [[C2CTextMessageCell alloc] initWithIdentifier:identifier style:showStyle];
            }
        }
            break;
        case EIMAMSG_Image:
        {
            MessageCellType cellType = MessageCellTypeImage;
            MessageShowStyle showStyle = MessageShowStyleNone;
            if (message) {
                if ([message isMineMsg]) {
                    showStyle = MessageShowStyleRight;
                } else {
                    showStyle = MessageShowStyleLeft;
                }
            }
            NSString * style = MessageShowStyleStringValue(showStyle);
            NSString * detailStyle = MessageCellTypeStringValue(cellType);
            NSString * identifier = [NSString stringWithFormat:@"%@-%@", style, detailStyle];
            cell = [tableView dequeueReusableCellWithIdentifier:identifier];
            if (!cell) {
                cell = [[C2CImageMessageCell alloc] initWithIdentifier:identifier style:showStyle];
            }
        }
            break;
        default:
            break;
    }
    if (cell) {
        cell.cellDelegate = self;
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
        [cell configWith:message conversation:self.conversation];
        return cell;
    } else {
        if (!isUnknownMessage) {
            LogError(@"message list error");
        }
        MessageCellType cellType = MessageCellTypeUnknown;
        MessageShowStyle showStyle = MessageShowStyleNone;
        if (message) {
            if ([message isMineMsg]) {
                showStyle = MessageShowStyleRight;
            } else {
                showStyle = MessageShowStyleLeft;
            }
        }
        NSString * style = MessageShowStyleStringValue(showStyle);
        NSString * detailStyle = MessageCellTypeStringValue(cellType);
        identifier = [NSString stringWithFormat:@"%@-%@", style, detailStyle];
        C2CUnknownMessageCell * cell = [tableView dequeueReusableCellWithIdentifier:identifier];
        if (!cell) {
            cell = [[C2CUnknownMessageCell alloc] initWithIdentifier:identifier style:showStyle];
        }
        cell.cellDelegate = self;
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
        if (message) {
            MessageContext * context = self.messageContext;
            context.otherUser = self.user;
            [cell configContext:context];
            [context clean];
        }
        [cell configWith:message conversation:self.conversation];
        return cell;
    }
}

- (void)prepareResend: (IMAMsg *) msg {
    __weak typeof(self) weakSelf = self;
    AlertController *alertview=[AlertController alertControllerWithTitle:nil message:@"重新发送该条消息？" preferredStyle:UIAlertControllerStyleAlert];
    // 设置按钮
    UIAlertAction *cancel=[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:nil];
    UIAlertAction *defult = [UIAlertAction actionWithTitle:@"重新发送" style:UIAlertActionStyleDefault handler:^(UIAlertAction * action) {
        [weakSelf reSend:msg];
    }];
    [alertview addAction:cancel];
    [alertview addAction:defult];
    
    [self presentViewController:alertview animated:YES completion:nil];
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    IMAMsg *message = [self.viewModel.messageList objectAtIndex:indexPath.row];
    CGFloat headerHeight = 0;
    CGFloat bodyHeight = 0;
    CGFloat footerHeight = 0;
    if ([message needsShowTimeTip]) {
        if (0 == indexPath.row) {
            headerHeight = 34;
        } else {
            headerHeight = 24;
        }
    }
    
    if (0 == indexPath.row) {
        message.isTopOne = true;
    } else {
        message.isTopOne = false;
    }
    switch (message.type) {
        case EIMAMSG_Text:
        {
            bodyHeight = [message cellHeight:false];
        }
            break;
        case EIMAMSG_TimeTip:
        {
            if (0 == indexPath.row) {
                bodyHeight = 34;
            } else {
                bodyHeight = 24;
            }
        }
            break;
        case EIMAMSG_Image:
        {
            //            CGSize size = [message imageSizeInWidth:[ImageTableViewCell imageViewMaxWidth] atMsg:message];
            CGSize size = [message messageLayout].contentSize;
            bodyHeight = size.height + 20 > 60 ? size.height + 20 : 60;
        }
            break;
        case EIMAMSG_Sound:
        {
            bodyHeight = 60;
        }
            break;
        default:
        {
            bodyHeight = [C2CUnknownMessageCell bodyHeightWithMessage:message];
            message.messageBodyHeight = bodyHeight;
        }
            break;
    }
    if ([message needsShowResult]) {
        footerHeight = 32;
    }
    message.messageHeaderHeight = headerHeight;
    message.messageBodyHeight = bodyHeight;
    return headerHeight + bodyHeight + footerHeight;
}
