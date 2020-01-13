//
//  ViewController.m
//  UIT
//
//  Created by vector on 2020/1/13.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

    CGRect frame = CGRectZero;
    frame.origin.x = 10;
    frame.size.width = 10;
    frame.size.height = 10;

    CGRect frame2 = frame;
    frame2.origin.x += frame.size.width;
    frame2.size.width -= frame.size.width * 2;
    frame2.size.height = 10;
    BOOL result = CGRectEqualToRect(frame, frame2);
    ;
    NSLog(@"%@", @(result));
    
    CGRect r = CGRectNull;
    NSLog(@"%@", @(r.origin.x));
    NSLog(@"%@", NSStringFromCGRect(CGRectNull));

    UILabel * label = [[UILabel alloc] initWithFrame:CGRectMake(10, 300, 200, -50)];
    [self.view addSubview:label];
    label.text = @"label";
}


@end
