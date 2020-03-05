//
//  FFDarwinUtil.h
//  LiveVideoCompress
//
//  Created by vector on 2020/3/3.
//  Copyright © 2020 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/*
 kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange = '420v'，表示输出的视频格式为NV12；范围： (luma=[16,235] chroma=[16,240])
 kCVPixelFormatType_420YpCbCr8BiPlanarFullRange = '420f'，表示输出的视频格式为NV12；范围： (luma=[0,255] chroma=[1,255])
 kCVPixelFormatType_32BGRA = 'BGRA', 输出的是BGRA的格式
 */


//kCVPixelFormatType_422YpCbCr16
@interface FFDarwinUtil : NSObject



@end
//
//NSDictionary *options = @{(id)kCVPixelBufferIOSurfacePropertiesKey : @{}};
//CVPixelBufferRef pixelBuffer = NULL;
//CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault, frameWidth, frameHeight, kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange, (__bridge CFDictionaryRef)(options), &pixelBuffer);
//NSParameterAssert(status == kCVReturnSuccess && pixelBuffer != NULL);
//    
//CVPixelBufferLockBaseAddress(pixelBuffer, 0);
//    
//NSLog(@"\n");
//    
//size_t width = CVPixelBufferGetWidth(pixelBuffer);
//NSLog(@"width:%lu",width);
//size_t height = CVPixelBufferGetHeight(pixelBuffer);
//NSLog(@"height:%lu",height);
//size_t bpr = CVPixelBufferGetBytesPerRow(pixelBuffer);
//NSLog(@"bpr:%lu",bpr);
//    
//NSLog(@"\n");
//    
//size_t wh = width * height;
//NSLog(@"wh:%lu\n",wh);
//size_t size = CVPixelBufferGetDataSize(pixelBuffer);
//NSLog(@"size:%lu",size);
//size_t count = CVPixelBufferGetPlaneCount(pixelBuffer);
//NSLog(@"count:%lu\n",count);
//    
//NSLog(@"\n");
//    
//size_t width0 = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
//NSLog(@"width0:%lu",width0);
//size_t height0 = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
//NSLog(@"height0:%lu",height0);
//size_t bpr0 = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
//NSLog(@"bpr0:%lu",bpr0);
//    
//NSLog(@"\n");
//    
//size_t width1 = CVPixelBufferGetWidthOfPlane(pixelBuffer, 1);
//NSLog(@"width1:%lu",width1);
//size_t height1 = CVPixelBufferGetHeightOfPlane(pixelBuffer, 1);
//NSLog(@"height1:%lu",height1);
//size_t bpr1 = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
//NSLog(@"bpr1:%lu",bpr1);
//    
//unsigned char *bufY = malloc(wh);
//unsigned char *bufUV = malloc(wh/2);
//
//size_t offset,p;
//
//int r,g,b,y,u,v;
//int a=255;
//for (int row = 0; row < height; ++row) {
//  for (int col = 0; col < width; ++col) {
//    //
//    offset = ((width * row) + col);
//    p = offset*4;
//    //
//    r = data[p + 0];
//    g = data[p + 1];
//    b = data[p + 2];
//    a = data[p + 3];
//    //
//    y = 0.299*r + 0.587*g + 0.114*b;
//    u = -0.1687*r - 0.3313*g + 0.5*b + 128;
//    v = 0.5*r - 0.4187*g - 0.0813*b + 128;
//    //
//    bufY[offset] = y;
//    bufUV[(row/2)*width + (col/2)*2] = u;
//    bufUV[(row/2)*width + (col/2)*2 + 1] = v;
//  }
//}
//uint8_t *yPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
//memset(yPlane, 0x80, height0 * bpr0);
//for (int row=0; row<height0; ++row) {
//  memcpy(yPlane + row*bpr0, bufY + row*width0, width0);
//}
//uint8_t *uvPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
//memset(uvPlane, 0x80, height1 * bpr1);
//for (int row=0; row<height1; ++row) {
//  memcpy(uvPlane + row*bpr1, bufUV + row*width, width);
//}
//    
//CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//free(bufY);
//free(bufUV);


NS_ASSUME_NONNULL_END
