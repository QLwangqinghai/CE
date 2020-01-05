//
//  ViewController.m
//  UIBasic
//
//  Created by vector on 2019/12/31.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"
#import "UIBasic-swift.h"

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
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:true];
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:true];
            dispatch_async(dispatch_get_main_queue(), ^{
                [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:true];
                [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:true];
                [self.navigationController popViewControllerAnimated:true];

                [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:true];

            });
        });
    });
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:true];
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:true];
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(4 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:true];
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:false];
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(6 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:true];
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(7 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.navigationController pushViewController:[[BaseViewController alloc] init] animated:false];
    });
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(9 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSLog(@"9");
        
        [self.navigationController.viewControllers[4] backWithSender:self.navigationController.viewControllers[4] animate:true completion:^{
            NSLog(@"9 end");
        }];
//        [self.navigationController popToViewController:self.navigationController.viewControllers[4] animated:true];
    });
    
    [[BaseNavigationController alloc] initWithRootViewController:[[BaseViewController alloc] init]];
    
    
    [[BaseNavigationController alloc] init];

}


@end
