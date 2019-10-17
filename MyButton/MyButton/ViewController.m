//
//  ViewController.m
//  CustomButton
//
//  Created by vector on 2019/10/16.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"
#import "CustomButton.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self button0];
    [self button1];
    [self button2];
    [self button3];
}


- (void)button0 {
    CustomButton * button = [[CustomButton alloc] init];
    
    [self.view addSubview:button];
    button.translatesAutoresizingMaskIntoConstraints = NO;
    [button.leftAnchor constraintEqualToAnchor:self.view.leftAnchor].active = true;
    [button.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:100].active = true;    
    button.backgroundColor = UIColor.greenColor;
    [self doTest:button];
}

- (void)button1 {
    CustomButton * button = [[CustomButton alloc] init];
    
    [self.view addSubview:button];
    button.translatesAutoresizingMaskIntoConstraints = NO;
    [button.leftAnchor constraintEqualToAnchor:self.view.leftAnchor].active = true;
    [button.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:200].active = true;
    [button.widthAnchor constraintEqualToConstant:50].active = true;
    [button.heightAnchor constraintEqualToConstant:60].active = true;

    button.backgroundColor = UIColor.greenColor;
    [self doTest:button];
}


- (void)button2 {
    CustomButton * button = [[CustomButton alloc] init];
    
    [self.view addSubview:button];
    button.translatesAutoresizingMaskIntoConstraints = NO;
    [button.leftAnchor constraintEqualToAnchor:self.view.leftAnchor].active = true;
    [button.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:300].active = true;
    [button.heightAnchor constraintEqualToConstant:60].active = true;

    button.backgroundColor = UIColor.greenColor;
    [self doTest:button];
}
- (void)button3 {
    CustomButton * button = [[CustomButton alloc] init];
    
    [self.view addSubview:button];
    button.translatesAutoresizingMaskIntoConstraints = NO;
    [button.leftAnchor constraintEqualToAnchor:self.view.leftAnchor].active = true;
    [button.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:400].active = true;
    [button.heightAnchor constraintEqualToConstant:120].active = true;
    button.contentDirection = CustomButtonContentDirectionVertical;

    button.backgroundColor = UIColor.greenColor;
    
    [self doTest:button];
}

- (void)doTest:(CustomButton *)button {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        button.title = @"123123";
        NSLog(@"%@", button);
    });
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        button.title = @"123123asflka;s'f";
        NSLog(@"%@", button);
    });
    
    UIImage * image0 = [UIImage imageNamed:@"material_save_white"];
    UIImage * image1 = [UIImage imageNamed:@"schedule_timeline_bubble"];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        button.image = image0;
        NSLog(@"%@", button);
    });
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(7 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        button.image = image1;
        NSLog(@"%@", button);
    });
    
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(9 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        button.contentInsets = UIEdgeInsetsMake(5, 5, 0, 0);
        NSLog(@"%@", button);
    });
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(11 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        button.contentAlignment = CustomButtonAlignmentBothSizes;
        NSLog(@"%@", button);
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(13 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        button.contentStyle = CustomButtonStyleTitleAndImage;
        NSLog(@"%@", button);
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(15 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        button.lineSpacing = 30;
        NSLog(@"%@", button);
    });
}



@end
