//
//  FFVideoFrame.h
//  LiveVideoCompress
//
//  Created by vector on 2020/3/5.
//  Copyright © 2020 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

#include "libavformat/avformat.h"

NS_ASSUME_NONNULL_BEGIN

@interface FFVideoFrame : NSObject
{
    AVFrame __frame;
}

@property (nonatomic, assign, readonly) AVFrame * frame;

@end

NS_ASSUME_NONNULL_END
