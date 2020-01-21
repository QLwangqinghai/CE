//
//  BCNetworkError.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/25.
//  Copyright © 2018年 王青海. All rights reserved.
//

#ifndef BCNetworkError_h
#define BCNetworkError_h

#import <Foundation/Foundation.h>


#define BCRequestErrorDomain @"com.bichan.network"

typedef NS_ENUM(NSInteger, BCNetworkErrorCode) {
    
    //"未知错误"
    BCNetworkErrorRequestUnknown = -1,
    
    //"到不到对应的环境"
    BCNetworkErrorRequestEvnConfigNotFound = -2,
    
    //"非法参数"
    BCNetworkErrorRequestIllegalParams = -3,
    
    //"对象转JSON错误"
    BCNetworkErrorRequestJSONSerialization = -4,
    
    //加密流程错误码10-30
    
    //"数据base64编码错误"
    BCNetworkErrorRequestBodyEncode = -10,
    
    //"数据加密错误"
    BCNetworkErrorRequestBodyEncrypt = -11,
    
    //"数据签名错误"
    BCNetworkErrorRequestBodySign = -12,

    
    //加密流程错误码100-120
    //"数据签名校验错误"
    BCNetworkErrorResponseBodySign = -100,
    
    //"数据解密错误"
    BCNetworkErrorResponseBodyDecrypt = -101,
    
    //"数据base64反编码错误"
    BCNetworkErrorResponseBodyDecode = -102,

    //"数据转对象错误"
    BCNetworkErrorResponseJSONSerialization = -120,
    
    //"数据格式错误"
    BCNetworkErrorResponseJSONSerializationNotDict = -121,
    
    //"数据格式错误"
    BCNetworkErrorResponseJSONSerializationStatusNotDict = -122,
    
    //"数据格式错误"
    BCNetworkErrorResponseJSONSerializationResultNotDict = -123,
    
    //"网络错误"
    BCNetworkErrorNoNet = -200,

    //"JSON转Model错误"
    BCNetworkErrorResponseJSONToModel = -201,
    
    //"请求被取消"
    BCNetworkErrorCanceled = -202,

    //"请求超时"
    BCNetworkErrorTimeout = -203,

    
};


/*
 规定 ：
 100 - 999 是服务的器的错误信息 (目前只有 三种 200 404 500 )
 1000 - 9999 是网络错误
 10000 - 99999  属于业务错误
 
 10****   这种为通用的错误码
 ##***    ##(11-99) 代表业务   ***(001-999)错误类型
 
 10000  通用业务错误信息   （相当于 500 返回的msg 不要直接tost提示用户）
 
 以下是已经定义了的状态码 ：
 
 11  开头为：provider  服务
 
 200          正常
 11001      查不出结果
 11002      请求参数有误
 11010      余额不足
 
 */
typedef NS_ENUM(NSInteger, BCResponseStatusCode) {
    
    /**
     * HTTP 错误
     */
    BCResponseStatusCodeNone = 200,//无错误
    
    
    /**
     * 业务错误码 [10000, ...)
     */
    BCResponseStatusCodeUnknown = 10000,//未知错误，一般发生在服务器调用外部接口, msg为透传的错误信息，不能用于tost直接显示
};


static inline NSString *BCNetworkErrorKey(NSInteger code) {
    return [NSString stringWithFormat:@"%@", @(code)];
}

#endif
