//
//  BCCustomAlertController.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/8.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCCustomAlertController.h"

@implementation BCCustomAlertController

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.modalPresentationStyle = UIModalPresentationOverFullScreen;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    self.view.backgroundColor = [UIColor clearColor];
}

@end
