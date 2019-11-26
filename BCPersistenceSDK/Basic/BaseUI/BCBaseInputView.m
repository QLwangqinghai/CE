//
//  BCBaseInputView.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/20.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCBaseInputView.h"
#import "BCTextView.h"
#import <Masonry/Masonry.h>
#import <BCNetworkSDK/BCNetworkSDK.h>



@interface BCBaseTextInputFilter ()

@end
@implementation BCBaseTextInputFilter


- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text {
    return true;
}

@end


@interface BCClosureTextInputFilter ()

@end
@implementation BCClosureTextInputFilter

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text {
    BOOL result = true;
    if (self.shouldChangeText) {
        result = self.shouldChangeText(textView, range, text);
    }
    if (result) {
        result = [super textView:textView shouldChangeTextInRange:range replacementText:text];
    }
    return result;
}

@end


void * BCBaseInputViewKVOContext = &BCBaseInputViewKVOContext;

@interface BCBaseInputView ()

@property (nonatomic, assign) BOOL isEditing;
@property (nonatomic, weak) BCTextView *textView;
@property (nonatomic, strong) NSMutableArray <BCBaseTextInputFilter *>*inputFilters;

@property (nonatomic, assign) int64_t currentLineNum;

@end
@implementation BCBaseInputView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        _inputFilters = [NSMutableArray array];
        BCTextView *textView = [[BCTextView alloc] init];
        [self addSubview:textView];
        textView.textContainerInset = UIEdgeInsetsZero;
        self.textView = textView;
        textView.showsHorizontalScrollIndicator = false;
        textView.delegate = self;
        textView.textAlignment = NSTextAlignmentLeft;
//        [textView mas_makeConstraints:^(MASConstraintMaker *make) {
//            make.left.equalTo(self.mas_left);
//            make.right.equalTo(self.mas_right);
//            make.centerY.equalTo(self.mas_centerY).offset(1);
//            make.height.equalTo(@22);
//        }];
        
        UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTapped:)];
        [self addGestureRecognizer:tap];
        
        [textView addObserver:self forKeyPath:@"contentSize" options:(NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld) context:BCBaseInputViewKVOContext];
    }
    return self;
}

- (void)dealloc {
    [self.textView removeObserver:self forKeyPath:@"contentSize" context:BCBaseInputViewKVOContext];
}

- (NSString *)text {
    return self.textView.text;
}

- (void)setText:(NSString *)text {
    self.textView.text = text;
}

- (void)handleTapped:(UITapGestureRecognizer *)tap {
    [self.textView becomeFirstResponder];
}

- (void)refresh:(BCTextView *)textView {
    
//      计算输入框的高度
//    CGFloat lineSpace = 1;//    行间距
//
//    CGRect textFrame = [[textView layoutManager] usedRectForTextContainer:[textView textContainer]];
//    CGSize contentSize = textFrame.size;
//
//    NSInteger numLines = ceil(contentSize.height / textView.font.lineHeight);
//
//    if (numLines == self.currentLineNum) {
//        return;
//    }
//    self.currentLineNum = numLines;
//    //  当输入超过三行，则不再增加输入框的高度
//    if (numLines > 1){
//        numLines = 3;
//    }
//
//    CGFloat textViewHeight = 0;
//    if (numLines == 1) {
//        textViewHeight = 36;
//    }
//    else if (numLines == 5) {
//        textViewHeight = (19 + lineSpace) * numLines - 3;
//    }
//    else {
//        textViewHeight = (19 + lineSpace) * numLines;
//    }
//    self.currentInputHeight = textViewHeight;
//
//    [self.textView mas_updateConstraints:^(MASConstraintMaker *make) {
//        make.height.equalTo(@(textViewHeight));
//    }];
//    if (numLines < 3) {
//        [textView scrollRangeToVisible:NSMakeRange(0,0)];
//    }
//    if (self.menuStates != DYMMessageInputMenuStatusAudio) {
//
//        //  重置输入框的高度
//        __weak __typeof(self)weakSelf = self;
//        [UIView animateWithDuration:0 animations:^{
//            DYMMessageInputMenuLayout * layout = [self makeLayout];
//            [self refreshSubLayoutWithLayout:layout];
//            [self refreshFrameWithLayout:layout];
//            [weakSelf layoutIfNeeded];
//        } completion:^(BOOL finished) {
//            if (self.resetRectBlock) {
//                self.resetRectBlock();
//            }
//        }];
//    }
    
    
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    [self refreshTextViewFrame];
}

- (void)refreshTextViewFrame {
    CGRect bounds = self.bounds;
    
    CGRect frame = bounds;
    CGSize contentSize = self.textView.contentSize;
    
    frame.size.height = contentSize.height < frame.size.height - 16 ? contentSize.height : frame.size.height - 16;
    frame.size.height = frame.size.height >= 16 ? frame.size.height : 16;
    
    if (self.contentVerticalAlignment == BCBaseInputContentVerticalAlignmentCenter) {
        frame.origin.y = (bounds.size.height - frame.size.height)/2;
    } else {
        frame.origin.y = 0;
    }
    
    self.textView.frame = frame;
    
}

#pragma mark - UITextViewDelegate
- (void)textViewDidChange:(UITextView *)textView {
    if (self.onInputDidChange) {
        self.onInputDidChange(self, textView.text);
    }
    
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text {
    if (self.onInputReturn && [text isEqualToString:@"\n"]) {
        return self.onInputReturn(self);
    }
    return true;
}

- (void)textViewDidBeginEditing:(UITextView *)textView {
    self.isEditing = true;
    BCLogVerbose(@" textViewDidBeginEditing isFist: %@", @(self.textView.isFirstResponder));
    if (self.onBeginInput) {
        self.onBeginInput(self, textView.text);
    }
}

- (void)textViewDidEndEditing:(UITextView *)textView {
    self.isEditing = false;
    BCLogVerbose(@" textViewDidEndEditing   isFist: %@", @(self.textView.isFirstResponder));
    if (self.onEndInput) {
        self.onEndInput(self, textView.text);
    }
}

- (void)beginEdit {
    [self.textView becomeFirstResponder];
}

- (void)endEdit {
    [self.textView resignFirstResponder];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
    if (context == BCBaseInputViewKVOContext) {
        if ([keyPath isEqualToString:@"contentSize"]) {
            if (object == self.textView) {
                [self refreshTextViewFrame];
            }
        }
    } else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}

@end
