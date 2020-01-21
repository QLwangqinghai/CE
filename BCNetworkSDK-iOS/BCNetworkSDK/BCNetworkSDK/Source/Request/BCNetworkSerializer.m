//
//  BCNetworkSerializer.m
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCNetworkSerializer.h"
#import "BCNetworkCrypto.h"
#import "BCNetworkError.h"
#import "BCBaseRequest.h"

@interface BCNetworkSerializerContext ()
{
    uint64_t _requestId;
}

@end
@implementation BCNetworkSerializerContext

- (instancetype)init {
    self = [super init];
    if (self) {
        _requestId = [BCNetworkSerializerContext p_makeId];
        _tailInfo = [NSMutableDictionary dictionary];
    }
    return self;
}
- (uint64_t)requestId {
    return _requestId;
}

+ (uint32_t)p_makeId {
    static volatile uint32_t requestId = 0;
    requestId ++;
    return requestId;
}

- (instancetype)initWithRequest:(BCBaseRequest *)request {
    self = [self init];
    if (self) {
        _request = request;
    }
    return self;
}

@end

BOOL BCNetworkSerializerIsValidString(id object) {
    if ([object isKindOfClass:[NSString class]]) {
        return [(NSString*)object length] > 0;
    } else {
        return false;
    }
}

@interface BCNetworkSerializer ()

@property (nonatomic, strong) NSMutableArray *requestSerializerItems;
@property (nonatomic, strong) NSMutableArray *responseSerializerItems;

@end
@implementation BCNetworkSerializer

- (instancetype)init {
    self = [super init];
    if (self) {
        _requestSerializerItems = [NSMutableArray array];
        _responseSerializerItems = [NSMutableArray array];
    }
    return self;
}

- (NSInteger)requestProcess:(id<BCRequestSerializerContext>)context {
    NSArray *items = self.requestSerializerItems;
    NSInteger result = 0;
    if (context.appID) {
        context.requestHeader[@"bcAppID"] = context.appID;
    }
    for (id<BCRequestSerializerProtocol> item in items) {
        result = [item requestProcess:context];
        if (result != 0) {
            break;
        }
    }
    return result;
}

- (NSInteger)responseProcess:(id<BCResponseSerializerContext>)context {
    NSArray *items = self.responseSerializerItems;
    NSInteger result = 0;
    for (id<BCResponseSerializerProtocol> item in items) {
        result = [item responseProcess:context];
        if (result != 0) {
            break;
        }
    }
    return result;
}


+ (instancetype)sharedInstance {
    static BCNetworkSerializer *__sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __sharedInstance = [[BCNetworkSerializer alloc] init];
        
        [__sharedInstance.requestSerializerItems addObject:[[BCNetworkJSONSerializer alloc] init]];
        [__sharedInstance.requestSerializerItems addObject:[[BCNetworkAes128Serializer alloc] init]];
        [__sharedInstance.requestSerializerItems addObject:[[BCNetworkCrc32Serializer alloc] init]];
        [__sharedInstance.requestSerializerItems addObject:[[BCNetworkBase64Serializer alloc] init]];

        [__sharedInstance.responseSerializerItems addObject:[[BCNetworkResponseSerializer alloc] init]];
        [__sharedInstance.responseSerializerItems addObject:[[BCNetworkBase64Serializer alloc] init]];
        [__sharedInstance.responseSerializerItems addObject:[[BCNetworkCrc32Serializer alloc] init]];
        [__sharedInstance.responseSerializerItems addObject:[[BCNetworkAes128Serializer alloc] init]];
        [__sharedInstance.responseSerializerItems addObject:[[BCNetworkJSONSerializer alloc] init]];
        [__sharedInstance.responseSerializerItems addObject:[[BCNetworkStatusSerializer alloc] init]];
        [__sharedInstance.responseSerializerItems addObject:[[BCNetworkResultSerializer alloc] init]];

    });
    
    return __sharedInstance;
}


