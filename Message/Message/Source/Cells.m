//
//  Cells.m
//  Message
//
//  Created by vector on 2019/6/27.
//  Copyright © 2019 vector. All rights reserved.
//

#import "Cells.h"

@implementation MessageBaseCellAction
+ (instancetype)actionWithType: (MessageBaseCellActionType)type cell:(MessageBaseCell *)cell {
    MessageBaseCellAction * action = [[MessageBaseCellAction alloc] init];
    action.type = type;
    action.cell = cell;
    return action;
}
@end

@implementation MessageBaseCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];
    
    // Configure the view for the selected state
}


- (instancetype)initWithIdentifier: (NSString *)identifier style: (MessageShowStyle)style {//初始化方法
    self = [super initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifier];
    if (self) {
        _style = style;
        self.contentView.backgroundColor = [MessageViewConfig messageBackgroundColor];
        self.selectionStyle = UITableViewCellSelectionStyleNone;
    }
    return self;
}

- (void)configWith:(IMAMsg *)message conversation: (IMAConversation *)conversation { //刷新cell，子类需要调用super，然后完成自己独有的部分数据刷新
    self.message = message;
}

- (BOOL)postAction: (MessageBaseCellAction *)action {
    __strong typeof(self.cellDelegate) cellDelegate = self.cellDelegate;
    if (cellDelegate) {
        BOOL handleResult = [cellDelegate handleCellAction:action];
        LogVerbose(@"cell: %@ postAction: %@ , result: %d", self, action, handleResult);
        return handleResult;
    } else {
        LogVerbose(@"cell: %@ postAction: %@ faild.", self, action);
        return false;
    }
}
- (void)setNeedsUpdateFromMessage {
    
}

- (void)copyTextToPasetboard {
    
}

- (void)configContext: (MessageContext *)context {
    
}

@end






#pragma mark - 带内容的消息cell ContentableMessageCell
@interface ContentableMessageCell ()

@property (nonatomic, weak) TimeTipView * topTimeTipView;
@property (nonatomic, weak) MessageContentView * messageContentView;
@property (nonatomic, weak) MessageFooterView * footerView;
@end

@implementation ContentableMessageCell

- (instancetype)initWithIdentifier:(NSString *)identifier style:(MessageShowStyle)style {
    self = [super initWithIdentifier:identifier style:style];
    if (self) {
        Class selfClass = [self class];
        
        UIView * lastView = nil;
        __weak typeof (self)weakSelf = self;
        TimeTipView * topTimeTipView = [selfClass makeTopTimeTipViewWithStyle:style];
        if (topTimeTipView) {
            [self.contentView addSubview:topTimeTipView];
            self.topTimeTipView = topTimeTipView;
            [topTimeTipView mas_makeConstraints:^(MASConstraintMaker *make) {
                make.left.equalTo(self.contentView.mas_left);
                make.top.equalTo(self.contentView.mas_top);
                make.right.equalTo(self.contentView.mas_right);
                make.height.equalTo(@0);
            }];
            lastView = topTimeTipView;
        }
        
        MessageContentView * messageContentView = [selfClass makeContentViewWithStyle:style];
        if (messageContentView) {
            [self.contentView addSubview:messageContentView];
            self.messageContentView = messageContentView;
            messageContentView.actionClosure = ^(MessageContentViewAction *actionInfo) {
                [weakSelf handleContentViewAction:actionInfo];
            };
            [messageContentView mas_makeConstraints:^(MASConstraintMaker *make) {
                make.left.equalTo(self.contentView.mas_left);
                if (lastView) {
                    make.top.equalTo(lastView.mas_bottom);
                } else {
                    make.top.equalTo(self.contentView.mas_top);
                }
                make.right.equalTo(self.contentView.mas_right);
                make.height.equalTo(@0);
            }];
            lastView = messageContentView;
        }
        
        MessageFooterView * footerView = [selfClass makeFooterViewWithStyle:style];
        if (footerView) {
            [self.contentView addSubview:footerView];
            self.footerView = footerView;
            [footerView mas_makeConstraints:^(MASConstraintMaker *make) {
                make.left.equalTo(self.contentView.mas_left);
                if (lastView) {
                    make.top.equalTo(lastView.mas_bottom);
                } else {
                    make.top.equalTo(self.contentView.mas_top);
                }
                make.right.equalTo(self.contentView.mas_right);
                make.height.equalTo(@0);
                //                make.bottom.equalTo(self.contentView.mas_bottom);
            }];
            lastView = footerView;
        }
    }
    return self;
}

