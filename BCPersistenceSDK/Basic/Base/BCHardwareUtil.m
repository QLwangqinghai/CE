//
//  BCHardwareUtil.m
//  BichanWallet
//
//  Created by 王青海 on 2018/9/21.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCHardwareUtil.h"
#include <sys/sysctl.h>

@implementation BCHardwareUtil

+ (NSString *)machineModel {
    static dispatch_once_t one;
    static NSString *model;
    dispatch_once(&one, ^{
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char *machine = malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        model = [NSString stringWithUTF8String:machine];
        free(machine);
    });
    return model;
}

+ (BOOL)isSimulator {
    NSString *name = [BCHardwareUtil machineModel];
    return ([name isEqualToString:@"x86_64"] || [name isEqualToString:@"i386"]);
}


+ (BOOL)isJailBreak {
    // 模拟器的时候返回 NO
    if ([BCHardwareUtil isSimulator]) {
        return NO;
    }
    
    NSArray *filePaths = @[@"/Applications/Cydia.app",
                           @"/Library/MobileSubstrate/MobileSubstrate.dylib",
                           @"/bin/bash",
                           @"/usr/sbin/sshd",
                           @"/etc/apt"
                           ];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    // 判断是否存在越狱文件
    for (NSString *path in filePaths) {
        if ([fileManager fileExistsAtPath:path]) {
            return YES;
        }
    }
    // 判断是否存在cydia应用
    if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:@"cydia://"]]) {
        return YES;
    }
    // 读取系统所有的应用名称
    if ([fileManager fileExistsAtPath:@"/User/Applications/"]) {
        return YES;
    }
    // 读取环境变量
    if (getenv("DYLD_INSERT_LIBRARIES")) {
        return YES;
    }
    return NO;
}

@end
