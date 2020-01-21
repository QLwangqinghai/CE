//
//  BCNetworkDispatch.m
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/26.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCNetworkDispatch.h"

@implementation BCNetworkDispatch

- (instancetype)initWithSerializationQueue:(dispatch_queue_t)serializationQueue {
    self = [super init];
    if (self) {
        self.serializationQueue = serializationQueue;
    }
    return self;
}


+ (instancetype)sharedInstance {
    static BCNetworkDispatch *__sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __sharedInstance = [[BCNetworkDispatch alloc] initWithSerializationQueue:dispatch_queue_create("com.bichain.request.serializer", DISPATCH_QUEUE_CONCURRENT)];
    });
    return __sharedInstance;
}

+ (dispatch_queue_t)serializationQueue {
    return [BCNetworkDispatch sharedInstance].serializationQueue;
}

@end
