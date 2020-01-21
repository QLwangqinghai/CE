//
//  BCBaseRequest.m
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCBaseRequest.h"
#import <AFNetworking.h>
#import "BCNetworkManager+Private.h"
#import "BCNetworkCrypto.h"
#import "BCNetworkSerializer.h"
#import "BCNetworkDispatch.h"
#import <objc/runtime.h>
#import <AFNetworking/AFURLResponseSerialization.h>
#import "BCEnvConfig.h"
#import "BCLog.h"

NSString *BCNetworkJOSNObjectToString(id obj) {
    NSData *data = [NSJSONSerialization dataWithJSONObject:obj options:(NSJSONWritingPrettyPrinted) error:nil];
    if (data) {
        return [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    } else {
        return nil;
    }
}



@interface BCBaseRequest ()
{
    NSURLSessionDataTask *_task;
}

@property (atomic, strong) NSURLSessionDataTask *task;

@end

@implementation BCBaseRequest

- (instancetype)initWithEnvName:(NSString *)envName {
    self = [super init];
    if (self) {
        _envName = [envName copy];
    }
    return self;
}

/**
 *    结合服务器地址和接口名称返回接口地址
 *
 *    @return 接口地址
 */
- (NSString*)apiURLWithEnvConfig:(BCEnvConfig *)config {
    if (nil == config.host) {
        return nil;
    }
    if (self.apiName.length > 0) {
        if (self.apiName.length > 0 && [self.apiName hasPrefix:@"/"]) {
            return [NSString stringWithFormat:@"%@%@", config.host, self.apiName];
        } else {
            return [NSString stringWithFormat:@"%@/%@", config.host, self.apiName];
        }
    } else {
        return config.host;
    }
}

/**
 *    结合服务器地址和接口名称返回接口地址
 *
 *    @return 接口地址
 */
- (NSString*)apiURL {
    BCEnvConfig *config = [[BCNetworkManager sharedInstance] getEnvConfigForName:self.envName];
    if (nil == config) {
        return nil;
    }
    return [self apiURLWithEnvConfig:config];
}

/**
 *    组装params参数字典
 *
 *    子类需重写此函数，且第一时间调用super，将所有需要上传的参数放入参数字典dict中
 */
- (void)assembleParamsTo:(NSMutableDictionary *)dict withConfig:(BCEnvConfig *)config {

}

/**
 *    组装bcHeader字典
 *
 *    子类需重写此函数，且第一时间调用super，将所有需要上传的参数放入参数字典dict中
 */
- (void)assembleHeaderTo:(NSMutableDictionary *)dict withConfig:(BCEnvConfig *)config {
    if (config.appID.length > 0) {
        dict[@"bcAppID"] = config.appID;
    } else {
        BCLogError(@"config.appID 为空");
    }
}

/**
 *    组装bcEnv字典
 *
 *    子类需重写此函数，且第一时间调用super，将所有需要上传的参数放入参数字典dict中
 */
- (void)assembleEnvTo:(NSMutableDictionary *)dict withConfig:(BCEnvConfig *)config {
    
}

/**
 *    检查请求参数
 *    如果参数不符合要求，则不会发起接口请求。
 *
 *    子类需重写此函数，检查相应的参数值。
 *
 *    @return 参数是否有误
 */
- (BOOL)illegalParams {
    return NO;
}

/**
 *    重置所有接口数据到初始值
 *    当接口请求需要复用时，可在每次请求前调用此函数，重置所有参数状态。
 *
 *    子类需重写此函数，将不同的参数设置为需要的默认值。
 */
- (void)reset {
    [self.task cancel];
    
    self.task = nil;
}

- (void)p_handleErrorToCallback:(BCError *)error context:(BCNetworkSerializerContext *)context handle:(BCAPIErrorHandle)handle {
    id obj = [self handleResponseError:error];
    BCNetworkManager *manager = [BCNetworkManager sharedInstance];
    context.status = BCRequestStatusFinishError;
    [manager reportWithContext:context error:error];
    if (handle) {
        handle(obj);
    }
    self.task = nil;
}
- (void)p_makeErrorToCallback:(NSInteger)code message:(NSString *)message userInfo:(NSDictionary *)userInfo context:(BCNetworkSerializerContext *)context handle:(BCAPIErrorHandle)handle {
    BCError *error = [BCError networkErrorWithCode:code message:message userInfo:userInfo];
    [self p_handleErrorToCallback:error context:context handle:handle];
    BCLogVerbose(@"api: %@, failure, error:\n%@\n", context.URLString, error);
}
- (void)p_handleResultToCallback:(id)obj context:(BCNetworkSerializerContext *)context handle:(BCAPISuccessHandle)handle {
    BCNetworkManager *manager = [BCNetworkManager sharedInstance];
    context.status = BCRequestStatusFinishSuccess;
    [manager reportWithContext:context error:nil];
    if (handle) {
        handle(obj);
    }
    self.task = nil;
    BCLogVerbose(@"api: %@, success, response: \n%@\n", context.URLString, [[NSString alloc] initWithData:context.responseBodyData encoding:NSUTF8StringEncoding]);
}

- (NSDictionary *)buildRequestBodyWithConfig: (BCEnvConfig *)config {
    NSMutableDictionary *bcParam = [NSMutableDictionary dictionary];
    [self assembleParamsTo:bcParam withConfig:config];
    
    NSMutableDictionary *bcHeader =  [[BCNetworkManager sharedInstance] makeBCHeader];
    [self assembleHeaderTo:bcHeader withConfig:config];
    
    NSMutableDictionary *bcEnv = [NSMutableDictionary dictionary];
    [self assembleEnvTo:bcEnv withConfig:config];
    
    NSDictionary *postBody = @{
                               @"bcParam" : bcParam,
                               @"bcHeader" : bcHeader,
                               @"bcEnv" : bcEnv
                               };
    return postBody;
}

- (void)processResponseWithContext:(BCNetworkSerializerContext *)context
                    onSuccessBlock:(BCAPISuccessHandle)success
                      failureBlock:(BCAPIErrorHandle)failure {
    
    NSInteger  responseProcessResult = [[BCNetworkSerializer sharedInstance] responseProcess:context];
    if (0 != responseProcessResult) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self p_makeErrorToCallback:responseProcessResult message:context.responseErrorMessage userInfo:context.responseErrorInfo context:context handle:failure];
        });
    } else {

        id obj = nil;
        if (context.resultInfo) {
            obj = [self parseResultInfo:context.resultInfo];
        } else if (context.resultArray) {
            obj = [self parseResultArray:context.resultArray];
        } else {
            obj = [self parseUnArrayAndUnDictionaryResultObject:context.resultObject];
        }

        if (nil == obj) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self p_makeErrorToCallback:BCNetworkErrorResponseJSONToModel message:nil userInfo:nil context:context handle:failure];
            });
        } else {
            if ([obj isKindOfClass:[NSError class]]) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self p_handleErrorToCallback:obj context:context handle:failure];
                });
            } else {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self p_handleResultToCallback:obj context:context handle:success];
                });
            }
        }
    }
    return;
    
}

