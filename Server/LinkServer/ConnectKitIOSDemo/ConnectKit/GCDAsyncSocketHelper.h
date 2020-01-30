//
//  SwiftHelper.h
//  IMDemo
//
//  Created by wangqinghai on 16/5/28.
//  Copyright © 2016年 st. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface GCDAsyncSocketHelper : NSObject

+ (nullable NSDictionary *)loadGCDSocketCertificateSettingsWithPeerName:(nonnull NSString *)peer passwd:(nonnull NSString *)passwd pkcs12data:(nonnull NSData *)pkcs12data;
@end





