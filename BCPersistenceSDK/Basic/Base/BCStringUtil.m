//
//  BCStringUtil.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/28.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCStringUtil.h"

@implementation BCStringUtil


+ (BOOL)string:(NSString *)string isEquelTo:(NSString *)other {
    if (string == other) {
        return true;
    }
    NSString *str1 = string;
    NSString *str2 = other;
    if (nil == str1) {
        str1 = @"";
    }
    if (nil == str2) {
        str2 = @"";
    }
    
    return [str1 isEqualToString:str2];
}

+ (BOOL)isAllNumber:(NSString *)string {
    NSString *checkedNumString = [string stringByTrimmingCharactersInSet:[NSCharacterSet decimalDigitCharacterSet]];
    if(checkedNumString.length > 0) {
        return NO;
    }
    return YES;
}



@end