- (void)buildRequestToSendWithContext:(BCNetworkSerializerContext *)context
                       onSuccessBlock:(BCAPISuccessHandle)success
                         failureBlock:(BCAPIErrorHandle)failure {

    NSInteger requestProcessResult = [[BCNetworkSerializer sharedInstance] requestProcess:context];
    if (0 == requestProcessResult) {
        NSMutableURLRequest *request = [BCBaseRequest postRequestWithURLString:context.URLString header:context.requestHeader jsonBody:context.requestBodyData headerConfig:context.requestHeader];
        
        NSURLSessionDataTask *task = nil;
        task = [BCBaseRequest dataTaskWithRequest:request
                                   uploadProgress:nil
                                 downloadProgress:nil
                                completionHandler:^(NSURLResponse *response, id  _Nullable responseObject, NSError * _Nullable error) {
                                    context.networkResponse = response;
                                    context.networkResponseObject = responseObject;
                                    context.networkError = error;
                                    
                                    [self processResponseWithContext:context onSuccessBlock:success failureBlock:failure];
                                }];
        if (nil == task) {
            if (failure) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    failure([BCError networkErrorWithCode:BCNetworkErrorRequestUnknown message:@"request task 创建失败" userInfo:nil]);
                });
            }
            return;
        } else {
            self.task = task;
            [task resume];
        }
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self p_makeErrorToCallback:requestProcessResult message:nil userInfo:context.requestErrorInfo context:context handle:failure];
        });
    }
}

- (void)sendRequestWithSuccessBlock:(BCAPISuccessHandle)success failureBlock:(BCAPIErrorHandle)failure {
    BCNetworkSerializerContext *context = [[BCNetworkSerializerContext alloc] init];
    BCNetworkManager *manager = [BCNetworkManager sharedInstance];
    context.status = BCRequestStatusRequesting;
    [manager reportWithContext:context error:nil];
    
    if ([self illegalParams]) { // check params
        // if params are illegal, stop request and redirect to the failure process
        
        BCLogVerbose(@"illegal params when sending request %@", [self apiURL]);
        dispatch_async(dispatch_get_main_queue(), ^{
            [self p_makeErrorToCallback:BCNetworkErrorRequestIllegalParams message:nil userInfo:nil context:context handle:failure];
        });
        return;
    }

    BCEnvConfig *config = [[BCNetworkManager sharedInstance] getEnvConfigForName:self.envName];
    if (nil == config) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self p_makeErrorToCallback:BCNetworkErrorRequestEvnConfigNotFound message:nil userInfo:nil context:context handle:failure];
        });
        return;
    }
    context.appID = config.appID;
    
    NSString *apiURL = [self apiURLWithEnvConfig:config];
    if (nil == apiURL) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self p_makeErrorToCallback:BCNetworkErrorRequestUnknown message:nil userInfo:nil context:context handle:failure];
        });
        return;
    }
    
    NSMutableDictionary *headerFields = [NSMutableDictionary dictionary];
    NSDictionary *postBody = [self buildRequestBodyWithConfig:config];
    [[BCNetworkManager sharedInstance] putTailInfo:context.tailInfo];
    context.URLString = apiURL;
    context.requestInfo = postBody;
    context.requestHeader = headerFields;
    context.requestSeed = config.seedData;
    context.responseSeed = config.seedData;
    
    dispatch_async([BCNetworkDispatch serializationQueue], ^{
        [self buildRequestToSendWithContext:context onSuccessBlock:success failureBlock:failure];
    });
    
    //log reqeust info, ignore the params if it's too long
    if ([[postBody description] length] < 200000) {
        BCLogDebug(@"Did send request(%p) %@ with http body:\n%@", self, [self apiURL], BCNetworkJOSNObjectToString(postBody));
    } else {
        BCLogVerbose(@"Did send request(%p) %@ with long params, params string length:%lu", self, [self apiURL], (unsigned long)[[postBody description] length]);
    }
}

