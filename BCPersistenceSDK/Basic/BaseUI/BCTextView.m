//
//  BCTextView.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCTextView.h"


@interface BCTextViewPlaceholderView : UIView

@property (nonatomic, weak) BCTextView *textView;
@property (nonatomic, strong) NSMutableParagraphStyle *placeholderParagraphStyle;

@end


@implementation BCTextView
{
    BCTextViewPlaceholderView *_placeholderView;
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        //设置默认值
        _placeholderParagraphStyle = [[NSMutableParagraphStyle alloc] init];
        self.placeholderFont = [UIFont systemFontOfSize:14];
//        self.placeholderTextColor = [UIColor colorWithHexString_:@"#999999"];;
        self.placeholderOffset = CGPointMake(6, 8);
        
        _placeholderView = [[BCTextViewPlaceholderView alloc] init];
        _placeholderView.placeholderParagraphStyle = _placeholderParagraphStyle;
        _placeholderView.backgroundColor = [UIColor clearColor];
        _placeholderView.textView = self;
        [self addSubview:_placeholderView];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textDidChanged) name:UITextViewTextDidChangeNotification object:nil];

        
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    _placeholderView.frame = CGRectMake(0, 0, self.frame.size.width, 60);
}

- (void)dealloc{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UITextViewTextDidChangeNotification object:nil];
}

- (void)textDidChanged {
    if (self.text.length) {
        _placeholderView.hidden = YES;
    }else{
        _placeholderView.hidden = NO;
        [_placeholderView setNeedsDisplay];
    }
}

- (void)setContentSize:(CGSize)contentSize {
    [super setContentSize:contentSize];
    
    NSLog(@"setContentSize: %@", NSStringFromCGSize(contentSize));
}

@end


@implementation BCTextViewPlaceholderView
- (void)drawRect:(CGRect)rect {
    [super drawRect:rect];
    
    if (self.textView.text.length == 0) {
        NSMutableDictionary * dict = [NSMutableDictionary dictionaryWithCapacity:3];
        if (self.textView.placeholderTextColor) {
            dict[NSForegroundColorAttributeName] = self.textView.placeholderTextColor;
        }
        if (self.textView.placeholderFont) {
            dict[NSFontAttributeName] = self.textView.placeholderFont;
        }
        
        if (self.placeholderParagraphStyle) {
            dict[NSParagraphStyleAttributeName] = self.placeholderParagraphStyle;
        }
        
        [self.textView.placeholder drawInRect:CGRectMake(self.textView.placeholderOffset.x, self.textView.placeholderOffset.y, self.frame.size.width - self.textView.placeholderOffset.x*2, self.frame.size.height) withAttributes:dict];
    }
}
- (BOOL)pointInside:(CGPoint)point withEvent:(UIEvent *)event {
    return false;
}
@end







