//
//  FFAudioPCMBufferAdapter.h
//  LiveVideoCompress
//
//  Created by vector on 2020/5/6.
//  Copyright © 2020 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "libavformat/avformat.h"

NS_ASSUME_NONNULL_BEGIN
//http://www.ffmpeg.org/doxygen/4.0/transcoding_8c-example.html
@class FFAudioAdapter;
@interface FFAudioPCMBufferAdapter : NSObject

@property (nonatomic, strong, readonly) FFAudioAdapter * audioAdapter;

- (instancetype)initWithAudioAdapter:(FFAudioAdapter *)audioAdapter;

//https://www.ffmpeg.org/doxygen/4.0/encode_video_8c-example.html

//time 单位 毫秒

- (void)finish;

//- (FFAVFrame *)makeFrame:(AVFrame *)frame handler:(void(^)(CGContextRef context))handler;

@end

NS_ASSUME_NONNULL_END
