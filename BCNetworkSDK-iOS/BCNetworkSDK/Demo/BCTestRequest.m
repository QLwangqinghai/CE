//
//  BCCreateTransactionRequest.m
//  BitherText
//
//  Created by 陈少楠 on 2018/7/6.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCTestRequest.h"

@implementation BCTestRequest

- (instancetype)initWithEnvName:(NSString *)envName {
    self = [super initWithEnvName:envName];
    if (self) {
        self.apiName = @"/walletServer/getCurrentCoinPrice/1.0";
    }
    return self;
}
/*
 "sessionType" : 1,
 "userId" : "a4cd867b31644c63a7fba68579a612a0",
 "token" : "d18c8f83954e4dba90d5bafb90275f68",
 */

- (void)assembleHeaderTo:(NSMutableDictionary *)dict withConfig:(BCEnvConfig *)config {
    [super assembleHeaderTo:dict withConfig:config];
    dict[@"sessionType"] = @1;
    dict[@"userId"] = @"a4cd867b31644c63a7fba68579a612a0";
    dict[@"token"] = @"d18c8f83954e4dba90d5bafb90275f68";
}

-(void)assembleParamsTo:(NSMutableDictionary *)dict withConfig:(BCEnvConfig *)config {
    [super assembleParamsTo:dict withConfig:config];
}

- (BOOL)illegalParams {
    if (self.mobile.length <= 0) {
        return true;
    }
    return NO;
}

- (id)parseResponseData:(NSDictionary *)respData {
    return respData;
}
@end
