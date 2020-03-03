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

#import "libyuv/libyuv.h"

@implementation FFDarwinUtil

static OSType KVideoPixelFormatType = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;

-(CVPixelBufferRef)yuvPixelBufferWithData:(NSData *)dataFrame
                         presentationTime:(CMTime)presentationTime
                                    width:(size_t)w
                                   heigth:(size_t)h
{
    unsigned char* buffer = (unsigned char*) dataFrame.bytes;
    CVPixelBufferRef getCroppedPixelBuffer = [self copyDataFromBuffer:buffer toYUVPixelBufferWithWidth:w Height:h];
    return getCroppedPixelBuffer;
}


-(CVPixelBufferRef)copyDataFromBuffer:(const unsigned char*)buffer toYUVPixelBufferWithWidth:(size_t)w Height:(size_t)h
{
//通过宏使模拟器能够通过编译
#if TARGET_IPHONE_SIMULATOR
    NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSDictionary dictionary],kCVPixelBufferOpenGLESTextureCacheCompatibilityKey,
                                           nil];
#else
    NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSDictionary dictionary],kCVPixelBufferIOSurfaceOpenGLESTextureCompatibilityKey,
                                           nil];
#endif


    CVPixelBufferRef pixelBuffer;
    CVPixelBufferCreate(NULL, w, h, KVideoPixelFormatType, (__bridge CFDictionaryRef)(pixelBufferAttributes), &pixelBuffer);

    CVPixelBufferLockBaseAddress(pixelBuffer, 0);

    size_t d = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
    const unsigned char* src = buffer;
    unsigned char* dst = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);

    for (unsigned int rIdx = 0; rIdx < h; ++rIdx, dst += d, src += w) {
        memcpy(dst, src, w);
    }

    d = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
    dst = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    h = h >> 1;
    for (unsigned int rIdx = 0; rIdx < h; ++rIdx, dst += d, src += w) {
        memcpy(dst, src, w);
    }

    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);

    return pixelBuffer;

}


- (CVPixelBufferRef) newPixelBufferFromCGImage: (CGImageRef) image
{
    CVPixelBufferRef pxbuffer = NULL;

    CFDictionaryRef option = (__bridge_retained CFDictionaryRef)_videoSettings;

    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault,
                                          self.options.videoWidth,
                                          self.options.videoHeight,
                                          kCVPixelFormatType_32ARGB,
                                          option,
                                          &pxbuffer
                                          );

    CFRelease(option);

    NSParameterAssert(status == kCVReturnSuccess && pxbuffer != NULL);

    CVPixelBufferLockBaseAddress(pxbuffer, 0);
    void *pxdata = CVPixelBufferGetBaseAddress(pxbuffer);

    NSParameterAssert(pxdata != NULL);

    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();

#if __IPHONE_OS_VERSION_MAX_ALLOWED > __IPHONE_6_1
    int bitmapInfo = (CGBitmapInfo)kCGImageAlphaNoneSkipFirst;
#else
    int bitmapInfo = kCGImageAlphaNoneSkipFirst;
#endif

    CGContextRef context = CGBitmapContextCreate(pxdata,
                                                 self.options.videoWidth,
                                                 self.options.videoHeight,
                                                 8,
                                                 4*self.options.videoWidth,
                                                 rgbColorSpace,
                                                 bitmapInfo
                                                 );

    //    CGContextRef context = CGBitmapContextCreate(pxdata, self.size.width, self.size.height, 8, 4*self.size.width, rgbColorSpace,kCGImageAlphaNoneSkipFirst);

    NSParameterAssert(context);

    //    CGFloat iw = CGImageGetWidth(image);
    CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
    CGContextFillRect(context, CGRectMake(0, 0, self.options.videoWidth, self.options.videoHeight));

    CGContextDrawImage(context, CGRectMake(0, 0, self.options.videoWidth, self.options.videoHeight), image);

    CGColorSpaceRelease(rgbColorSpace);
    CGContextRelease(context);

    CVPixelBufferUnlockBaseAddress(pxbuffer, 0);

    return pxbuffer;
}


@end
