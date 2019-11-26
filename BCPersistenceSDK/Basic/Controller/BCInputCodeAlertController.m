//
//  BCInputCodeAlertController.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/8.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCInputCodeAlertController.h"
#import "BCUIKit.h"


@interface BCInputCodeView ()

@property (nonatomic, weak) BCLabel *titleLabel;
@property (nonatomic, weak) BCTextField *textField;
@property (nonatomic, weak) UIButton *cancelButton;
@property (nonatomic, weak) UIButton *okButton;

@end
@implementation BCInputCodeView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor whiteColor];
        
        BCLabel *titleLabel = [[BCLabel alloc] init];
        [self addSubview:titleLabel];
        self.titleLabel = titleLabel;
        titleLabel.numberOfLines = 1;
        titleLabel.textColor = [UIColor colorWithRed:55.0f/255.0f green:55.0f/255.0f blue:55.0f/255.0f alpha:1.0f];
        titleLabel.font = [UIFont systemFontOfSize:18.0f];
        [titleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.centerX.equalTo(self.mas_centerX);
            make.centerY.equalTo(self.mas_top).offset(34.0 / 249.0 * frame.size.height);
        }];
        
        
        BCTextField *textField = [[BCTextField alloc] init];
        [self addSubview:textField];
        textField.layer.borderColor = [[UIColor colorWithRed:255.0f/255.0f green:160.0f/255.0f blue:0.0f/255.0f alpha:1.0f] CGColor];
        textField.layer.borderWidth = 1.0 / [UIScreen mainScreen].scale;
        self.textField = textField;
        [textField mas_makeConstraints:^(MASConstraintMaker *make) {
            make.height.equalTo(@30);
            make.centerY.equalTo(self.mas_top).offset(138.0 / 249.0 * frame.size.height);
            make.left.equalTo(self.mas_left).offset(20);
            make.right.equalTo(self.mas_right).offset(-20);
        }];
        textField.keyboardType = UIKeyboardTypeASCIICapable;
        textField.secureTextEntry = true;
        textField.autocorrectionType = UITextAutocorrectionTypeNo;
        textField.autocapitalizationType = UITextAutocapitalizationTypeNone;
        textField.returnKeyType = UIReturnKeyDone;

        UIButton *cancelButton = [[UIButton alloc] init];
        [cancelButton setTitle:@"取消" forState:(UIControlStateNormal)];
        [cancelButton setTitleColor:[UIColor colorWithRed:255.0f/255.0f green:160.0f/255.0f blue:0.0f/255.0f alpha:1.0f] forState:(UIControlStateNormal)];
        cancelButton.titleLabel.font = [UIFont systemFontOfSize:16.0f];
        [self addSubview:cancelButton];
        
        self.cancelButton = cancelButton;
        [cancelButton mas_makeConstraints:^(MASConstraintMaker *make) {
            make.height.equalTo(@(46.0 / 249.0 * frame.size.height));
            make.bottom.equalTo(self.mas_bottom);
            make.left.equalTo(self.mas_left);
        }];

        
        UIButton *okButton = [[UIButton alloc] init];
        [okButton setTitle:@"确定" forState:(UIControlStateNormal)];
        [okButton setTitleColor:[UIColor colorWithRed:255.0f/255.0f green:160.0f/255.0f blue:0.0f/255.0f alpha:1.0f] forState:(UIControlStateNormal)];
        okButton.titleLabel.font = [UIFont systemFontOfSize:16.0f];
        [self addSubview:okButton];

        self.okButton = okButton;
        [okButton mas_makeConstraints:^(MASConstraintMaker *make) {
            make.height.equalTo(cancelButton.mas_height);
            make.bottom.equalTo(cancelButton.mas_bottom);
            make.left.equalTo(cancelButton.mas_right).offset(2);
            make.right.equalTo(self.mas_right);
            make.width.equalTo(cancelButton.mas_width);
        }];
        
        UIView *topLineView = [[UIView alloc] init];
        topLineView.backgroundColor = [UIColor colorWithRed:233.0f/255.0f green:233.0f/255.0f blue:233.0f/255.0f alpha:1.0f];
        topLineView.alpha = 1;
        [self addSubview:topLineView];
        [topLineView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.equalTo(self.mas_left);
            make.bottom.equalTo(cancelButton.mas_top);
            make.right.equalTo(self.mas_right);
            make.height.equalTo(@2);
        }];
        
        UIView *spaceLineView = [[UIView alloc] init];
        spaceLineView.backgroundColor = [UIColor colorWithRed:233.0f/255.0f green:233.0f/255.0f blue:233.0f/255.0f alpha:1.0f];
        spaceLineView.alpha = 1;
        [self addSubview:spaceLineView];
        [spaceLineView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.width.equalTo(@2);
            make.bottom.equalTo(cancelButton.mas_bottom);
            make.centerX.equalTo(self.mas_centerX);
            make.height.equalTo(cancelButton.mas_height);
        }];
        
        
    }
    return self;
}

