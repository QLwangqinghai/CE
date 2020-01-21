//
//  BCNetworkManager.m
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCNetworkManager+Private.h"
#import "BCEnvConfig.h"
#import <UIKit/UIKit.h>
#import "BCNetworkSerializer.h"
#import "BCBaseRequest.h"
#import <Security/Security.h>
#include <sys/sysctl.h>
#import <CoreLocation/CoreLocation.h>
#import "BCNetworkErrorNotify.h"
#import "BCDeviceIdentifier.h"

@implementation BCDeviceInfo

- (instancetype)init {
    self = [super init];
    if (self) {
        
    }
    return self;
}

- (instancetype)initPrivate {
    self = [super init];
    if (self) {
        _osVersion = [[[UIDevice currentDevice] systemVersion] copy];
        _deviceName = [BCDeviceInfo machineModelName];
        _platform = 1;
        CGSize size = [UIScreen mainScreen].bounds.size;
        if (size.width < size.height) {
            size = CGSizeMake(size.height, size.width);
        }
        _screenSize = NSStringFromCGSize(size);
        
        _deviceId = [BCDeviceIdentifier sharedInstance].UUIDString;
    }
    return self;
}

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

+ (NSString *)machineModelName {
    static dispatch_once_t one;
    static NSString *name;
    dispatch_once(&one, ^{
        NSString *model = [self machineModel];
        if (!model) return;
        NSDictionary *dic = @{
                              @"Watch1,1" : @"Apple Watch",
                              @"Watch1,2" : @"Apple Watch",
                              
                              @"iPod1,1" : @"iPod touch 1",
                              @"iPod2,1" : @"iPod touch 2",
                              @"iPod3,1" : @"iPod touch 3",
                              @"iPod4,1" : @"iPod touch 4",
                              @"iPod5,1" : @"iPod touch 5",
                              @"iPod7,1" : @"iPod touch 6",
                              
                              @"iPhone1,1" : @"iPhone 1G",
                              @"iPhone1,2" : @"iPhone 3G",
                              @"iPhone2,1" : @"iPhone 3GS",
                              @"iPhone3,1" : @"iPhone 4 (GSM)",
                              @"iPhone3,2" : @"iPhone 4",
                              @"iPhone3,3" : @"iPhone 4 (CDMA)",
                              @"iPhone4,1" : @"iPhone 4S",
                              @"iPhone5,1" : @"iPhone 5",
                              @"iPhone5,2" : @"iPhone 5",
                              @"iPhone5,3" : @"iPhone 5c",
                              @"iPhone5,4" : @"iPhone 5c",
                              @"iPhone6,1" : @"iPhone 5s",
                              @"iPhone6,2" : @"iPhone 5s",
                              @"iPhone7,1" : @"iPhone 6 Plus",
                              @"iPhone7,2" : @"iPhone 6",
                              @"iPhone8,1" : @"iPhone 6s",
                              @"iPhone8,2" : @"iPhone 6s Plus",
                              @"iPhone8,4" : @"iPhone SE",
                              
                              @"iPad1,1" : @"iPad 1",
                              @"iPad2,1" : @"iPad 2 (WiFi)",
                              @"iPad2,2" : @"iPad 2 (GSM)",
                              @"iPad2,3" : @"iPad 2 (CDMA)",
                              @"iPad2,4" : @"iPad 2",
                              @"iPad2,5" : @"iPad mini 1",
                              @"iPad2,6" : @"iPad mini 1",
                              @"iPad2,7" : @"iPad mini 1",
                              @"iPad3,1" : @"iPad 3 (WiFi)",
                              @"iPad3,2" : @"iPad 3 (4G)",
                              @"iPad3,3" : @"iPad 3 (4G)",
                              @"iPad3,4" : @"iPad 4",
                              @"iPad3,5" : @"iPad 4",
                              @"iPad3,6" : @"iPad 4",
                              @"iPad4,1" : @"iPad Air",
                              @"iPad4,2" : @"iPad Air",
                              @"iPad4,3" : @"iPad Air",
                              @"iPad4,4" : @"iPad mini 2",
                              @"iPad4,5" : @"iPad mini 2",
                              @"iPad4,6" : @"iPad mini 2",
                              @"iPad4,7" : @"iPad mini 3",
                              @"iPad4,8" : @"iPad mini 3",
                              @"iPad4,9" : @"iPad mini 3",
                              @"iPad5,1" : @"iPad mini 4",
                              @"iPad5,2" : @"iPad mini 4",
                              @"iPad5,3" : @"iPad Air 2",
                              @"iPad5,4" : @"iPad Air 2",
                              @"iPad6,3" : @"iPad Pro (9.7 inch)",
                              @"iPad6,4" : @"iPad Pro (9.7 inch)",
                              @"iPad6,7" : @"iPad Pro (12.9 inch)",
                              @"iPad6,8" : @"iPad Pro (12.9 inch)",
                              
                              @"AppleTV2,1" : @"Apple TV 2",
                              @"AppleTV3,1" : @"Apple TV 3",
                              @"AppleTV3,2" : @"Apple TV 3",
                              @"AppleTV5,3" : @"Apple TV 4",
                              
                              @"i386" : @"Simulator x86",
                              @"x86_64" : @"Simulator x64",
                              };
        name = dic[model];
        if (!name) name = model;
    });
    return name;
}




