//
//  BCAmountInputView.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/17.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCAmountInputView.h"
#import "BCTextView.h"
#import <Masonry/Masonry.h>
#import <BCNetworkSDK/BCNetworkSDK.h>


@interface BCAmountInputView ()

@end
@implementation BCAmountInputView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.textView.keyboardType = UIKeyboardTypeDecimalPad;
        _allowDecimalsInput = true;
    }
    return self;
}

- (void)setAllowDecimalsInput:(BOOL)allowDecimalsInput {
    if (_allowDecimalsInput != allowDecimalsInput) {
        _allowDecimalsInput = allowDecimalsInput;
        
        if (_allowDecimalsInput) {
            self.textView.keyboardType = UIKeyboardTypeDecimalPad;
        } else {
            self.textView.keyboardType = UIKeyboardTypeNumberPad;
        }
    }
}

#pragma mark - UITextViewDelegate
- (void)textViewDidChange:(UITextView *)textView {
    if ([textView.text hasPrefix:@"."]) {
        textView.text = [@"0" stringByAppendingString:textView.text];
    } else {
        NSString *text = textView.text;
        if (nil == text) {
            text = @"";
        }
        while (true) {
            if (![text hasPrefix:@"0."]) {
                if (text.length > 1) {
                    if ([text hasPrefix:@"0"]) {
                        text = [text substringFromIndex:1];
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        textView.text = text;
    }
    [super textViewDidChange:textView];
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text {
    if (text.length == 0 && range.length == 1) { // 删除操作
        return true;
    }
    
    NSString *string = textView.text;
    if (nil == string) {
        string = @"";
    }
    
    NSString *replacementText = text;
    if (nil == replacementText) {
        replacementText = @"";
    }
    NSString * toBeString = [string stringByReplacingCharactersInRange:range withString:replacementText];
    BOOL result = ([BCAmountInputView isValidInputs:toBeString maxIntegerNumberCount:(0 != self.integerNumberCount ? self.integerNumberCount : NSUIntegerMax) allowDecimalsInput:self.allowDecimalsInput maxDecimalsNumberCount:(0 != self.decimalsNumberCount ? self.decimalsNumberCount : NSUIntegerMax)]);
    if (result) {
        result = [super textView:textView shouldChangeTextInRange:range replacementText:text];
    }
    return result;
}



+ (BOOL)isValidInputs:(NSString *)input maxIntegerNumberCount:(NSUInteger)maxIntegerNumberCount allowDecimalsInput:(BOOL)allowDecimalsInput maxDecimalsNumberCount:(NSUInteger)maxDecimalsNumberCount {
    
    NSInteger decimalPointCount = 0;
    NSUInteger integerNumberCount = 0;
    NSUInteger decimalsNumberCount = 0;
    NSUInteger count = input.length;
    
    for (NSUInteger index=0; index<count; index++) {
        unichar c = [input characterAtIndex:index];
        
        BOOL in0To9 = (c >= '0' && c <= '9');
        BOOL isDecimalPoint = (c == '.');
        
        if (in0To9) {
            if (0 == decimalPointCount) {
                integerNumberCount ++;
            } else {
                decimalsNumberCount ++;
            }
        } else if (isDecimalPoint) {
            decimalPointCount ++;
            if (allowDecimalsInput) {
                if (decimalPointCount > 1) {
                    return false;
                }
            } else {
                if (decimalPointCount > 0) {
                    return false;
                }
            }
        } else {
            return false;
        }
    }

    return (integerNumberCount <= maxIntegerNumberCount && decimalsNumberCount <= maxDecimalsNumberCount);
}


@end
