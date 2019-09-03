//
//  SCTableViewSectionQuickHandler.m
//  SC_Lecture
//
//  Created by vector on 2019/9/3.
//  Copyright © 2019 Shituo Han. All rights reserved.
//

#import "SCTableViewSectionQuickHandler.h"

@implementation SCTableViewSectionQuickHandler

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (self.numberOfRows) {
        return self.numberOfRows(self.delegate, tableView, section);
    } else {
        return 0;
    }
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    if (self.cellForRow) {
        return self.cellForRow(self.delegate, tableView, indexPath);
    } else {
        return nil;
    }
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    if (self.heightForRow) {
        return self.heightForRow(self.delegate, tableView, indexPath);
    } else {
        return self.defaultCellHeight;
    }
}
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    return self.defaultHeaderHeight;
}
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    return self.defaultFooterHeight;
}
- (nullable UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section {
    if (self.headerViewForSection) {
        return self.headerViewForSection(self.delegate, tableView, section);
    } else {
        return nil;
    }
}
- (nullable UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section {
    if (self.footerViewForSection) {
        return self.footerViewForSection(self.delegate, tableView, section);
    } else {
        return nil;
    }
}
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    if (self.didSelectRow) {
        self.didSelectRow(self.delegate, tableView, indexPath);
    } else {
        
    }
}

@end