+ (NSInteger)integerValue: (id)obj {
    if ([obj isKindOfClass:[NSNumber class]]) {
        return [(NSNumber *)obj integerValue];
    } else if ([obj isKindOfClass:[NSString class]]) {
        return [(NSString *)obj integerValue];
    }
    return 0;
}

+ (NSString *)stringValue: (id)obj {
    if ([obj isKindOfClass:[NSNumber class]]) {
        return [NSString stringWithFormat:@"%@", obj];
    } else if ([obj isKindOfClass:[NSString class]]) {
        return obj;
    } else {
        return [NSString stringWithFormat:@"%@", obj];
    }
}
+ (NSArray *)arrayValue: (id)obj {
    if ([obj isKindOfClass:[NSArray class]]) {
        return obj;
    }
    return nil;
}
+ (NSDictionary *)dictionaryValue: (id)obj {
    if ([obj isKindOfClass:[NSDictionary class]]) {
        return obj;
    }
    return nil;
}
+ (NSString *)optionStringToString: (NSString *)optionString {
    return nil != optionString ? optionString : @"";
}
+ (BOOL)isValidString:(id)object {
    return BCNetworkSerializerIsValidString(object);
}
+ (NSString *)modelWithName:(NSString *)name json:(NSDictionary *)json {
    
    return nil;
}

@end


@implementation BCNetworkBase64Serializer

- (NSInteger)requestProcess:(id<BCRequestSerializerContext>)context {
    if (nil == context.requestBodyData) {
        return BCNetworkErrorRequestBodyEncode;
    }

    context.requestBodyData = [BCNetworkCrypto base64EncodeToData:context.requestBodyData];
    if (nil == context.requestBodyData) {
        return BCNetworkErrorRequestBodyEncode;
    }
    return 0;
}
- (NSInteger)responseProcess:(id<BCResponseSerializerContext>)context {
    if (nil == context.responseBodyData) {
        return BCNetworkErrorResponseBodyDecode;
    }
    context.responseBodyData = [BCNetworkCrypto base64DecodeFromData:context.responseBodyData];
    if (nil == context.responseBodyData) {
        return BCNetworkErrorResponseBodyDecode;
    }
    return 0;
}

@end



@implementation BCNetworkAes128Serializer

- (NSInteger)requestProcess:(id<BCRequestSerializerContext>)context {
    if (nil == context.requestBodyData) {
        return BCNetworkErrorRequestBodyEncrypt;
    }
    
    BCAes128Key_t key = {};
    
    if (0 == BCNetworkCryptoGenerateAesKey(context.requestSeed, &key)) {
        context.requestBodyData = [BCNetworkCrypto aes128ECBEncrypt:context.requestBodyData withKey:&key];
        memset(&key, 0, sizeof(BCAes128Key_t));
        if (nil == context.requestBodyData) {
            return BCNetworkErrorRequestBodyEncrypt;
        }
    } else {
        return BCNetworkErrorRequestBodyEncrypt;
    }
    
    return 0;
}
- (NSInteger)responseProcess:(id<BCResponseSerializerContext>)context {
    if (nil == context.responseBodyData) {
        return BCNetworkErrorResponseBodyDecrypt;
    }
    
    BCAes128Key_t key = {};
    
    if (0 == BCNetworkCryptoGenerateAesKey(context.responseSeed, &key)) {
        context.responseBodyData = [BCNetworkCrypto aes128ECBDecrypt:context.responseBodyData withKey:&key];
        memset(&key, 0, sizeof(BCAes128Key_t));
        if (nil == context.responseBodyData) {
            return BCNetworkErrorResponseBodyDecrypt;
        }
        return 0;
    } else {
        return BCNetworkErrorResponseBodyDecrypt;
    }
}
@end

@implementation BCNetworkCrc32Serializer

