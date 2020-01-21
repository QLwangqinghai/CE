//
//  BCBaseRequest.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

/*  请求交互工具  负责提供请求交互相关事项
 *
 */
#import <Foundation/Foundation.h>
#import "BCNetworkError.h"
#import "BCNetworkTypes.h"

typedef NS_ENUM(NSInteger, BCNetworkResultType) {
    BCNetworkResultTypeDictionary = 0,
    BCNetworkResultTypeArray = 1,
};

@class BCEnvConfig;
@interface BCBaseRequest : NSObject


@property (nonatomic, assign) BCNetworkResultType requireResultType;

/**
 配置信息
 */
@property (nonatomic, strong) NSString *envName;

/**
 *    接口名称
 */
@property (nonatomic, strong) NSString *apiName;//  "/api/1.0"


- (instancetype)initWithEnvName:(NSString *)envName;


/**
 *    组装params参数字典
 *
 *    子类需重写此函数，且第一时间调用super，将所有需要上传的参数放入参数字典dict中
 */
- (void)assembleParamsTo:(NSMutableDictionary *)dict withConfig:(BCEnvConfig *)config NS_REQUIRES_SUPER;

/**
 *    组装bcHeader字典
 *
 *    子类需重写此函数，且第一时间调用super，将所有需要上传的参数放入参数字典dict中
 */
- (void)assembleHeaderTo:(NSMutableDictionary *)dict withConfig:(BCEnvConfig *)config NS_REQUIRES_SUPER;

/**
 *    组装bcEnv字典
 *
 *    子类需重写此函数，且第一时间调用super，将所有需要上传的参数放入参数字典dict中
 */
- (void)assembleEnvTo:(NSMutableDictionary *)dict withConfig:(BCEnvConfig *)config NS_REQUIRES_SUPER;



/**
 *    检查请求参数
 *    如果参数不符合要求，则不会发起接口请求。
 *
 *    子类需重写此函数，检查相应的参数值。
 *
 *    @return 参数是否有误
 */
- (BOOL)illegalParams;

/**
 *    重置所有接口数据到初始值
 *    当接口请求需要复用时，可在每次请求前调用此函数，重置所有参数状态。
 *
 *    子类需重写此函数，将不同的参数设置为需要的默认值。
 */
- (void)reset;

/**
 *    发起请求
 *
 *    @param success 请求成功回调
 *    @param failure 请求失败的回调
 */
- (void)sendRequestWithSuccessBlock:(BCAPISuccessHandle)success failureBlock:(BCAPIErrorHandle)failure;

/**
 *    取消请求
 */
- (void)cancel;

/**
 *    解析返回数据
 *
 *    子类需重写此函数，对接口返回进行解析处理。如不处理则直接将数据字典返回给请求回调函数。
 *
 *    @param resultInfo 返回数据转化的字段字典
 *
 *    @return 解析后的结果
 */
- (id)parseResultInfo:(NSDictionary *)resultInfo;



/**
 *    解析返回数据
 *
 *    子类需重写此函数，对接口返回进行解析处理。如不处理则直接将数据数组返回给请求回调函数。
 *
 *    @param resultArray 返回数据转化的字段数组
 *
 *    @return 解析后的结果
 */
- (id)parseResultArray:(NSArray *)resultArray;


/**
 *    解析返回数据
 *
 *    子类需重写此函数，对接口返回进行解析处理。如不处理则直接将数据数组返回给请求回调函数。
 *
 *    @param obj 返回数据转化的字段数组
 *
 *    @return 解析后的结果
 */
- (id)parseUnArrayAndUnDictionaryResultObject:(id)obj;


/**
 *    当接口请求返回错误时，此函数被调用
 *
 *    子类需重写此函数，对错误进行处理。如不处理则直接将数据信息返回给请求回调函数。
 *
 *    @param error 错误信息
 *
 *    @return 错误处理结果
 */
- (id)handleResponseError:(BCError *)error;

/**
 接口地址
 
 @return url
 */
- (NSString *)apiURL;


@end





