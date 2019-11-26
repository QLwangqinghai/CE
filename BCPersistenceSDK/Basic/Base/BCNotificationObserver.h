//
//  BCNotificationObserver.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/29.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BCNotificationObserver : NSObject

@property (nonatomic, weak) id handler;
@property (nonatomic, copy) void(^onNotify)(id handler, NSNotification *notify);

- (instancetype)initWithNotificationName:(NSString *)name handler:(id)handler onNotify:(void(^)(id handler, NSNotification *notify))onNotify;


@end
