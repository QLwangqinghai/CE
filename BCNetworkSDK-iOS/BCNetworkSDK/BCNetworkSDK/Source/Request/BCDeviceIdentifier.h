//
//  BCDeviceIdentifier.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/8/17.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BCDeviceIdentifier : NSObject

@property (nonatomic, copy, readonly) NSString *UUIDString;


+ (instancetype)sharedInstance;

@end
