//
//  BCRichLabel.m
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCRichLabel.h"
#import <Masonry/Masonry.h>


@interface BCRichLabel ()

@property (nonatomic, strong) UILongPressGestureRecognizer *longPressGestureRecognizer;

@end
@implementation BCRichLabel

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        YYLabel *label = [[YYLabel alloc] init];
        [self addSubview:label];
        self.label = label;
        
        
        UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGestureRecognizer:)];
        longPress.numberOfTouchesRequired = 1;
        longPress.minimumPressDuration = 0.5;
        [self addGestureRecognizer:longPress];
        self.longPressGestureRecognizer = longPress;
        longPress.enabled = false;
    }
    return self;
}

- (void)setModel:(BCRichModel *)model {
    _model = model;
    
    CGSize size = self.model.layout.textBoundingSize;
    CGRect frame = CGRectMake(self.labelOffset.x, self.labelOffset.y, size.width, size.height);
    self.label.textLayout = self.model.layout;
    self.label.frame = frame;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    CGSize size = self.model.layout.textBoundingSize;
    CGRect frame = CGRectMake(self.labelOffset.x, self.labelOffset.y, size.width, size.height);
    self.label.frame = frame;
}


- (void)setEnableCopy:(BOOL)enableCopy {
    if (_enableCopy != enableCopy) {
        _enableCopy = enableCopy;
        if (_enableCopy) {
            self.userInteractionEnabled = true;
        }
        self.longPressGestureRecognizer.enabled = _enableCopy;
    }
}

- (BOOL)canBecomeFirstResponder {
    return self.enableCopy;
}

- (BOOL)canPerformAction:(SEL)action withSender:(id)sender {
    BOOL result = (action == @selector(customCopy:));
    return result;
}

- (void)customCopy:(id)sender {
    UIPasteboard *pasteBoard = [UIPasteboard generalPasteboard];
    pasteBoard.string = self.model.stringForCopyed.length > 0? self.model.stringForCopyed : @"";
}

- (void)handleLongPressGestureRecognizer:(UILongPressGestureRecognizer *)recognizer {
    if (self.model.stringForCopyed.length <= 0) {
        return;
    }
    
    if (recognizer.state == UIGestureRecognizerStateBegan) {
        UIWindow *win = self.window;
        if (win) {
            [self becomeFirstResponder];
            UIMenuItem *copyLink = [[UIMenuItem alloc] initWithTitle:@"复制" action:@selector(customCopy:)];
            [[UIMenuController sharedMenuController] setMenuItems:@[copyLink]];
            [[UIMenuController sharedMenuController] setTargetRect:self.frame inView:self.superview];
            [[UIMenuController sharedMenuController] setMenuVisible:YES animated:YES];
        }
    }
}





@end
