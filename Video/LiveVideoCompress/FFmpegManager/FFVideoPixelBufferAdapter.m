//
//  FFVideoPixelBufferAdaptor.m
//  LiveVideoCompress
//
//  Created by vector on 2020/3/5.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "FFVideoPixelBufferAdapter.h"
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <mach/mach_time.h>
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#import "libyuv/libyuv.h"
#import "FFVideoFrame.h"
#import "FFmpegVideoRecorder.h"


/*
 https://www.jianshu.com/p/7da76246ce82
 kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange = '420v'，表示输出的视频格式为NV12；范围： (luma=[16,235] chroma=[16,240])
 kCVPixelFormatType_420YpCbCr8BiPlanarFullRange = '420f'，表示输出的视频格式为NV12；范围： (luma=[0,255] chroma=[1,255])
 kCVPixelFormatType_32BGRA = 'BGRA', 输出的是BGRA的格式
 */

@interface FFVideoPixelBufferAdapter ()

@property (nonatomic, assign, readonly) uint32_t width;
@property (nonatomic, assign, readonly) uint32_t height;

@property (nonatomic, assign, readonly) uint8_t * ptr;
@property (nonatomic, assign, readonly) uint8_t * yuvBuffer;

@property (nonatomic, assign, readonly) CGContextRef bitmapContext;
@property (nonatomic, assign, readonly) AVFrame * from;
@property (nonatomic, assign, readonly) AVFrame * to;
@property (nonatomic, assign, readonly) struct SwsContext * swsContext;
@end
@implementation FFVideoPixelBufferAdapter

+ (CGColorSpaceRef)colorSpace {
    static CGColorSpaceRef __colorSpace = NULL;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __colorSpace = CGColorSpaceCreateDeviceRGB();
    });
    return __colorSpace;
}

- (instancetype)initWithVideoAdapter:(FFVideoAdapter *)videoAdapter {
    self = [super init];
    if (self) {
        _videoAdapter = videoAdapter;
        uint32_t width = videoAdapter.width;
        uint32_t height = videoAdapter.height;
        _width = width;
        _height = height;
        _from = av_frame_alloc();
        _from->width = width;
        _from->height = height;
        _from->format = AV_PIX_FMT_BGR0;

        _to = av_frame_alloc();
        _to->width = width;
        _to->height = height;
        _to->format = AV_PIX_FMT_NV12;
        
        int ret3 = av_frame_get_buffer(_from, 32);
        int ret4 = av_frame_get_buffer(_to, 32);

        int ret0 = av_frame_make_writable(_from);
        int ret1 = av_frame_make_writable(_to);
        
        _swsContext = sws_getContext(videoAdapter.width, height, AV_PIX_FMT_BGR0, width, height, AV_PIX_FMT_NV12, SWS_BILINEAR, NULL, NULL, NULL);
        _yuvBuffer = malloc(width * height * 2);
        size_t size = width * height * 4;
        _ptr = malloc(size);
        assert(_ptr);
        memset(_ptr, 0, size);
        
        uint32_t info = kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst;
        _bitmapContext = CGBitmapContextCreate(_ptr, width, height, 8, width * 4, [FFVideoPixelBufferAdapter colorSpace], info);
        assert(_bitmapContext);
    }
    return self;
}
- (void)makeFrame:(AVFrame *)frame handler:(void(^)(CGContextRef context))handler {
    handler(self.bitmapContext);
        
    [self makeFrame:frame handler:^(CGContextRef  _Nonnull context) {
        
    }];
}

