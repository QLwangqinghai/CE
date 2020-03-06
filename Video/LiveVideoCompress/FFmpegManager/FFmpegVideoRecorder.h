//
//  FFmpegVideoRecorder.h
//  LiveVideoCompress
//
//  Created by vector on 2020/2/29.
//  Copyright © 2020 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FFAVAdapter.h"
#include "libavformat/avformat.h"


//rgb => yuv
//https://github.com/cirosantilli/cpp-cheat/blob/9974f61341f9740915a61f0de04e0e9e2f18ad91/ffmpeg/encode.c

typedef NS_ENUM(NSUInteger, FFAVFormat) {
    FFAVFormatMp4 = 1,
};

typedef NS_ENUM(NSUInteger, FFAudioCodec) {
    FFAudioCodecAac = 1,
};
typedef NS_ENUM(NSUInteger, FFVideoCodec) {
    FFVideoCodecH264 = 1,
};

typedef NS_ENUM(NSUInteger, FFVideoCodecPixelFormat) {
    FFVideoCodecPixelFormatA = 1,
};

NS_ASSUME_NONNULL_BEGIN

@class FFAVFormatContext;
@interface FFAVStream : NSObject
@property (nonatomic, strong, readonly) FFAVFormatContext * formatContext;

@property (nonatomic, assign, readonly) AVStream * stream;

@property (nonatomic, assign, readonly) AVCodec * codec;

@end

//https://blog.csdn.net/xubuwei/article/details/83994044
@interface FFAVFormatContext : NSObject

+ (nullable instancetype)contextWithFormat:(FFAVFormat)format path:(NSString *)path;

- (nullable FFAVStream *)createAudioStream:(FFAudioCodec)codec;
- (nullable FFAVStream *)createVideoStream:(FFVideoCodec)codec;

@property (nonatomic, assign, readonly) FFAVFormat format;
@property (nonatomic, copy, readonly) NSString * path;
@property (nonatomic, assign, readonly) AVFormatContext * context;

@end



@interface FFBaseAVAdapter : NSObject
@property (nonatomic, strong, readonly) FFAVFormatContext * formatContext;
@property (nonatomic, strong, readonly) FFAVStream * stream;
@property (nonatomic, assign, readonly) AVCodecContext * context;

- (BOOL)close;

@end

@interface FFAudioOption : NSObject

@property (nonatomic, assign) FFAudioCodec codec;

@end
@interface FFVideoOption : NSObject
@property (nonatomic, assign) FFVideoCodec codec;
@property (nonatomic, assign) uint32_t width;
@property (nonatomic, assign) uint32_t height;
@property (nonatomic, assign) int frameRate;//帧率 每秒钟多少帧
@end


@interface FFAudioAdapter : FFBaseAVAdapter


@end

@interface FFVideoAdapter : FFBaseAVAdapter
@property (nonatomic, assign, readonly) uint32_t width;
@property (nonatomic, assign, readonly) uint32_t height;

@end


@interface FFAVWriter : NSObject

@property (nonatomic, strong, readonly) FFAVFormatContext * context;


@property (nonatomic, strong, readonly, nullable) FFAudioAdapter * audioAdapter;
@property (nonatomic, strong, readonly, nullable) FFAudioAdapter * videoAdapter;


+ (nullable instancetype)writerWithFormat:(FFAVFormat)format path:(NSString *)path;


//添加失败返回空
- (nullable FFAudioAdapter *)addAudioAdapter:(FFAudioOption *)option;

//添加失败返回空
- (nullable FFVideoAdapter *)addVideoAdapter:(FFVideoOption *)option;


+ (void)test;

@end



@interface FFmpegVideoRecorder : NSObject

@end
NS_ASSUME_NONNULL_END