- (void)updateStoreage:(NSMutableDictionary *)dict {
    dict[BCHeaderFieldKeyDeviceId] = [BCNetworkSerializer optionStringToString:self.deviceId];
    dict[BCHeaderFieldKeyImei] = [BCNetworkSerializer optionStringToString:self.imei];
    dict[BCHeaderFieldKeyImsi] = [BCNetworkSerializer optionStringToString:self.imsi];
    dict[BCHeaderFieldKeyMac] = [BCNetworkSerializer optionStringToString:self.mac];
    dict[BCHeaderFieldKeyPlatform] = @(self.platform);
    dict[BCHeaderFieldKeyOsVersion] = [BCNetworkSerializer optionStringToString:self.osVersion];
    dict[BCHeaderFieldKeyDeviceName] = [BCNetworkSerializer optionStringToString:self.deviceName];
    dict[BCHeaderFieldKeyScreenSize] = [BCNetworkSerializer optionStringToString:self.screenSize];
}
- (void)removeFromStoreage:(NSMutableDictionary *)dict {
    [dict removeObjectForKey:BCHeaderFieldKeyDeviceId];
    [dict removeObjectForKey:BCHeaderFieldKeyImei];
    [dict removeObjectForKey:BCHeaderFieldKeyImsi];
    [dict removeObjectForKey:BCHeaderFieldKeyMac];
    [dict removeObjectForKey:BCHeaderFieldKeyPlatform];
    [dict removeObjectForKey:BCHeaderFieldKeyOsVersion];
    [dict removeObjectForKey:BCHeaderFieldKeyDeviceName];
    [dict removeObjectForKey:BCHeaderFieldKeyScreenSize];
}

@end


@implementation BCNetworkSession
//- (void)updateStoreage:(NSMutableDictionary *)dict {
//    dict[BCHeaderFieldKeySessionType] = @(self.sessionType);
//    dict[BCHeaderFieldKeyUserId] = [BCNetworkSerializer optionStringToString:self.userId];
//    dict[BCHeaderFieldKeyToken] = [BCNetworkSerializer optionStringToString:self.token];
//}
//- (void)removeFromStoreage:(NSMutableDictionary *)dict {
//    [dict removeObjectForKey:BCHeaderFieldKeySessionType];
//    [dict removeObjectForKey:BCHeaderFieldKeyUserId];
//    [dict removeObjectForKey:BCHeaderFieldKeyToken];
//}
@end