- (void)sendFrame:(AVFrame *)frame {
    int ret = 0;

    if (frame) {
        ret = av_frame_is_writable(frame);
        if (ret != 0) {
            int wresult = avcodec_send_frame(self.videoAdapter.context, frame);
            if (0 == wresult) {
                //success
            }
            NSLog(@"avcodec_send_frame wresult: %@", @(wresult == 0));
        }
    } else {
        int wresult = avcodec_send_frame(self.videoAdapter.context, frame);
        if (0 == wresult) {
            //success
        }
        NSLog(@"avcodec_send_frame wresult: %@", @(wresult == 0));
    }
    
    AVPacket pac = {};
    av_init_packet(&pac);
    AVPacket * pkt = &pac;
    


    while (ret >= 0) {
        ret = avcodec_receive_packet(self.videoAdapter.context, pkt);
        if (ret == AVERROR(EAGAIN)) {
            return;
        } else if (ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }
           
        printf("Write packet %ld (size=%5d)\n", pkt->pts, pkt->size);
//           fwrite(pkt->data, 1, pkt->size, outfile);
        ret = av_interleaved_write_frame(self.videoAdapter.formatContext.context, pkt);
        NSLog(@"av_interleaved_write_frame wresult: %@", @(ret == 0));
        av_packet_unref(pkt);
    }
    
    NSLog(@"av_interleaved_write_frame wresult: %@", @(ret == 0));

}

- (void)writeFrameAtTime:(int64_t)time handler:(void(^)(CGContextRef context))handler {
    int64_t tmp = time / self.videoAdapter.context->ticks_per_frame;
    tmp = time * self.videoAdapter.stream.stream->time_base.den / self.videoAdapter.stream.stream->time_base.num / 1000;
    
    handler(self.bitmapContext);
    BOOL result = [self mapTo];
    AVFrame * to = self.to;
    to->pts = tmp;
//    AVRational t = {.num = time, .den = self.videoAdapter.stream.stream->time_base.den};
//    to->sample_aspect_ratio = t;
//    AVCodecContext
    [self sendFrame:to];
}

- (void)finish {
    [self sendFrame:NULL];
}


- (BOOL)mapTo {
    uint8_t * source = self.ptr;
    memcpy(_from->data[0], source, 720 * 1280 * 4);
    
    
    int width = self.width;
    int height = self.height;

    AVFrame * from = self.from;
    AVFrame * to = self.to;
    
//    AV_PIX_FMT_0RGB,        ///< packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
//    AV_PIX_FMT_RGB0,        ///< packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined
//    AV_PIX_FMT_0BGR,        ///< packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
//    AV_PIX_FMT_BGR0,        ///< packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined

    int len = av_image_fill_arrays(from->data, from->linesize, source, AV_PIX_FMT_BGR0, width, height, 1);

        //指定原数据格式，分辨率及目标数据格式，分辨率
    struct SwsContext *sws = self.swsContext;
    int ret = sws_scale(sws, from->data, from->linesize, 0, height, to->data, to->linesize);
    return (ret == height);
}

/*
 */

//- (AVFrame *)makeFrame:(enum AVPixelFormat)fmt width:(int)width height:(int)height {
//    AVFrame * picture = av_frame_alloc();
//    uint8_t * picture_buf = NULL;
//
//    if (!picture) {
//        return NULL;
//    }
//
//    int size = av_image_get_buffer_size(fmt, width, height, 64);
//    if (size < 0) {
//        return NULL;
//    }
//    int line = 0;
//    int createImageBufferResult = av_image_alloc(&picture_buf, &line, width, height, fmt, 64);
//    if (!picture_buf) {
//        av_frame_free(&picture);
//        return NULL;
//    }
//    av_image_fill_arrays(<#uint8_t **dst_data#>, <#int *dst_linesize#>, <#const uint8_t *src#>, <#enum AVPixelFormat pix_fmt#>, <#int width#>, <#int height#>, <#int align#>)
//    avpicture_fill((AVPicture *)picture, picture_buf,
//                   pix_fmt, width, height);
//    return picture;
//}