/**
 *    取消请求
 */
- (void)cancel {
    [self.task cancel];
}

- (id)parseResultInfo:(NSDictionary *)resultInfo {
    return resultInfo;
}

- (id)parseResultArray:(NSArray *)resultArray {
    return resultArray;
}

/**
 *    解析返回数据
 *
 *    子类需重写此函数，对接口返回进行解析处理。如不处理则直接将数据数组返回给请求回调函数。
 *
 *    @param id 返回数据转化的字段数组
 *
 *    @return 解析后的结果
 */
- (id)parseUnArrayAndUnDictionaryResultObject:(id)obj {
    return nil;
}

/**
 *    当接口请求返回错误时，此函数被调用
 *
 *    子类需重写此函数，对错误进行处理。如不处理则直接将数据信息返回给请求回调函数。
 *
 *    @param error 错误信息
 *
 *    @return 错误处理结果
 */
- (id)handleResponseError:(BCError*)error {
    self.task = nil;
    
    return error;
}

+ (AFURLSessionManager*)defaultNetworkManager {
    static AFURLSessionManager* __networkManager = nil;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __networkManager = [[AFURLSessionManager alloc] initWithSessionConfiguration:nil];
        AFHTTPResponseSerializer *serializer = [AFHTTPResponseSerializer serializer];
        __networkManager.responseSerializer = serializer;
        NSMutableSet *set = [serializer.acceptableContentTypes mutableCopy];
        if (nil == set) {
            set = [NSMutableSet set];
        }
        
        [set addObject:@"application/json"];
        [set addObject:@"text/plain"];
        [set addObject:@"text/json"];
        serializer.acceptableContentTypes = [set copy];
        __networkManager.completionQueue = [BCNetworkDispatch serializationQueue];
    });
    
    return __networkManager;
}

+ (AFHTTPRequestSerializer*)requestSerializer {
    static AFHTTPRequestSerializer* __requestSerializer = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __requestSerializer = [AFHTTPRequestSerializer serializer];
    });
    return __requestSerializer;
}


+ (NSMutableURLRequest *)postRequestWithURLString:(NSString *)URLString
                                           header:(NSDictionary *)header
                                         jsonBody:(NSData *)jsonBody
                                     headerConfig:(NSDictionary *)config {
    if (nil == URLString || nil == jsonBody) {
        return nil;
    }
    
    NSError *error;
    NSMutableURLRequest *mutableRequest = [[BCBaseRequest requestSerializer] requestWithMethod:@"POST" URLString:URLString parameters:nil error:&error];
    
    [mutableRequest setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
//    [mutableRequest setValue:@"text/plain" forHTTPHeaderField:@"Content-Type"];

    [header enumerateKeysAndObjectsUsingBlock:^(id field, id value, BOOL *stop) {
        [mutableRequest setValue:value forHTTPHeaderField:field];
    }];
    
    [mutableRequest setHTTPBody:jsonBody];
    
    return mutableRequest;
}

+ (NSURLSessionDataTask *)dataTaskWithRequest:(NSURLRequest *)request
                               uploadProgress:(nullable void (^)(NSProgress *uploadProgress)) uploadProgress
                             downloadProgress:(nullable void (^)(NSProgress *downloadProgress)) downloadProgress
                            completionHandler:(nullable void (^)(NSURLResponse *response, id _Nullable responseObject, NSError * _Nullable error)) completionHandler {
    
    AFURLSessionManager *session = [BCBaseRequest defaultNetworkManager];
    if (nil == request) {
        return nil;
    }
    __block NSURLSessionDataTask *dataTask = nil;
    dataTask = [session dataTaskWithRequest:request
                             uploadProgress:uploadProgress
                           downloadProgress:downloadProgress
                          completionHandler:completionHandler];
    
    return dataTask;
}


@end





