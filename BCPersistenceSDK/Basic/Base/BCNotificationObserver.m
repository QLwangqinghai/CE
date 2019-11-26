//
//  BCNotificationObserver.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/29.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCNotificationObserver.h"

@implementation BCNotificationObserver

- (instancetype)initWithNotificationName:(NSString *)name handler:(id)handler onNotify:(void(^)(id handler, NSNotification *notify))onNotify {
    self = [super init];
    if (self) {
        self.onNotify = onNotify;
        self.handler = handler;
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleNotify:) name:name object:nil];
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)handleNotify:(NSNotification *)notify {
    if (self.onNotify) {
        __strong id handler = self.handler;
        self.onNotify(handler, notify);
    }
}



@end
