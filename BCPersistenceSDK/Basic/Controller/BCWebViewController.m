//
//  BCWebViewController.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/8.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCWebViewController.h"

#import "BCJSAPIProvider.h"

@interface BCWebViewController ()


@end

@implementation BCWebViewController



- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self.webView addJavascriptObject:[[BCJSAPIProvider alloc] init] namespace:nil];

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
