//
//  Cells.h
//  Message
//
//  Created by vector on 2019/6/27.
//  Copyright © 2019 vector. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Views.h"

NS_ASSUME_NONNULL_BEGIN

@class MessageBaseCell;
@class IMAMsg;
@class IMAConversation;
@class MessageBaseCellAction;


typedef NS_ENUM(NSUInteger, MessageContentActionType) {
    MessageContentActionTypeNone,
    MessageContentActionTypeHeaderTapped,
    MessageContentActionTypeResendMessage,
    MessageContentActionTypePlaySound,
    MessageContentActionTypeCopyText, // 长按事件
    MessageContentActionTypeRichTextAction,
    MessageContentActionTypeAnimateImageTapped,
    MessageContentActionTypeImageTapped, // 查看大图
    MessageContentActionTypeLiveRoom,
};


typedef NS_ENUM(NSUInteger, MessageBaseCellActionType) {
    MessageBaseCellActionTypeNone,
    MessageBaseCellActionTypeTapHead,
    MessageBaseCellActionTypeResendMessage,
    MessageBaseCellActionTypeCopyText,// 长按事件
    MessageBaseCellActionTypeRichTextAction,
    MessageBaseCellActionTypeImageTapped, // 查看大图
    MessageBaseCellActionTypeAnimateImageTapped, // 查看大图
    MessageBaseCellActionTypeGoLiveRoom,
};

typedef NS_ENUM(NSUInteger, MessageShowStyle) {
    MessageShowStyleNone = 0,
    MessageShowStyleLeft = 1,
    MessageShowStyleRight = 2,
};

typedef NS_ENUM(NSInteger, IMAMsgShowStatus)
{
    IMAMsgShowStatusSending,
    IMAMsgShowStatusSendFailure,       // 消息发送中
    IMAMsgShowStatusSendSuccess,    // 消息发送成功
};

typedef NS_ENUM(NSUInteger, MessageCellType) {
    MessageCellTypeNone = 0,
    MessageCellTypeText = 1,
    MessageCellTypeImage = 2,
    MessageCellTypeSound = 3,
    MessageCellTypePrepareSound = 4,
    MessageCellTypeUnknown = 0xffffffff,
};

@protocol MessageCellDelegate <NSObject>

- (BOOL)handleCellAction: (MessageBaseCellAction *)action;

@end

@interface MessageBaseCellAction : NSObject

@property (nonatomic, assign) MessageBaseCellActionType type;
@property (nonatomic, strong) MessageBaseCell * cell;
@property (nonatomic, copy) NSDictionary * info;

+ (instancetype)actionWithType: (MessageBaseCellActionType)type cell:(MessageBaseCell *)cell;

@end


@interface MessageBaseCell : UITableViewCell
@property (nonatomic, weak) id<MessageCellDelegate> cellDelegate;
@property (nonatomic, strong) IMAMsg * message;
@property (nonatomic, assign) MessageShowStyle style;//左边他人的消息，右边自己的消息，其他
- (instancetype)initWithIdentifier: (NSString *)identifier style: (MessageShowStyle)style;//初始化方法
- (void)configWith:(IMAMsg *)message conversation: (IMAConversation *)conversation;
- (BOOL)postAction: (MessageBaseCellAction *)action;
- (void)setNeedsUpdateFromMessage;
- (void)copyTextToPasetboard;
@end


#pragma mark - 带内容的消息cell ContentableMessageCell
@interface ContentableMessageCell : MessageBaseCell
{
    __weak TimeTipView * _topTimeTipView;
    __weak MessageContentView * _messageContentView;
    __weak MessageFooterView * _footerView;
}
@property (nonatomic, weak, readonly) TimeTipView * topTimeTipView;
@property (nonatomic, weak, readonly) MessageContentView * messageContentView;
@property (nonatomic, weak, readonly) MessageFooterView * footerView;
@property (nonatomic, strong) FBKVOController * msgKVO;

- (void)configTimeTipViewWith:(IMAMsg *)message conversation:(IMAConversation *)conversation;//<刷新时间标签>在调用super当前类实现给configWith: conversation: 后，调用该方法对topTimeTipView赋值，隐藏不隐藏操作.
- (void)configFooterViewWith:(IMAMsg *)message conversation:(IMAConversation *)conversation;//<刷新下方小纸条信息>在调用super当前类实现给configWith: conversation: 后，调用该方法对footerView赋值，隐藏不隐藏操作.
- (void)handleContentViewAction: (MessageContentViewAction *) actionInfo;

- (void)copyTextToPasetboard;


- (void)configWith:(IMAMsg *)message conversation:(IMAConversation *)conversation;

+ (MessageContentView *) makeContentViewWithStyle:(MessageShowStyle)style;
+ (MessageFooterView *) makeFooterViewWithStyle:(MessageShowStyle)style;
+ (TimeTipView *) makeTopTimeTipViewWithStyle:(MessageShowStyle)style;
@end


NS_ASSUME_NONNULL_END
