//
//  ViewController.m
//  UIBasic
//
//  Created by vector on 2019/12/31.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"
#import <pthread.h>

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    UIStackView * v = [[UIStackView alloc] initWithFrame:CGRectMake(100, 150, 150, 150)];
    [self.view addSubview:v];
    
    UIView * v1 = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 200, 100)];
    [v addSubview:v1];
    v1.backgroundColor = [UIColor redColor];
    
}


@end
