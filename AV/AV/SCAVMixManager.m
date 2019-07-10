//
//  SCAVMixManager.m
//  AV
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "SCAVMixManager.h"

@implementation SCAVMixManager



+ (instancetype)shared {
    static SCAVMixManager * __shared;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __shared = [[SCAVMixManager alloc] init];
    });
    return __shared;
}

@end
