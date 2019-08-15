//
//  ScrollViewCore.h
//  ScrollView
//
//  Created by vector on 2019/8/15.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef struct {
    int32_t x;
    int32_t y;
} UIPoint;

typedef struct {
    int32_t width;
    int32_t height;
} UISize;

typedef struct {
    UIPoint origin;
    UISize size;
} UIRect;






NS_ASSUME_NONNULL_BEGIN

@interface ScrollViewCore : UIControl

@end

NS_ASSUME_NONNULL_END
