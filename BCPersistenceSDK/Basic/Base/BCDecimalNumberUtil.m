//
//  BCDecimalNumberUtil.m
//  BichanWallet
//
//  Created by 王青海 on 2018/9/11.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCDecimalNumberUtil.h"

@implementation BCDecimalNumberUtil

+ (NSDecimalNumber *)numberWithString:(NSString *)string {
    if (nil == string) {
        return nil;
    }
    NSDecimalNumber *number = [[NSDecimalNumber alloc] initWithString:string];
    if ([number isEqualToNumber:[NSDecimalNumber notANumber]]) {
        return nil;
    } else {
        return number;
    }
}

@end
