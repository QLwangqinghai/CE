//
//  FFVideoFrame.m
//  LiveVideoCompress
//
//  Created by vector on 2020/3/5.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "FFVideoFrame.h"

@implementation FFVideoFrame

- (instancetype)init{
    self = [super init];
    if (self) {
        _frame = &(self->__frame);
        memset(_frame, 0, sizeof(AVFrame));
    }
    return self;
}

@end