- (NSInteger)requestProcess:(id<BCRequestSerializerContext>)context {
    if (nil == context.requestBodyData || nil == context.requestHeader) {
        return BCNetworkErrorRequestBodySign;
    }
    NSString * key = BCHeaderFieldCheckNumber;
    [context.requestHeader setObject:[NSString stringWithFormat:@"%@", @([BCNetworkCrypto crc32:context.requestBodyData])] forKey:key];
    return 0;
}
- (NSInteger)responseProcess:(id<BCResponseSerializerContext>)context {
    BOOL getBcCheckCodeHasError = false;
    id bcCheckObject = context.responseHeader[BCHeaderFieldCheckNumber];
    if (nil == bcCheckObject) {
        getBcCheckCodeHasError = true;
    }
    uint32_t bcCheck = 0;
    if (!getBcCheckCodeHasError) {
        if ([bcCheckObject isKindOfClass:[NSNumber class]]) {
            bcCheck = [(NSNumber *)bcCheckObject unsignedIntValue];
        } else if ([bcCheckObject isKindOfClass:[NSString class]]) {
            int64_t tmp = [((NSString *)bcCheckObject) longLongValue];
            if (tmp < 0) {
                getBcCheckCodeHasError = true;
            } else {
                bcCheck = (uint32_t)tmp;
            }
        } else {
            getBcCheckCodeHasError = true;
        }
    }
    if (getBcCheckCodeHasError) {
        return BCNetworkErrorResponseBodySign;
    }
    if (nil == context.responseBodyData) {
        return BCNetworkErrorResponseBodySign;
    }
    
    uint32_t code = [BCNetworkCrypto crc32:context.responseBodyData];
    if (code != bcCheck) {
        return BCNetworkErrorResponseBodySign;
    }
    return 0;
}

@end

@implementation BCNetworkJSONSerializer

- (NSInteger)requestProcess:(id<BCRequestSerializerContext>)context {
    if (nil == context.requestInfo) {
        return BCNetworkErrorRequestJSONSerialization;
    }
    NSError *serializationError = nil;
    NSData * data = [NSJSONSerialization dataWithJSONObject:context.requestInfo options:(NSJSONWritingPrettyPrinted) error:&serializationError];
    if (serializationError) {
        context.requestErrorInfo = serializationError.userInfo;
        return BCNetworkErrorRequestJSONSerialization;
    }
    context.requestBodyData = data;
    return 0;
}

- (NSInteger)responseProcess:(id<BCResponseSerializerContext>)context {
    if (nil == context.responseBodyData) {
        return BCNetworkErrorResponseJSONSerialization;
    }
    NSError *respSerializationError = nil;
    NSDictionary *json = [NSJSONSerialization JSONObjectWithData:context.responseBodyData options:(NSJSONReadingMutableLeaves) error:&respSerializationError];
    
    if (respSerializationError) {
        NSString *str = [[NSString alloc] initWithData:context.responseBodyData encoding:NSUTF8StringEncoding];
        if (str) {
            NSMutableDictionary *info = [respSerializationError.userInfo mutableCopy];
            info[@"jsonString"] = str;
            context.responseErrorInfo = info;
        } else {
            context.responseErrorInfo = respSerializationError.userInfo;
        }
        
        return BCNetworkErrorResponseJSONSerialization;
    }
    context.responseInfo = json;
    if (![json isKindOfClass:[NSDictionary class]]) {
        return BCNetworkErrorResponseJSONSerializationNotDict;
    }
    return 0;
}

@end

@implementation BCNetworkStatusSerializer

- (NSInteger)responseProcess:(id<BCResponseSerializerContext>)context {
    id statusJson = context.responseInfo[@"status"];
    if (![statusJson isKindOfClass:[NSDictionary class]]) {
        return BCNetworkErrorResponseJSONSerializationStatusNotDict;
    }
    NSInteger status = [BCNetworkSerializer integerValue:statusJson[@"code"]];
    if (status == 200) {
        return 0;
    } else {
        context.responseErrorMessage = [BCNetworkSerializer stringValue:statusJson[@"msg"]];
        return 0 != status ? status : BCNetworkErrorResponseJSONSerializationResultNotDict;
    }
}

