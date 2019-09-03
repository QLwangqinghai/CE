//
//  SCTableViewSectionHandleProtocol.h
//  SC_Lecture
//
//  Created by vector on 2019/9/3.
//  Copyright © 2019 Shituo Han. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol SCTableViewSectionHandleProtocol <NSObject>

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath;
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section;
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section;
- (nullable UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section;
- (nullable UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section;
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath;

@end

NS_ASSUME_NONNULL_END
