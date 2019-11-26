//
//  BCScrollController.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/13.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCScrollController.h"
#import "BCScrollView.h"

@interface BCScrollController ()
@property (nonatomic, weak) BCScrollView *scrollView;
@property (nonatomic, weak) UIView *contentView;

@end

@implementation BCScrollController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    BCScrollView *scrollView = [[BCScrollView alloc] init];
//    scrollView.panGestureRecognizer.
    [self.view addSubview:scrollView];
    self.scrollView = scrollView;
    [scrollView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.view).insets(UIEdgeInsetsZero);
    }];
    scrollView.directionalLockEnabled = true;
    
    
    UIView *contentView = [[UIView alloc] init];
    [scrollView addSubview:contentView];
    self.contentView = contentView;
    [contentView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.equalTo(scrollView.mas_top);
        make.left.equalTo(self.view.mas_left);
        make.right.equalTo(self.view.mas_right);
        make.bottom.equalTo(scrollView.mas_bottom);
    }];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
