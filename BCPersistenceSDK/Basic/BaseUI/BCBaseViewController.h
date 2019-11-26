//
//  BCBaseViewController.h
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCNavigationBarConfig.h"


/**
 VC 基类， 任何模块的VC 都必须继承自BCBaseViewController
 */
@interface BCBaseViewController : UIViewController

@property (nonatomic, strong, readonly) BCNavigationBarConfig *barConfig;


@end
