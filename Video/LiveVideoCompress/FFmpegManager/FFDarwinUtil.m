//
//  FFDarwinUtil.m
//  LiveVideoCompress
//
//  Created by vector on 2020/3/3.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "FFDarwinUtil.h"
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <mach/mach_time.h>
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#import "libyuv/libyuv.h"

@implementation FFDarwinUtil

static OSType KVideoPixelFormatType = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;

//+ (CVPixelBufferRef)yuvPixelBufferWithData:(NSData *)dataFrame
//                         presentationTime:(CMTime)presentationTime
//                                    width:(size_t)width
//                                   heigth:(size_t)heigth {
//    unsigned char* buffer = (unsigned char*) dataFrame.bytes;
//    CVPixelBufferRef getCroppedPixelBuffer = [self copyDataFromBuffer:buffer toYUVPixelBufferWithWidth:width Height:heigth];
//    return getCroppedPixelBuffer;
//}
//
//+ (CVPixelBufferRef)copyDataFromBuffer:(const unsigned char*)buffer toYUVPixelBufferWithWidth:(size_t)w Height:(size_t)h {
////通过宏使模拟器能够通过编译
//#if TARGET_IPHONE_SIMULATOR
//    NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSDictionary dictionary],kCVPixelBufferOpenGLESTextureCacheCompatibilityKey,
//                                           nil];
//#else
//    NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSDictionary dictionary],kCVPixelBufferIOSurfaceOpenGLESTextureCompatibilityKey,
//                                           nil];
//#endif
//
//
//    CVPixelBufferRef pixelBuffer;
//    CVPixelBufferCreate(NULL, w, h, KVideoPixelFormatType, (__bridge CFDictionaryRef)(pixelBufferAttributes), &pixelBuffer);
//
//    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
//
//    size_t d = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
//    const unsigned char* src = buffer;
//    unsigned char* dst = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
//
//    for (unsigned int rIdx = 0; rIdx < h; ++rIdx, dst += d, src += w) {
//        memcpy(dst, src, w);
//    }
//
//    d = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
//    dst = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
//    h = h >> 1;
//    for (unsigned int rIdx = 0; rIdx < h; ++rIdx, dst += d, src += w) {
//        memcpy(dst, src, w);
//    }
//
//    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//
//    return pixelBuffer;
//}
//
//
//- (CVPixelBufferRef) newPixelBufferFromCGImage: (CGImageRef) image
//{
//    CVPixelBufferRef pxbuffer = NULL;
//
//    CFDictionaryRef option = (__bridge_retained CFDictionaryRef)_videoSettings;
//
//    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault,
//                                          self.options.videoWidth,
//                                          self.options.videoHeight,
//                                          kCVPixelFormatType_32ARGB,
//                                          option,
//                                          &pxbuffer
//                                          );
//
//    CFRelease(option);
//
//    NSParameterAssert(status == kCVReturnSuccess && pxbuffer != NULL);
//
//    CVPixelBufferLockBaseAddress(pxbuffer, 0);
//    void *pxdata = CVPixelBufferGetBaseAddress(pxbuffer);
//
//    NSParameterAssert(pxdata != NULL);
//
//    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
//
//#if __IPHONE_OS_VERSION_MAX_ALLOWED > __IPHONE_6_1
//    int bitmapInfo = (CGBitmapInfo)kCGImageAlphaNoneSkipFirst;
//#else
//    int bitmapInfo = kCGImageAlphaNoneSkipFirst;
//#endif
//
//    CGContextRef context = CGBitmapContextCreate(pxdata,
//                                                 self.options.videoWidth,
//                                                 self.options.videoHeight,
//                                                 8,
//                                                 4*self.options.videoWidth,
//                                                 rgbColorSpace,
//                                                 bitmapInfo
//                                                 );
//
//    //    CGContextRef context = CGBitmapContextCreate(pxdata, self.size.width, self.size.height, 8, 4*self.size.width, rgbColorSpace,kCGImageAlphaNoneSkipFirst);
//
//    NSParameterAssert(context);
//
//    //    CGFloat iw = CGImageGetWidth(image);
//    CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
//    CGContextFillRect(context, CGRectMake(0, 0, self.options.videoWidth, self.options.videoHeight));
//
//    CGContextDrawImage(context, CGRectMake(0, 0, self.options.videoWidth, self.options.videoHeight), image);
//
//    CGColorSpaceRelease(rgbColorSpace);
//    CGContextRelease(context);
//
//    CVPixelBufferUnlockBaseAddress(pxbuffer, 0);
//
//    return pxbuffer;
//}



//- (void)aaa {
//    NSDictionary *options = @{(id)kCVPixelBufferIOSurfacePropertiesKey : @{}};
//    CVPixelBufferRef pixelBuffer = NULL;
//    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault, frameWidth, frameHeight, kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange, (__bridge CFDictionaryRef)(options), &pixelBuffer);
//    NSParameterAssert(status == kCVReturnSuccess && pixelBuffer != NULL);
//        
//    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
//        
//    NSLog(@"\n");
//        
//    size_t width = CVPixelBufferGetWidth(pixelBuffer);
//    NSLog(@"width:%lu",width);
//    size_t height = CVPixelBufferGetHeight(pixelBuffer);
//    NSLog(@"height:%lu",height);
//    size_t bpr = CVPixelBufferGetBytesPerRow(pixelBuffer);
//    NSLog(@"bpr:%lu",bpr);
//        
//    NSLog(@"\n");
//        
//    size_t wh = width * height;
//    NSLog(@"wh:%lu\n",wh);
//    size_t size = CVPixelBufferGetDataSize(pixelBuffer);
//    NSLog(@"size:%lu",size);
//    size_t count = CVPixelBufferGetPlaneCount(pixelBuffer);
//    NSLog(@"count:%lu\n",count);
//        
//    NSLog(@"\n");
//        
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
//}


//- (void)rr {
//    AVFrame * bitmap = avcodec_alloc_fra
//    
//    
//    AVPacket pkt;
//    av_init_packet(&pkt);
//
//    pkt.flags |= PKT_FLAG_KEY;
//    pkt.stream_index= st->index;
//    pkt.data= (uint8_t *)picture;
//    pkt.size= sizeof(AVPicture);
//
//    ret = av_interleaved_write_frame(oc, &pkt);
//}

@end

//AVFrame * alloc_picture(int pix_fmt, int width, int height) {
//    AVFrame * picture;
//    uint8_t *picture_buf;
//    int size;
//    av_frame_alloc();
//    picture = avcodec_alloc_frame();
//    if (!picture)
//        return NULL;
//    size = avpicture_get_size(pix_fmt, width, height);
//    picture_buf = av_malloc(size);
//    if (!picture_buf) {
//        av_free(picture);
//        return NULL;
//    }
//    avpicture_fill((AVPicture *)picture, picture_buf,
//                   pix_fmt, width, height);
//    return picture;
//}
