//
//  BCNetworkManager.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BCRequestStatus.h"

typedef NSString * BCHeaderFieldKey;

FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyDeviceId;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyImei;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyImsi;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyMac;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyPlatform;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyOsVersion;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyDeviceName;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyScreenSize;

FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyNetwork;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyLbs;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyNotificationStatus;
FOUNDATION_EXPORT BCHeaderFieldKey const BCHeaderFieldKeyTimestamp;

@protocol BCNetworkBaseHeaderUpdateProtocol
- (void)updateStoreage:(NSMutableDictionary *)dict;
- (void)removeFromStoreage:(NSMutableDictionary *)dict;
@end

@interface BCDeviceInfo : NSObject <BCNetworkBaseHeaderUpdateProtocol>

@property (nonatomic, copy) NSString *deviceId;
@property (nonatomic, copy) NSString *imei;
@property (nonatomic, copy) NSString *imsi;
@property (nonatomic, copy) NSString *mac;
@property (nonatomic, assign) NSInteger platform;
@property (nonatomic, copy) NSString *osVersion;
@property (nonatomic, copy) NSString *deviceName;
@property (nonatomic, copy) NSString *screenSize;

@end


@interface BCNetworkSession : NSObject

@property (nonatomic, copy) NSDictionary *session;
@property (nonatomic, copy) NSSet *allHttpKeys;//需要放到httpbody 中的字段
@property (nonatomic, copy) NSSet *allTailKeys;//本地跟踪状态用的key

//@property (nonatomic, assign) NSInteger sessionType;
//@property (nonatomic, copy) NSString *userId;
//@property (nonatomic, copy) NSString *token;

@end


typedef NS_ENUM(NSUInteger, BCDeviceStatusNetStatus) {
    BCDeviceStatusNetStatusDisable = 0,
    BCDeviceStatusNetStatus2G = 2,
    BCDeviceStatusNetStatus3G = 3,
    BCDeviceStatusNetStatus4G = 4,
    BCDeviceStatusNetStatusWifi = 20,
    BCDeviceStatusNetStatusWWan = 21,
    BCDeviceStatusNetStatusUnknown = 22,
};

@interface BCDeviceStatus : NSObject <BCNetworkBaseHeaderUpdateProtocol>

@property (nonatomic, assign, readonly) BCDeviceStatusNetStatus network;
@property (nonatomic, copy, readonly) NSString *lbs;

//0 未知， 1 开 2关
@property (nonatomic, assign, readonly) NSInteger nofiticationStatus;
@property (nonatomic, assign, readonly) CFTimeInterval timestamp;

@end


@class BCEnvConfig;
@interface BCNetworkManager : NSObject

@property (nonatomic, strong, readonly) BCDeviceInfo *deviceInfo;

//设备的状态
@property (nonatomic, strong, readonly) BCDeviceStatus *deviceStatus;

@property (nonatomic, strong, readonly) BCNetworkSession *networkSession;

/**
 添加config

 @param config 如果已经存在config.name 相对应的config， 则会用新的config覆盖旧的config
 */
- (void)addEnvConfig:(BCEnvConfig *)config;


/**
 删除config

 @param config 要删除的config
 */
- (void)removeEnvConfig:(BCEnvConfig *)config;

/**
 删除config
 
 @param name 通过name删除
 */
- (void)removeEnvConfigForName:(NSString *)name;

- (BCEnvConfig *)getEnvConfigForName:(NSString *)name;

- (NSDictionary *)getAllEnvConfigs;

- (void)updateNetworkSession:(BCNetworkSession *)session;

+ (instancetype)sharedInstance;

@end

FOUNDATION_EXPORT NSString * const BCNetworkNotifityNameGatewayError;