- (void)setAlertTitle:(NSString *)alertTitle {
    _alertTitle = [alertTitle copy];
    self.titleLabel.text = _alertTitle;
}

@end



@interface BCInputCodeAlertController () <UITextFieldDelegate>
@property (nonatomic, strong) BCInputCodeView *inputCodeView;

@end

@implementation BCInputCodeAlertController

- (BCInputCodeView *)inputCodeView {
    if (nil == _inputCodeView) {
        CGFloat w = [BCUIUtil screenWidth] - 38 * 2;
        CGFloat h = w *249.0/300;
        _inputCodeView = [[BCInputCodeView alloc] initWithFrame:CGRectMake(38, ([BCUIUtil screenHeight] - h)/2 - 20, w, h)];
    }
    return _inputCodeView;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

    BCInputCodeView *inputCodeView = self.inputCodeView;
    [self.view addSubview:inputCodeView];
    [inputCodeView.cancelButton addTarget:self action:@selector(handleCancelAction:) forControlEvents:(UIControlEventTouchUpInside)];
    [inputCodeView.okButton addTarget:self action:@selector(handleOkAction:) forControlEvents:(UIControlEventTouchUpInside)];
    inputCodeView.textField.delegate = self;
}

- (void)handleCancelAction:(UIButton *)button {
    [self.inputCodeView.textField resignFirstResponder];
    
    [UIView animateWithDuration:0.25 animations:^{
        self.view.backgroundColor = [UIColor clearColor];
    } completion:^(BOOL finished) {
        if (finished) {
            [self dismissViewControllerAnimated:false completion:^{
                if (self.onCancel) {
                    self.onCancel(self);
                }
            }];
        }
    }];
    
}

- (void)handleOkAction:(UIButton *)button {
    NSString *text = self.inputCodeView.textField.text;
    if (self.willSuccess) {
        if (!self.willSuccess(self, text)) {
            return;
        }
    }
    
    [self.inputCodeView.textField resignFirstResponder];
    [UIView animateWithDuration:0.25 animations:^{
        self.view.backgroundColor = [UIColor clearColor];
    } completion:^(BOOL finished) {
        if (finished) {
            [self dismissViewControllerAnimated:false completion:^{
                if (self.onSuccess) {
                    self.onSuccess(self, text);
                }
            }];
        }
    }];
}

- (void)showAnimate:(BOOL)animate {
    [self showAnimate:animate completion:nil];
}

- (void)showAnimate:(BOOL)animate completion:(void (^)(void))completion {
    [[BCViewControllerUtil currentViewController] presentViewController:self animated:false completion:^{
        if (animate) {
            [UIView animateWithDuration:0.25 animations:^{
                self.view.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.6];
            } completion:^(BOOL finished) {
                if (finished) {
                    if (completion) {
                        completion();
                    }
                }
            }];
        } else {
            self.view.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.6];
            if (completion) {
                completion();
            }
        }
    }];
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
    if ([string isEqualToString:@"\n"]) {
        [self.inputCodeView.textField resignFirstResponder];
        return false;
    }
    return true;
}

@end