@end

@implementation BCNetworkResultSerializer
- (NSInteger)responseProcess:(id<BCResponseSerializerContext>)context {
    
    id resultJson = context.responseInfo[@"result"];
    BOOL isDict = [resultJson isKindOfClass:[NSDictionary class]];
    BOOL isArray = [resultJson isKindOfClass:[NSArray class]];
    
    if ([resultJson isKindOfClass:[NSNull class]]) {
        context.resultObject = nil;
        if (isDict) {
            context.resultInfo = nil;
        } else if (isArray) {
            context.resultArray = nil;
        }
        return 0;
    }
    
    if (!isDict && !isArray) {
        return BCNetworkErrorResponseJSONSerializationResultNotDict;
    } else {
        context.resultObject = resultJson;
        if (isDict) {
            context.resultInfo = resultJson;
        } else if (isArray) {
            context.resultArray = resultJson;
        }
    }
    return 0;
}
@end

@implementation BCNetworkResponseSerializer
- (NSInteger)responseProcess:(id<BCResponseSerializerContext>)context {
    NSURLResponse *response = context.networkResponse;
    id responseObject = context.networkResponseObject;
    NSError * error = context.networkError;
    
    context.networkError = nil;
    context.networkResponseObject = nil;
    context.networkResponse = nil;
    
    
    if (nil == response && nil == error) {
        return BCNetworkErrorRequestUnknown;
    }

    if (error) {
        if ([error.domain isEqualToString:NSURLErrorDomain]) {
            if (error.code == NSURLErrorCancelled) {
                return BCNetworkErrorCanceled;
            } else if (error.code == NSURLErrorTimedOut) {
                return BCNetworkErrorTimeout;
            }
        }
    }
    
    
    NSInteger result = 0;
    if (error) {//failure
        if (response) {
            if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
                NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
                if (httpResponse.statusCode != 200) {
                    result = httpResponse.statusCode != 0 ? httpResponse.statusCode : BCNetworkErrorRequestUnknown;
                    context.responseErrorInfo = httpResponse.allHeaderFields;
                    context.responseHeader = httpResponse.allHeaderFields;
                } else {
                    result = BCNetworkErrorRequestUnknown;
                    context.responseErrorInfo = @{@"debugMessage":@"response error, 但是statusCode==200"};
                }
            } else {
                result = BCNetworkErrorRequestUnknown;
                context.responseErrorInfo = @{@"debugMessage":@"response 不是NSHTTPURLResponse类型"};
            }
            return result;
        } else {//无网络连接
            return BCNetworkErrorNoNet;
        }
    } else {//success
        if (response) {
            if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
                NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
                if (httpResponse.statusCode != 200) {
                    result = httpResponse.statusCode != 0 ? httpResponse.statusCode : BCNetworkErrorRequestUnknown;
                    context.responseErrorInfo = httpResponse.allHeaderFields;
                    
                    if ([responseObject isKindOfClass:[NSData class]]) {
                        context.responseBodyData = responseObject;
                    }
                    context.responseHeader = httpResponse.allHeaderFields;
                } else {
                    result = 0;
                    context.responseHeader = httpResponse.allHeaderFields;
                    if ([responseObject isKindOfClass:[NSData class]]) {
                        context.responseBodyData = responseObject;
                    }
                }
            } else {
                result = BCNetworkErrorRequestUnknown;
                context.responseErrorInfo = @{@"debugMessage":@"response 不是NSHTTPURLResponse类型"};
            }
            return result;
        } else {
            return BCNetworkErrorRequestUnknown;
        }
    }
}
@end