@implementation BCDeviceStatus
- (BCDeviceStatusNetStatus)network {
    AFNetworkReachabilityStatus status = AFNetworkReachabilityManager.sharedManager.networkReachabilityStatus;
    switch (status) {
        case AFNetworkReachabilityStatusNotReachable:
            return BCDeviceStatusNetStatusDisable;
        case AFNetworkReachabilityStatusReachableViaWWAN:
            return BCDeviceStatusNetStatusWWan;
        case AFNetworkReachabilityStatusReachableViaWiFi:
            return BCDeviceStatusNetStatusWifi;
        case AFNetworkReachabilityStatusUnknown:
        default:
            return BCDeviceStatusNetStatusUnknown;
    }
}
- (NSString *)lbs {
    return @"";
}
- (CFTimeInterval)timestamp {
    return [NSDate date].timeIntervalSince1970;
}

- (void)updateStoreage:(NSMutableDictionary *)dict {
    dict[BCHeaderFieldKeyNetwork] = @(self.network);
    dict[BCHeaderFieldKeyLbs] = [BCNetworkSerializer optionStringToString:self.lbs];
    dict[BCHeaderFieldKeyNotificationStatus] = @(self.nofiticationStatus);
    dict[BCHeaderFieldKeyTimestamp] = @((uint64_t)(self.timestamp * 1000));
}
- (void)removeFromStoreage:(NSMutableDictionary *)dict {
    [dict removeObjectForKey:BCHeaderFieldKeyNetwork];
    [dict removeObjectForKey:BCHeaderFieldKeyLbs];
    [dict removeObjectForKey:BCHeaderFieldKeyNotificationStatus];
    [dict removeObjectForKey:BCHeaderFieldKeyTimestamp];
}
@end


@interface BCNetworkManager ()

@property (nonatomic, strong) NSMutableDictionary *baseHeaderStoreage;
@property (nonatomic, strong) NSMutableDictionary *configDict;

@property (nonatomic, strong) dispatch_queue_t baseHeaderStoreageUpdateQueue;

@property (nonatomic, strong) BCDeviceInfo *deviceInfo;
@property (nonatomic, strong) BCNetworkSession *networkSession;
@property (nonatomic, strong) BCDeviceStatus *deviceStatus;

@end

@implementation BCNetworkManager

- (instancetype)init {
    self = [super init];
    if (self) {
        _configDict = [NSMutableDictionary dictionary];
        _baseHeaderStoreage = [NSMutableDictionary dictionary];
        self.baseHeaderStoreageUpdateQueue = dispatch_queue_create("com.bichan.network.BCNetworkManager.baseHeaderStoreageUpdateQueue", NULL);
        
//        [[NSNotificationCenter defaultCenter] addObserver:self
//                                                 selector:@selector(handleApplicationDidBecomeActiveNotification:)
//                                                     name:UIApplicationDidBecomeActiveNotification
//                                                   object:nil];
        dispatch_async(dispatch_get_main_queue(), ^{
            [[AFNetworkReachabilityManager  sharedManager] startMonitoring];
        });
    }
    return self;
}

//- (void)handleApplicationDidBecomeActiveNotification:(NSNotification *)notify {
//}


#pragma mark - property
- (void)setDeviceInfo:(BCDeviceInfo *)deviceInfo {
    [self.deviceInfo removeFromStoreage:self.baseHeaderStoreage];
    _deviceInfo = deviceInfo;
    [self.deviceInfo updateStoreage:self.baseHeaderStoreage];
}

- (void)setNetworkSession:(BCNetworkSession *)networkSession {
    NSArray *allKeys = [_networkSession.allHttpKeys allObjects];
    for (id key in allKeys) {
        [self.baseHeaderStoreage removeObjectForKey:key];
    }
    _networkSession = networkSession;
    allKeys = [_networkSession.allHttpKeys allObjects];
    for (id key in allKeys) {
        id obj = [_networkSession.session objectForKey:key];
        if (obj) {
            [self.baseHeaderStoreage setObject:obj forKey:key];
        }
    }
}

//- (void)setDeviceStatus:(BCDeviceStatus *)deviceStatus {
//    [self.deviceStatus removeFromStoreage:self.baseHeaderStoreage];
//    _deviceStatus = deviceStatus;
//    [self.deviceStatus updateStoreage:self.baseHeaderStoreage];
//}

#pragma mark - API
- (void)addEnvConfig:(BCEnvConfig *)config {
    if (nil == config.name) {
        return;
    }
    [self.configDict setObject:config forKey:config.name];
}

