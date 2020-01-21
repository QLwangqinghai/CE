//
//  BCEnvConfig.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <Foundation/Foundation.h>

//@interface BCHTTPFieldInfo : NSObject
//
///**
// Whether created requests can use the device’s cellular radio (if present). `YES` by default.
// 
// @see NSMutableURLRequest -setAllowsCellularAccess:
// */
//@property (nonatomic, assign) BOOL allowsCellularAccess;
//
///**
// The cache policy of created requests. `NSURLRequestUseProtocolCachePolicy` by default.
// 
// @see NSMutableURLRequest -setCachePolicy:
// */
//@property (nonatomic, assign) NSURLRequestCachePolicy cachePolicy;
//
///**
// Whether created requests should use the default cookie handling. `YES` by default.
// 
// @see NSMutableURLRequest -setHTTPShouldHandleCookies:
// */
//@property (nonatomic, assign) BOOL HTTPShouldHandleCookies;
//
///**
// Whether created requests can continue transmitting data before receiving a response from an earlier transmission. `NO` by default
// 
// @see NSMutableURLRequest -setHTTPShouldUsePipelining:
// */
//@property (nonatomic, assign) BOOL HTTPShouldUsePipelining;
//
///**
// The network service type for created requests. `NSURLNetworkServiceTypeDefault` by default.
// 
// @see NSMutableURLRequest -setNetworkServiceType:
// */
//@property (nonatomic, assign) NSURLRequestNetworkServiceType networkServiceType;
//
///**
// The timeout interval, in seconds, for created requests. The default timeout interval is 60 seconds.
// 
// @see NSMutableURLRequest -setTimeoutInterval:
// */
//@property (nonatomic, assign) NSTimeInterval timeoutInterval;
//
//- (NSMutableDictionary *)allFields;
//
//@end


@interface BCEnvConfig : NSObject

@property (nonatomic, copy) NSString *tag;


//配置名
@property (nonatomic, readonly) NSString *name;

@property (nonatomic, copy) NSString *appID;

//主机地址 eg. "http://192.168.0.18:8081"
@property (nonatomic, copy) NSString *host;

//密钥seed
@property (nonatomic, copy) NSString *seed;


//h5 url
@property (nonatomic, strong, readonly) NSDictionary *URLStringInfo;

//其他透传给BCBaseRequest子类的info
@property (nonatomic, copy) NSDictionary *info;

//密钥seed 编码后的data
@property (nonatomic, readonly) NSData *seedData;


- (instancetype)initWithName:(NSString *)name;

- (instancetype)initWithName:(NSString *)name appID:(NSString *)appID host:(NSString *)host seed:(NSString *)seed NS_DESIGNATED_INITIALIZER;

- (NSString *)urlStringForKey:(id <NSCopying>)key;

- (void)setUrlString:(NSString *)urlString forKey:(id <NSCopying>)key;

- (void)removeAllUrl;

- (void)addUrlsFrom:(NSDictionary *)info;



+ (instancetype)configWithName:(NSString *)name appID:(NSString *)appID host:(NSString *)host seed:(NSString *)seed;

+ (instancetype)configWithName:(NSString *)name appID:(NSString *)appID host:(NSString *)host seed:(NSString *)seed builder:(void(^)(BCEnvConfig *config))builder;

+ (instancetype)configWithName:(NSString *)name builder:(void(^)(BCEnvConfig *config))builder;

@end
