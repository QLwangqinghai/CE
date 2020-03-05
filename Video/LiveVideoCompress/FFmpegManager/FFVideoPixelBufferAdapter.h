//
//  FFVideoPixelBufferAdapter.h
//  LiveVideoCompress
//
//  Created by vector on 2020/3/5.
//  Copyright © 2020 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

#include "libavformat/avformat.h"

NS_ASSUME_NONNULL_BEGIN
//http://www.ffmpeg.org/doxygen/4.0/transcoding_8c-example.html
@class FFVideoAdapter;
@interface FFVideoPixelBufferAdapter : NSObject

@property (nonatomic, strong, readonly) FFVideoAdapter * videoAdapter;

- (instancetype)initWithVideoAdapter:(FFVideoAdapter *)videoAdapter;

//https://www.ffmpeg.org/doxygen/4.0/encode_video_8c-example.html#a9
- (void)writeFrameAtTime:(int64_t)time handler:(void(^)(CGContextRef context))handler;

//- (FFAVFrame *)makeFrame:(AVFrame *)frame handler:(void(^)(CGContextRef context))handler;

@end

NS_ASSUME_NONNULL_END
