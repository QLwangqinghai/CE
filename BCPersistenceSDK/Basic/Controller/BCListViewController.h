//
//  BCListViewController.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/10.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCViewController.h"
#import <BCNetworkSDK/BCNetworkSDK.h>

@interface BCListViewController : BCViewController <UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, weak, readonly) BCTableView *tableView;


- (void)reloadData;

- (BOOL)isNoData;



@end
