//
//  BCDeviceIdentifier.m
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/8/17.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCDeviceIdentifier.h"
#import "BCKeychainUtil.h"


@interface BCDeviceIdentifier ()

@property (nonatomic, copy) NSString *UUIDString;

@end
@implementation BCDeviceIdentifier


- (instancetype)init {
    self = [super init];
    if (self) {
        NSString *service = @"com.bichain.network.device";
        NSString *account = @"identifier";
        NSString *userDefaultKey = [NSString stringWithFormat:@"%@.%@", service, account];

        NSString *UUIDString = nil;
        id obj = [[NSUserDefaults standardUserDefaults] stringForKey:userDefaultKey];//取出数组
        if ([obj isKindOfClass:[NSString class]]) {
            UUIDString = obj;
        }
        
        if (UUIDString.length <= 0) {
            NSError *error = nil;
            UUIDString = [BCKeychainUtil passwordForService:service account:account error:&error];
            if (error.code == errSecInteractionNotAllowed) {//没有权限
                NSUUID *UUID = [NSUUID UUID];
                UUIDString = UUID.UUIDString;
            } else {
                if (UUIDString.length <= 0) {//查询失败
                    NSUUID *UUID = [NSUUID UUID];
                    UUIDString = UUID.UUIDString;
                    [BCKeychainUtil setPassword:UUIDString forService:service account:account error:&error];
                }
            }
            [[NSUserDefaults standardUserDefaults] setObject:UUIDString forKey:userDefaultKey];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }
        
        self.UUIDString = UUIDString;
    }
    return self;
}

+ (instancetype)sharedInstance {
    static BCDeviceIdentifier * __sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __sharedInstance = [[BCDeviceIdentifier alloc] init];
    });
    return __sharedInstance;
}

@end