- (void)removeEnvConfig:(BCEnvConfig *)config {
    if (nil == config.name) {
        return;
    }
    
    [self.configDict removeObjectForKey:config.name];
}

- (void)removeEnvConfigForName:(NSString *)name {
    if (nil == name) {
        return;
    }
    
    [self.configDict removeObjectForKey:name];
}

- (BCEnvConfig *)getEnvConfigForName:(NSString *)name {
    if (nil == name) {
        return nil;
    }
    return [self.configDict objectForKey:name];
}

- (NSDictionary *)getAllEnvConfigs {
    return [self.configDict copy];
}

- (void)updateNetworkSession:(BCNetworkSession *)session {
    [self updateBaseHeaderStoreage:^(NSMutableDictionary *info) {
        self.networkSession = session;
    }];
}
//- (void)updateNetworkDeviceStatus:(BCDeviceStatus *)status {
//    [self updateBaseHeaderStoreage:^(NSMutableDictionary *info) {
//        self.deviceStatus = status;
//    }];
//}



+ (instancetype)sharedInstance {
    static BCNetworkManager *__sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __sharedInstance = [[BCNetworkManager alloc] init];
        __sharedInstance.deviceInfo = [[BCDeviceInfo alloc] initPrivate];
        __sharedInstance.deviceStatus = [[BCDeviceStatus alloc] init];
    });
    return __sharedInstance;
}

@end


@implementation BCNetworkManager (Private)
- (void)updateBaseHeaderStoreage:(void(^)(NSMutableDictionary *info))handler {
    NSMutableDictionary *info = self.baseHeaderStoreage;
    dispatch_async(self.baseHeaderStoreageUpdateQueue, ^{
        if (handler) {
            handler(info);
        }
    });
}
- (NSMutableDictionary *)makeBCHeader {
    [self.deviceStatus updateStoreage:self.baseHeaderStoreage];
    return [self.baseHeaderStoreage mutableCopy];
}
- (void)putTailInfo:(NSMutableDictionary *)dict {
    NSArray *allKeys = [self.networkSession.allTailKeys allObjects];
    for (id key in allKeys) {
        id obj = [self.networkSession.session objectForKey:key];
        if (obj) {
            [dict setObject:obj forKey:key];
        }
    }
}
- (void)reportWithContext:(BCNetworkSerializerContext *)context error:(BCError *)error {
    if (BCRequestStatusFinishError == context.status) {
        if (error.code >= 1000 && error.code < 10000) {
            BCNetworkErrorNotify *notify = [[BCNetworkErrorNotify alloc] init];
            notify.error = error;
            notify.tailInfo = context.tailInfo;
            [[NSNotificationCenter defaultCenter] postNotificationName:BCNetworkNotifityNameGatewayError object:notify];
        }
    }
}

@end

BCHeaderFieldKey const BCHeaderFieldKeyDeviceId = @"deviceId";
BCHeaderFieldKey const BCHeaderFieldKeyImei = @"imei";
BCHeaderFieldKey const BCHeaderFieldKeyImsi = @"imsi";
BCHeaderFieldKey const BCHeaderFieldKeyMac = @"mac";
BCHeaderFieldKey const BCHeaderFieldKeyPlatform = @"platform";
BCHeaderFieldKey const BCHeaderFieldKeyOsVersion = @"osVersion";
BCHeaderFieldKey const BCHeaderFieldKeyDeviceName = @"deviceName";
BCHeaderFieldKey const BCHeaderFieldKeyScreenSize = @"screenSize";

BCHeaderFieldKey const BCHeaderFieldKeyNetwork = @"network";
BCHeaderFieldKey const BCHeaderFieldKeyLbs = @"lbs";
BCHeaderFieldKey const BCHeaderFieldKeyNotificationStatus = @"notificationStatus";
BCHeaderFieldKey const BCHeaderFieldKeyTimestamp = @"timestamp";


NSString * const BCNetworkNotifityNameGatewayError = @"BCNetworkNotifityNameGatewayError";

