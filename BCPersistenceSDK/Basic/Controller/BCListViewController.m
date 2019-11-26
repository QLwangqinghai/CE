//
//  BCListViewController.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/10.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCListViewController.h"


@interface BCListViewController ()

@property (nonatomic, weak) BCTableView *tableView;

@end

@implementation BCListViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    BCTableView *tableView = [[BCTableView alloc] init];
    [self.view addSubview:tableView];
    self.tableView = tableView;
    [tableView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.view).insets(UIEdgeInsetsZero);
    }];
    
    tableView.dataSource = self;
    tableView.delegate = self;
    [tableView registerClass:[BCTableViewCell class] forCellReuseIdentifier:@"BCTableViewCell"];
}

- (void)reloadData {
    [self.tableView reloadData];
    
    if ([self isNoData]) {
        [self showNoDataViewToView:self.tableView];
    } else {
        [self hideNoDataView];
    }
}

- (BOOL)isNoData {
    return true;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    BCTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"BCTableViewCell" forIndexPath:indexPath];
    return cell;
}

#pragma mark - UITableViewDelegate

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 40.0;
}

@end
