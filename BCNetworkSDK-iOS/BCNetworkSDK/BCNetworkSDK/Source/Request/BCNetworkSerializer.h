//
//  BCNetworkSerializer.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AFNetworking.h>
#import "BCRequestStatus.h"

#define BCHeaderFieldCheckNumber @"bcCheck"

@class BCBaseRequest;

@protocol BCSerializerBaseContext <NSObject>

@property (nonatomic, copy) NSString *appID;
@property (nonatomic, copy) NSString *URLString;
@property (nonatomic, assign, readonly) uint64_t requestId;
@property (nonatomic, strong) BCBaseRequest *request;
@property (nonatomic, assign) BCRequestStatus status;

@property (nonatomic, strong) NSMutableDictionary *tailInfo;

@end

@protocol BCRequestSerializerContext <BCSerializerBaseContext>

//入参
@property (nonatomic, strong) NSMutableDictionary *requestHeader;
@property (nonatomic, copy) NSDictionary *requestInfo;
@property (nonatomic, copy, readonly) NSData *requestSeed;

//出参
@property (nonatomic, strong) NSData *requestBodyData;
@property (nonatomic, copy) NSDictionary *requestErrorInfo;
@property (nonatomic, copy) NSString *requestErrorMessage;

@end

@protocol BCResponseSerializerContext <BCSerializerBaseContext>

//入参
@property (nonatomic, strong) NSURLResponse *networkResponse;
@property (nonatomic, strong) id networkResponseObject;
@property (nonatomic, strong) NSError *networkError;

//出参
@property (nonatomic, copy) NSDictionary *responseHeader;
@property (nonatomic, strong) NSData *responseBodyData;
@property (nonatomic, copy, readonly) NSData *responseSeed;
@property (nonatomic, copy) NSDictionary *responseInfo;
@property (nonatomic, copy) NSDictionary *responseErrorInfo;
@property (nonatomic, copy) id resultObject;
@property (nonatomic, copy) NSDictionary *resultInfo;
@property (nonatomic, copy) NSArray *resultArray;
@property (nonatomic, copy) NSString *responseErrorMessage;

@end


@interface BCNetworkSerializerContext : NSObject <BCRequestSerializerContext, BCResponseSerializerContext>

@property (nonatomic, copy) NSString *appID;
@property (nonatomic, copy) NSString *URLString;
@property (nonatomic, assign, readonly) uint64_t requestId;
@property (nonatomic, strong) BCBaseRequest *request;
@property (nonatomic, assign) BCRequestStatus status;
@property (nonatomic, strong) NSMutableDictionary *tailInfo;

//入参
@property (nonatomic, strong) NSMutableDictionary *requestHeader;
@property (nonatomic, copy) NSDictionary *requestInfo;
@property (nonatomic, copy) NSData *requestSeed;

//出参
@property (nonatomic, strong) NSData *requestBodyData;
@property (nonatomic, copy) NSDictionary *requestErrorInfo;
@property (nonatomic, copy) NSString *requestErrorMessage;

//入参
@property (nonatomic, strong) NSURLResponse *networkResponse;
@property (nonatomic, strong) id networkResponseObject;
@property (nonatomic, strong) NSError *networkError;
@property (nonatomic, copy) NSDictionary *responseHeader;
@property (nonatomic, strong) NSData *responseBodyData;
@property (nonatomic, copy) NSData *responseSeed;

//出参
@property (nonatomic, copy) NSDictionary *responseInfo;
@property (nonatomic, copy) NSDictionary *responseErrorInfo;
@property (nonatomic, copy) id resultObject;
@property (nonatomic, copy) NSDictionary *resultInfo;
@property (nonatomic, copy) NSArray *resultArray;
@property (nonatomic, copy) NSString *responseErrorMessage;

- (instancetype)initWithRequest:(BCBaseRequest *)request;


@end


@protocol BCRequestSerializerProtocol <NSObject>

- (NSInteger)requestProcess:(id<BCRequestSerializerContext>)context;

@end


@protocol BCResponseSerializerProtocol <NSObject>

- (NSInteger)responseProcess:(id<BCResponseSerializerContext>)context;

@end


BOOL BCNetworkSerializerIsValidString(id object);

@interface BCNetworkSerializer : NSObject <BCRequestSerializerProtocol, BCResponseSerializerProtocol>

+ (instancetype)sharedInstance;


//工具方法


/**
 用于JSON 对象， 获取integerValue

 @param obj JSON对象(NSNumber, String有效， 其他返回0)
 @return integerValue
 */
+ (NSInteger)integerValue: (id)obj;

+ (NSString *)stringValue: (id)obj;

+ (NSArray *)arrayValue: (id)obj;

+ (NSDictionary *)dictionaryValue: (id)obj;

//@"" 或者 nil  => @""
+ (NSString *)optionStringToString: (NSString *)optionString;

+ (BOOL)isValidString:(id)object;


+ (NSString *)modelWithName:(NSString *)name json:(NSDictionary *)json;

@end


@interface BCNetworkBase64Serializer : NSObject <BCRequestSerializerProtocol, BCResponseSerializerProtocol>
@end

@interface BCNetworkAes128Serializer : NSObject <BCRequestSerializerProtocol, BCResponseSerializerProtocol>
@end

@interface BCNetworkCrc32Serializer : NSObject <BCRequestSerializerProtocol, BCResponseSerializerProtocol>
@end

@interface BCNetworkJSONSerializer : NSObject <BCRequestSerializerProtocol, BCResponseSerializerProtocol>
@end

@interface BCNetworkStatusSerializer : NSObject <BCResponseSerializerProtocol>
@end

@interface BCNetworkResultSerializer : NSObject <BCResponseSerializerProtocol>
@end

@interface BCNetworkResponseSerializer : NSObject <BCResponseSerializerProtocol>
@end