- (void)configWith:(IMAMsg *)message conversation:(IMAConversation *)conversation {
    if (nil == message) {
        NSLog(@"message  nil");
    }
    
    [super configWith:message conversation:conversation];
    [self.messageContentView mas_updateConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(message.messageBodyHeight));
    }];
    [self.messageContentView configWith:message conversation:conversation];
    [self configContentViewSizeWith:message conversation:conversation];
    [self configTimeTipViewWith:message conversation:conversation];
    [self configFooterViewWith:message conversation:conversation];
    
    [self setNeedsLayout];
}

- (void)configContentViewSizeWith:(IMAMsg *)message conversation:(IMAConversation *)conversation {
    [self.messageContentView mas_updateConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(message.messageBodyHeight));
    }];
}

- (void)configTimeTipViewWith:(IMAMsg *)message conversation:(IMAConversation *)conversation {
    if (message.needsShowTimeTip) {
        if (message.isTopOne) {
            [self.topTimeTipView mas_updateConstraints:^(MASConstraintMaker *make) {
                make.height.equalTo(@34);
            }];
        } else {
            [self.topTimeTipView mas_updateConstraints:^(MASConstraintMaker *make) {
                make.height.equalTo(@24);
            }];
        }
        self.topTimeTipView.hidden = false;
        [self.topTimeTipView setTimeString:[NSDate messageDate:[message msgDate]]];
    } else {
        if (message.isTopOne) {
            [self.topTimeTipView mas_updateConstraints:^(MASConstraintMaker *make) {
                make.height.equalTo(@10);
            }];
        } else {
            [self.topTimeTipView mas_updateConstraints:^(MASConstraintMaker *make) {
                make.height.equalTo(@0);
            }];
        }
        self.topTimeTipView.hidden = true;
        [self.topTimeTipView setTimeString:@""];
    }
}
- (void)configFooterViewWith:(IMAMsg *)message conversation:(IMAConversation *)conversation {
    if ([message isMineMsg]) {
        if ([message needsShowResult]) {//有小纸条
            [self.footerView mas_updateConstraints:^(MASConstraintMaker *make) {
                make.height.equalTo(@32);
            }];
            self.footerView.hidden = false;
            [self.footerView setTipString: [message.expandModel.sendResultModel descriptionStringWithMessageType:message.type]];
        } else {//没有
            [self.footerView mas_updateConstraints:^(MASConstraintMaker *make) {
                make.height.equalTo(@0);
            }];
            self.footerView.hidden = true;
        }
    } else {//没有
        [self.footerView mas_updateConstraints:^(MASConstraintMaker *make) {
            make.height.equalTo(@0);
        }];
        self.footerView.hidden = true;
    }
}

- (void)handleContentViewAction: (MessageContentViewAction *) actionInfo {
    NSLog(@"unhandled contentview action: %@ type: %ld info: %@", actionInfo, actionInfo.type, actionInfo.info);
}

- (void)copyTextToPasetboard {
    
    [self.messageContentView copyTextToPasetboard:self.message];
}

- (void)configKVO
{
    [_msgKVO unobserveAll];
    
}
- (void)dealloc
{
    [_msgKVO unobserveAll];
}

+ (MessageContentView *) makeContentViewWithStyle:(MessageShowStyle)style {
    return [[MessageContentView alloc] initWithFrame:CGRectZero];
}
+ (MessageFooterView *) makeFooterViewWithStyle:(MessageShowStyle)style {
    return [[MessageFooterView alloc] initWithFrame:CGRectZero];
}
+ (TimeTipView *) makeTopTimeTipViewWithStyle:(MessageShowStyle)style {
    return [[TimeTipView alloc] initWithFrame:CGRectZero];
}
@end
