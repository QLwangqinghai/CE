//
//  BCBaseInputView.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/20.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <UIKit/UIKit.h>

@class BCTextView;


@interface BCBaseTextInputFilter : NSObject

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text;

@end


@interface BCClosureTextInputFilter : BCBaseTextInputFilter

@property (nonatomic, copy) BOOL(^shouldChangeText)(UITextView *textView, NSRange range, NSString *replacementText);

@end

typedef NS_ENUM(NSInteger, BCBaseInputContentVerticalAlignment) {
    BCBaseInputContentVerticalAlignmentCenter  = 0,
    BCBaseInputContentVerticalAlignmentTop     = 1,
    BCBaseInputContentVerticalAlignmentBottom  = 2,
    BCBaseInputContentVerticalAlignmentFill    = 3,
};

//typedef NS_ENUM(NSInteger, UIControlContentHorizontalAlignment) {
//    UIControlContentHorizontalAlignmentCenter = 0,
//    UIControlContentHorizontalAlignmentLeft   = 1,
//    UIControlContentHorizontalAlignmentRight  = 2,
//    UIControlContentHorizontalAlignmentFill   = 3,
//    UIControlContentHorizontalAlignmentLeading  API_AVAILABLE(ios(11.0), tvos(11.0)) = 4,
//    UIControlContentHorizontalAlignmentTrailing API_AVAILABLE(ios(11.0), tvos(11.0)) = 5,
//};



/**
 金额输入框
 */
@interface BCBaseInputView : UIView <UITextViewDelegate>

@property (nonatomic, assign, readonly) CGRect contentFrame;

@property (nonatomic, assign) NSUInteger numberOfLines;
@property (nonatomic, assign) BCBaseInputContentVerticalAlignment contentVerticalAlignment;



@property (nonatomic, strong) NSString *text;
@property (nonatomic, assign, readonly) BOOL isEditing;

@property (nonatomic, strong, readonly) NSMutableArray <BCBaseTextInputFilter *>*inputFilters;

@property (nonatomic, weak, readonly) BCTextView *textView;


@property (nonatomic, copy) BOOL(^onInputReturn)(BCBaseInputView *inputView);
@property (nonatomic, copy) void(^onInputDidChange)(BCBaseInputView *inputView, NSString *text);
@property (nonatomic, copy) void(^onEndInput)(BCBaseInputView *inputView, NSString *text);
@property (nonatomic, copy) void(^onBeginInput)(BCBaseInputView *inputView, NSString *text);

- (void)beginEdit;
- (void)endEdit;

@end
