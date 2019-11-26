//
//  BCTableViewCell.h
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BCTableViewCell : UITableViewCell

@end


@interface BCLineStyleTableViewCell : BCTableViewCell

@property (nonatomic, weak, readonly) UIView *spaceline;

@end
