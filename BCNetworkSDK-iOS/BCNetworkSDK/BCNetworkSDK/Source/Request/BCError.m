//
//  BCError.m
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/30.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCError.h"
#import "BCModelUtil.h"

@implementation BCError

- (instancetype)initWithDomain:(NSErrorDomain)domain code:(NSInteger)code userInfo:(NSDictionary<NSErrorUserInfoKey,id> *)dict {
    self = [super initWithDomain:domain code:code userInfo:dict];
    if (self) {
        
    }
    return self;
}

- (NSString *)debugDescription {
    return [NSString stringWithFormat:@"<%@: %p> %@", [self class], self, [self modelToJSONString]];
}

- (NSString *)description {
    return [NSString stringWithFormat:@"BCError: %@", [self modelToJSONString]];
}

+ (instancetype)networkErrorWithCode:(NSInteger)code message:(NSString *)message userInfo:(NSDictionary *)userInfo {
    BCError *error = [[BCError alloc] initWithDomain:BCRequestErrorDomain code:code userInfo:userInfo];
    if (message) {
        error.message = message;
    } else {
        error.message = [BCError networkErrorMessageWithCode:code];
    }
    return error;
}

+ (NSString *)networkErrorMessageWithCode:(NSInteger)code {
    static NSDictionary *__networkErrorMessageDict = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        dict[BCNetworkErrorKey(BCNetworkErrorRequestUnknown)] = @"未知错误";
        dict[BCNetworkErrorKey(BCNetworkErrorRequestEvnConfigNotFound)] = @"到不到对应的环境";
        dict[BCNetworkErrorKey(BCNetworkErrorRequestIllegalParams)] = @"非法参数";
        dict[BCNetworkErrorKey(BCNetworkErrorRequestJSONSerialization)] = @"对象转JSON错误";
        dict[BCNetworkErrorKey(BCNetworkErrorRequestBodyEncode)] = @"数据base64编码错误";
        dict[BCNetworkErrorKey(BCNetworkErrorRequestBodyEncrypt)] = @"数据加密错误";
        dict[BCNetworkErrorKey(BCNetworkErrorRequestBodySign)] = @"数据签名错误";
        
        dict[BCNetworkErrorKey(BCNetworkErrorResponseBodySign)] = @"数据签名校验错误";
        dict[BCNetworkErrorKey(BCNetworkErrorResponseBodyDecrypt)] = @"数据解密错误";
        dict[BCNetworkErrorKey(BCNetworkErrorResponseBodyDecode)] = @"数据base64反编码错误";
        dict[BCNetworkErrorKey(BCNetworkErrorResponseJSONSerialization)] = @"数据转对象错误";
        dict[BCNetworkErrorKey(BCNetworkErrorResponseJSONSerializationNotDict)] = @"数据格式错误";
        dict[BCNetworkErrorKey(BCNetworkErrorResponseJSONSerializationStatusNotDict)] = @"数据格式错误";
        dict[BCNetworkErrorKey(BCNetworkErrorResponseJSONSerializationResultNotDict)] = @"数据格式错误";
        dict[BCNetworkErrorKey(BCNetworkErrorNoNet)] = @"网络错误";
        dict[BCNetworkErrorKey(BCNetworkErrorResponseJSONToModel)] = @"JSON转Model错误";
        dict[BCNetworkErrorKey(BCNetworkErrorTimeout)] = @"请求超时";
        dict[BCNetworkErrorKey(BCNetworkErrorCanceled)] = @"请求被取消";
        
        __networkErrorMessageDict = [dict copy];
    });
    return __networkErrorMessageDict[BCNetworkErrorKey(code)];
}

- (id)modelToJSONObject {
    NSMutableDictionary *jsonObject = [NSMutableDictionary dictionary];
    jsonObject[@"code"] = @(self.code);
    jsonObject[@"domain"] = [self.domain description];
    jsonObject[@"message"] = [self.message description];

    if (self.userInfo && [NSJSONSerialization isValidJSONObject:self.userInfo]) {
        jsonObject[@"userInfo"] = self.userInfo;
    }
    return jsonObject;
}

@end