//- (void)makep:(CGImageRef)imageRef {
//    const uint32_t * data = (const uint32_t *)(self.ptr);
//
//    size_t frameWidth = self.width;
//    size_t frameHeight = self.height;
//    size_t bytesPerRow = self.width * 4;
//
//    NSDictionary *options = @{(id)kCVPixelBufferIOSurfacePropertiesKey : @{}};
//    CVPixelBufferRef pixelBuffer = NULL;
//    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault, frameWidth, frameHeight, kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange, (__bridge CFDictionaryRef)(options), &pixelBuffer);
//    NSParameterAssert(status == kCVReturnSuccess && pixelBuffer != NULL);
//
//    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
//    size_t width = CVPixelBufferGetWidth(pixelBuffer);
//    size_t height = CVPixelBufferGetHeight(pixelBuffer);
//    size_t bpr = CVPixelBufferGetBytesPerRow(pixelBuffer);
//
//    size_t wh = width * height;
//    size_t size = CVPixelBufferGetDataSize(pixelBuffer);
//    size_t count = CVPixelBufferGetPlaneCount(pixelBuffer);
//    assert(count == 2);
//    size_t width0 = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
//    NSLog(@"width0:%lu",width0);
//    size_t height0 = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
//    NSLog(@"height0:%lu",height0);
//    size_t bpr0 = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
//    NSLog(@"bpr0:%lu",bpr0);
//
//    NSLog(@"\n");
//
//    size_t width1 = CVPixelBufferGetWidthOfPlane(pixelBuffer, 1);
//    NSLog(@"width1:%lu",width1);
//    size_t height1 = CVPixelBufferGetHeightOfPlane(pixelBuffer, 1);
//    NSLog(@"height1:%lu",height1);
//    size_t bpr1 = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
//    NSLog(@"bpr1:%lu",bpr1);
//
//    unsigned char *bufY = malloc(wh);
//    unsigned char *bufUV = malloc(wh/2);
//
//    size_t offset,p;
//
//    int r,g,b,y,u,v;
//    int a=255;
//    for (int row = 0; row < height; ++row) {
//      for (int col = 0; col < width; ++col) {
//        //
//        offset = ((width * row) + col);
//        p = offset*4;
//        //
//        r = data[p + 0];
//        g = data[p + 1];
//        b = data[p + 2];
//        a = data[p + 3];
//        //
//        y = 0.299*r + 0.587*g + 0.114*b;
//        u = -0.1687*r - 0.3313*g + 0.5*b + 128;
//        v = 0.5*r - 0.4187*g - 0.0813*b + 128;
//        //
//        bufY[offset] = y;
//        bufUV[(row/2)*width + (col/2)*2] = u;
//        bufUV[(row/2)*width + (col/2)*2 + 1] = v;
//      }
//    }
//    uint8_t *yPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
//    memset(yPlane, 0x80, height0 * bpr0);
//    for (int row=0; row<height0; ++row) {
//      memcpy(yPlane + row*bpr0, bufY + row*width0, width0);
//    }
//    uint8_t *uvPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
//    memset(uvPlane, 0x80, height1 * bpr1);
//    for (int row=0; row<height1; ++row) {
//      memcpy(uvPlane + row*bpr1, bufUV + row*width, width);
//    }
//
//    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//    free(bufY);
//    free(bufUV);
//
//    NSLog(@"bitsPerPixel:%lu",bitsPerPixel);
//
//    NSLog(@"bitsPerComponent:%lu",bitsPerComponent);
//
//    NSLog(@"\n");
//
//    NSLog(@"frameWidth:%lu",frameWidth);
//
//    NSLog(@"frameHeight:%lu",frameHeight);
//
//    NSLog(@"bytesPerRow:%lu ==:%lu",bytesPerRow,bytesPerRow/4);
//
//    NSLog(@"\n");
//    NSLog(@"width:%lu",width);
//
//    NSLog(@"height:%lu",height);
//    NSLog(@"bpr:%lu",bpr);
//
//    NSLog(@"\n");
//
//
//    CFRelease(pixelData);
//}




