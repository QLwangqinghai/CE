//
//  BCDataUtil.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/22.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCDataUtil.h"

@implementation BCDataUtil

+ (NSString *)hexWithData:(NSData *)data {
    const uint8_t *bytes = data.bytes;
    NSUInteger length = data.length;
    NSMutableString *hex = [NSMutableString string];
    
    for (NSUInteger i = 0; i < length; i++) {
        [hex appendFormat:@"%02x", bytes[i]];
    }
    
    return [hex uppercaseString];
}

+ (NSData *)dataWithHexString:(NSString *)string {
    NSUInteger stringLength = string.length;

    if (stringLength % 2) return nil;
    
    NSMutableData *d = [NSMutableData dataWithLength:stringLength / 2];
    uint8_t b = 0;
    
    for (NSUInteger i = 0; i < stringLength; i++) {
        unichar c = [string characterAtIndex:i];
        
        switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                b += c - '0';
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                b += c + 10 - 'A';
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                b += c + 10 - 'a';
                break;
            default:
                return nil;
        }
        
        if (i % 2) {
            [d appendBytes:&b length:1];
            b = 0;
        } else {
            b *= 16;
        }
    }
    
    return d;
}

@end
