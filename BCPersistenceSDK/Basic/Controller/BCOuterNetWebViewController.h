//
//  BCOuterNetWebViewController.h
//  BichanWallet
//
//  Created by 王青海 on 2018/9/10.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCBaseWebViewController.h"

@interface BCBaseUIWebViewController : BCViewController <UIWebViewDelegate>

@property (nonatomic, copy) NSURLRequest *URLRequest;


@end


@interface BCOuterNetWebViewController : BCBaseUIWebViewController

@end