//
//- (void)makep:(CGImageRef)imageRef {
//    CGDataProviderRef provider = CGImageGetDataProvider(imageRef);
//    CFDataRef pixelData = CGDataProviderCopyData(provider);
//    const UInt8 * data = CFDataGetBytePtr(pixelData);
//
//    size_t bitsPerPixel = CGImageGetBitsPerPixel(imageRef);
//    size_t bitsPerComponent = CGImageGetBitsPerComponent(imageRef);
//
//    size_t frameWidth = CGImageGetWidth(imageRef);
//    size_t frameHeight = CGImageGetHeight(imageRef);
//    size_t bytesPerRow = CGImageGetBytesPerRow(imageRef);
//
//    NSDictionary *options = @{(id)kCVPixelBufferIOSurfacePropertiesKey : @{}};
//    CVPixelBufferRef pixelBuffer = NULL;
//    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault, frameWidth, frameHeight, kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange, (__bridge CFDictionaryRef)(options), &pixelBuffer);
//    NSParameterAssert(status == kCVReturnSuccess && pixelBuffer != NULL);
//
//    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
//    size_t width = CVPixelBufferGetWidth(pixelBuffer);
//    size_t height = CVPixelBufferGetHeight(pixelBuffer);
//    size_t bpr = CVPixelBufferGetBytesPerRow(pixelBuffer);
//
//    size_t wh = width * height;
//    size_t size = CVPixelBufferGetDataSize(pixelBuffer);
//    size_t count = CVPixelBufferGetPlaneCount(pixelBuffer);
//    assert(count == 2);
//    size_t width0 = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
//    NSLog(@"width0:%lu",width0);
//    size_t height0 = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
//    NSLog(@"height0:%lu",height0);
//    size_t bpr0 = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
//    NSLog(@"bpr0:%lu",bpr0);
//
//    NSLog(@"\n");
//
//    size_t width1 = CVPixelBufferGetWidthOfPlane(pixelBuffer, 1);
//    NSLog(@"width1:%lu",width1);
//    size_t height1 = CVPixelBufferGetHeightOfPlane(pixelBuffer, 1);
//    NSLog(@"height1:%lu",height1);
//    size_t bpr1 = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
//    NSLog(@"bpr1:%lu",bpr1);
//
//    unsigned char *bufY = malloc(wh);
//    unsigned char *bufUV = malloc(wh/2);
//
//    size_t offset,p;
//
//    int r,g,b,y,u,v;
//    int a=255;
//    for (int row = 0; row < height; ++row) {
//      for (int col = 0; col < width; ++col) {
//        //
//        offset = ((width * row) + col);
//        p = offset*4;
//        //
//        r = data[p + 0];
//        g = data[p + 1];
//        b = data[p + 2];
//        a = data[p + 3];
//        //
//        y = 0.299*r + 0.587*g + 0.114*b;
//        u = -0.1687*r - 0.3313*g + 0.5*b + 128;
//        v = 0.5*r - 0.4187*g - 0.0813*b + 128;
//        //
//        bufY[offset] = y;
//        bufUV[(row/2)*width + (col/2)*2] = u;
//        bufUV[(row/2)*width + (col/2)*2 + 1] = v;
//      }
//    }
//    uint8_t *yPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
//    memset(yPlane, 0x80, height0 * bpr0);
//    for (int row=0; row<height0; ++row) {
//      memcpy(yPlane + row*bpr0, bufY + row*width0, width0);
//    }
//    uint8_t *uvPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
//    memset(uvPlane, 0x80, height1 * bpr1);
//    for (int row=0; row<height1; ++row) {
//      memcpy(uvPlane + row*bpr1, bufUV + row*width, width);
//    }
//
//    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//    free(bufY);
//    free(bufUV);
//
//    NSLog(@"bitsPerPixel:%lu",bitsPerPixel);
//
//    NSLog(@"bitsPerComponent:%lu",bitsPerComponent);
//
//    NSLog(@"\n");
//
//    NSLog(@"frameWidth:%lu",frameWidth);
//
//    NSLog(@"frameHeight:%lu",frameHeight);
//
//    NSLog(@"bytesPerRow:%lu ==:%lu",bytesPerRow,bytesPerRow/4);
//
//    NSLog(@"\n");
//    NSLog(@"width:%lu",width);
//
//    NSLog(@"height:%lu",height);
//    NSLog(@"bpr:%lu",bpr);
//
//    NSLog(@"\n");
//
//
//    CFRelease(pixelData);
//}




@end
