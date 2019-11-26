//
//  BCColorUtil.m
//  BichanWallet
//
//  Created by 王青海 on 2018/8/6.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCColorUtil.h"

static NSString * _BCColorTrumString(NSString *string) {
    NSCharacterSet *set = [NSCharacterSet whitespaceAndNewlineCharacterSet];
    return [string stringByTrimmingCharactersInSet:set];
}
static inline NSUInteger _BCColorHexStrToInt_(NSString *str) {
    uint32_t result = 0;
    sscanf([str UTF8String], "%X", &result);
    return result;
}

static BOOL _BCColorHexStrToRGBA_(NSString *str,
                          CGFloat *r, CGFloat *g, CGFloat *b, CGFloat *a) {
    str = [_BCColorTrumString(str) uppercaseString];
    if ([str hasPrefix:@"#"]) {
        str = [str substringFromIndex:1];
    } else if ([str hasPrefix:@"0X"]) {
        str = [str substringFromIndex:2];
    }
    
    NSUInteger length = [str length];
    //         RGB            RGBA          RRGGBB        RRGGBBAA
    if (length != 3 && length != 4 && length != 6 && length != 8) {
        return NO;
    }
    
    //RGB,RGBA,RRGGBB,RRGGBBAA
    if (length < 5) {
        *r = _BCColorHexStrToInt_([str substringWithRange:NSMakeRange(0, 1)]) / 255.0f;
        *g = _BCColorHexStrToInt_([str substringWithRange:NSMakeRange(1, 1)]) / 255.0f;
        *b = _BCColorHexStrToInt_([str substringWithRange:NSMakeRange(2, 1)]) / 255.0f;
        if (length == 4)  *a = _BCColorHexStrToInt_([str substringWithRange:NSMakeRange(3, 1)]) / 255.0f;
        else *a = 1;
    } else {
        *r = _BCColorHexStrToInt_([str substringWithRange:NSMakeRange(0, 2)]) / 255.0f;
        *g = _BCColorHexStrToInt_([str substringWithRange:NSMakeRange(2, 2)]) / 255.0f;
        *b = _BCColorHexStrToInt_([str substringWithRange:NSMakeRange(4, 2)]) / 255.0f;
        if (length == 8) *a = _BCColorHexStrToInt_([str substringWithRange:NSMakeRange(6, 2)]) / 255.0f;
        else *a = 1;
    }
    return YES;
}


@implementation BCColorUtil

+ (UIColor *)colorWithHexString:(NSString *)hexStr {
    CGFloat r, g, b, a;
    if (_BCColorHexStrToRGBA_(hexStr, &r, &g, &b, &a)) {
        return [UIColor colorWithRed:r green:g blue:b alpha:a];
    }
    return nil;
}

+ (UIColor *)colorWithHexString:(NSString *)hexStr alpha:(CGFloat)alpha {
    CGFloat r, g, b, a;
    if (_BCColorHexStrToRGBA_(hexStr, &r, &g, &b, &a)) {
        return [UIColor colorWithRed:r green:g blue:b alpha:alpha];
    }
    return nil;
}

@end
