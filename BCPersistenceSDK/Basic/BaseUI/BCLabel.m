//
//  BCLabel.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCLabel.h"



@interface BCLabel ()

@property (nonatomic, strong) UILongPressGestureRecognizer *longPressGestureRecognizer;

@end
@implementation BCLabel

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.userInteractionEnabled = true;
        
        UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGestureRecognizer:)];
        longPress.numberOfTouchesRequired = 1;
        longPress.minimumPressDuration = 0.5;
        [self addGestureRecognizer:longPress];
        self.longPressGestureRecognizer = longPress;
        longPress.enabled = false;
    }
    return self;
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

- (void)setFontSize:(CGFloat)size {
    self.font = [UIFont systemFontOfSize:size];
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
    pasteBoard.string = self.text.length > 0 ? self.text : @"";
}

- (void)handleLongPressGestureRecognizer:(UILongPressGestureRecognizer *)recognizer {
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

