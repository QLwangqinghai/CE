//
//  BCNetworkErrorNotify.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/8/15.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BCError.h"

@interface BCNetworkErrorNotify : NSObject

@property (nonatomic, strong) BCError *error;
@property (nonatomic, copy) NSDictionary *tailInfo;



@end
