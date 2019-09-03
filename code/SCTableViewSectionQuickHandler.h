//
//  SCTableViewSectionQuickHandler.h
//  SC_Lecture
//
//  Created by vector on 2019/9/3.
//  Copyright © 2019 Shituo Han. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SCTableViewSectionHandleProtocol.h"

NS_ASSUME_NONNULL_BEGIN

@interface SCTableViewSectionQuickHandler : NSObject<SCTableViewSectionHandleProtocol>

@property (nonatomic, weak, nullable) id delegate;
@property (nonatomic, assign) CGFloat defaultCellHeight;
@property (nonatomic, assign) CGFloat defaultHeaderHeight;
@property (nonatomic, assign) CGFloat defaultFooterHeight;

@property (nonatomic, copy, nullable) NSInteger(^numberOfRows)(_Nullable id delegate, UITableView * tableView, NSInteger section);
@property (nonatomic, copy, nullable) UITableViewCell *(^cellForRow)(_Nullable id delegate, UITableView * tableView, NSIndexPath * indexPath);
@property (nonatomic, copy, nullable) CGFloat(^heightForRow)(_Nullable id delegate, UITableView * tableView, NSIndexPath * indexPath);
@property (nonatomic, copy, nullable) UIView * _Nullable (^headerViewForSection)(_Nullable id delegate, UITableView * tableView, NSInteger section);
@property (nonatomic, copy, nullable) UIView * _Nullable (^footerViewForSection)(_Nullable id delegate, UITableView * tableView, NSInteger section);
@property (nonatomic, copy, nullable) void(^didSelectRow)(_Nullable id delegate, UITableView * tableView, NSIndexPath * indexPath);

@end

NS_ASSUME_NONNULL_END
