//
//  SCAVMixHandler.m
//  AV
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "SCAVMixHandler.h"
#import <UIKit/UIKit.h>


typedef NS_ENUM(NSUInteger, SCAVWriterInputHandlerStatus) {
    SCAVWriterInputHandlerStatusNone,
    SCAVWriterInputHandlerStatusRunning,
    SCAVWriterInputHandlerStatusFinished,
    SCAVWriterInputHandlerStatusCanceled,
    SCAVWriterInputHandlerStatusFailure,
};
typedef NS_ENUM(NSUInteger, SCAVWriterInputHandlerKind) {
    SCAVWriterInputHandlerKindUnknown,
    SCAVWriterInputHandlerKindVideo,
    SCAVWriterInputHandlerKindAudio,
};

@class SCAVWriterInputHandler;
typedef void(^SCAVWriterInputHandlerBlock)(SCAVWriterInputHandler * handler);
@interface SCAVWriterInputHandler : NSObject

@property (atomic, assign) SCAVWriterInputHandlerKind kind;
@property (atomic, assign) SCAVWriterInputHandlerStatus status;


@property (nonatomic, strong) AVAssetReader * assetReader;
@property (nonatomic, strong) AVAssetWriter * assetWriter;

@property (nonatomic, strong) dispatch_queue_t queue;
@property (nonatomic, strong) AVAssetWriterInput * writerInput;
@property (nonatomic, strong) AVAssetReaderTrackOutput * readerTrackOutput;

@property (nonatomic, copy) SCAVWriterInputHandlerBlock completion;
@property (nonatomic, copy) NSString * tag;

@property (atomic, assign) NSInteger breakFlag;// > 0 标识取消， < 0 标识取消完成


//参数全部非空
- (instancetype)initWithWriterInput:(AVAssetWriterInput *)writerInput
                  readerTrackOutput:(AVAssetReaderTrackOutput *)readerTrackOutput
                              queue:(dispatch_queue_t)queue;

- (BOOL)startWithCompletion:(SCAVWriterInputHandlerBlock)completion;
- (BOOL)finish:(BOOL)isSuccess;
- (void)cancel;

@end
@implementation SCAVWriterInputHandler

//参数全部非空
- (instancetype)initWithWriterInput:(AVAssetWriterInput *)writerInput
                  readerTrackOutput:(AVAssetReaderTrackOutput *)readerTrackOutput
                              queue:(dispatch_queue_t)queue {
    self = [super init];
    if (self) {
        self.writerInput = writerInput;
        self.readerTrackOutput = readerTrackOutput;
        self.queue = queue;
    }
    return self;
}

- (void)dealloc {
    NSLog(@"SCAVWriterInputHandler dealloc");
}

- (BOOL)startWithCompletion:(SCAVWriterInputHandlerBlock)completion {
    if (SCAVWriterInputHandlerStatusNone == self.status) {
        self.status = SCAVWriterInputHandlerStatusRunning;
        self.completion = completion;
        [self assertReadToAssetInput];
        return true;
    } else {
        return false;
    }
}

- (BOOL)finish:(BOOL)isSuccess {
    if (SCAVWriterInputHandlerStatusRunning == self.status) {
        [self.writerInput markAsFinished];
        if (isSuccess) {
            self.status = SCAVWriterInputHandlerStatusFinished;
        } else {
            self.status = SCAVWriterInputHandlerStatusFailure;
        }
        
        if (self.completion) {
            self.completion(self);
        }
        self.completion = nil;
        return true;
    } else {
        return false;
    }
}

- (void)cancel {
    self.breakFlag += 1;

    dispatch_async(self.queue, ^{
        if (SCAVWriterInputHandlerStatusRunning == self.status) {
            [self.writerInput markAsFinished];
            self.status = SCAVWriterInputHandlerStatusCanceled;
            if (self.completion) {
                self.completion(self);
            }
            self.completion = nil;
        }
    });
}

//- (CGContextRef)creatBitmapContextWithPixelBuffer:(CVPixelBufferRef)pixelBuffer {
//    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
//    CGContextRef bitmapContext = NULL;
//    CGSize _viewSize = [self videoSize];
//    bitmapContext = CGBitmapContextCreate(CVPixelBufferGetBaseAddress(*pixelBuffer),
//                                          CVPixelBufferGetWidth(*pixelBuffer),
//                                          CVPixelBufferGetHeight(*pixelBuffer),
//                                          8, CVPixelBufferGetBytesPerRow(*pixelBuffer), _colorSpace,
//                                          kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst
//                                          );
//    CGAffineTransform flipVertical = CGAffineTransformMake(1, 0, 0, -1, 0, _viewSize.height);
//    CGContextConcatCTM(bitmapContext, flipVertical);
//
//    return bitmapContext;
//}

+ (CGColorSpaceRef)colorSpace {
    static CGColorSpaceRef __colorSpace;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __colorSpace = CGColorSpaceCreateDeviceRGB();
    });
    return __colorSpace;
}

- (BOOL)addImageFromSampleBuffer:(CMSampleBufferRef) sampleBuffer {
    // 为媒体数据设置一个CMSampleBuffer的Core Video图像缓存对象

    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    // 锁定pixel buffer的基地址
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    
    // 得到pixel buffer的基地址
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    
    // 得到pixel buffer的行字节数
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    // 得到pixel buffer的宽和高
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    
    // 创建一个依赖于设备的RGB颜色空间
    CGColorSpaceRef colorSpace = [[self class] colorSpace];
//    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    // 用抽样缓存的数据创建一个位图格式的图形上下文（graphics context）对象
    CGContextRef bitmapContext = CGBitmapContextCreate(baseAddress, width, height, 8,
                                                 bytesPerRow, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
//    CGColorSpaceRelease(colorSpace);
    if (NULL == bitmapContext) {
        NSLog(@"bitmap is nil");
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
        return false;
    }

    UIGraphicsPushContext(bitmapContext);
    CGRect frame = CGRectMake(10, 10, 76, 76);
    CGRect rect = frame;
    rect.origin.y = height - frame.origin.y - frame.size.height;
    
    CGContextDrawImage(bitmapContext, rect, [UIImage imageNamed:@"ipad app-76.png"].CGImage);
    UIGraphicsPopContext();
    CGContextRelease(bitmapContext);
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    
    // 释放context和颜色空间
    return true;
}

- (CMSampleBufferRef _Nullable)nextSampleBuffer {
    CMSampleBufferRef buffer = [self.readerTrackOutput copyNextSampleBuffer];
    BOOL isVideo = (SCAVWriterInputHandlerKindVideo == self.kind);
    if (isVideo) {
        [self addImageFromSampleBuffer:buffer];
    }
    return buffer;
}

-(void)assertReadToAssetInput {
    if (SCAVWriterInputHandlerStatusRunning != self.status) {
        return;
    }
    /*
     执行效率无法提高， 目前找不到能提升效率的地方
     尝试过
     1: io优化 CMSampleBufferRef 预加载20帧，readyForMoreMediaData 中只做拼接， !readyForMoreMediaData时 预加载， 效果微乎其微
     2 video audio 分离在不同的queue中； 起反效果，时间多了0.2秒
     */
    dispatch_queue_t queue = self.queue;
    NSString * tag = self.tag ?: @"";
    
    __block NSInteger sequence = 0;
    NSInteger logStepLength = 120;
    
    AVAssetWriterInput * writerInput = self.writerInput;
    
    AVAssetReader * assetReader = self.assetReader;
    AVAssetWriter * assetWriter = self.assetWriter;

    [self.writerInput requestMediaDataWhenReadyOnQueue:queue usingBlock:^{
        NSInteger breakFlag = self.breakFlag;
        while (writerInput.readyForMoreMediaData) {
            if (SCAVWriterInputHandlerStatusRunning != self.status) {
                return;
            }
            CMSampleBufferRef buffer = [self nextSampleBuffer];
            if (buffer) {
                BOOL appendResult = [writerInput appendSampleBuffer:buffer];

                CFRelease(buffer);

                if (appendResult) {
                    sequence += 1;
#if DEBUG
                    //状态log
                    if (sequence % logStepLength == 0) {
                        printf("[%s a %ld]", tag.UTF8String, sequence);
                    }
#endif
                } else {
                    AVAssetReaderStatus readStatus = assetReader.status;
                    AVAssetWriterStatus writeStatus = assetWriter.status;
                    
                    if (AVAssetReaderStatusCompleted == readStatus || AVAssetWriterStatusCompleted == writeStatus) {
                        [self finish:true];
                    } else {
                        [self finish:false];
                    }
                }
            } else {
                printf("[%s: input finish %ld]", tag.UTF8String, sequence);
                [self finish:true];
            }
            if (self.breakFlag != breakFlag) {
                break;
            }
        }
    }];
}

@end



#import "SCAVMixManager.h"

@interface SCAVMixHandler ()

@property (nonatomic, readonly) NSString * name;

@property (nonatomic, assign) SCAVMixHandlerStatus status;
@property (nonatomic, copy) SCAVMixHandlerBlock completion;

@property (nonatomic, strong) AVAsset * videoAsset;
@property (nonatomic, strong) AVAsset * audioAsset;

@property (nonatomic, strong) AVAssetReader * assetReader;

@property (nonatomic, strong) AVAssetReaderTrackOutput * videoOutPut;
@property (nonatomic, strong) AVAssetReaderTrackOutput * audioOutPut;


@property (nonatomic, strong) AVAssetWriter * assetWriter;

@property (nonatomic, strong) SCAVWriterInputHandler * videoWriterInputHandler;
@property (nonatomic, strong) SCAVWriterInputHandler * audioWriterInputHandler;


@property (nonatomic, copy) NSURL * videoInputUrl;
@property (nonatomic, copy) NSURL * audioInputUrl;

//文件临时输出地方， 最后成功后移动文件位置
@property (nonatomic, copy) NSString * tmpOutputPath;
@property (nonatomic, copy) NSString * outputCachePath;

@property (nonatomic, copy) NSURL * tmpOutputUrl;
@property (nonatomic, strong) dispatch_queue_t queue;

@property (nonatomic, assign) CGSize videoSize;

@end
@implementation SCAVMixHandler

- (NSString *)name {
    return self.config.identifier;
}

- (instancetype)initWithConfig:(SCAVMixConfig *)config queue:(dispatch_queue_t)queue {
    self = [super init];
    if (self) {
        self.config = config;
        self.queue = queue;
        
        self.videoInputUrl = [NSURL fileURLWithPath:config.videoInputPath];
        self.audioInputUrl = [NSURL fileURLWithPath:config.audioInputPath];
        
        NSUUID * uuid = [NSUUID UUID];
        [[NSFileManager defaultManager] createDirectoryAtPath:[SCAVMixManager tmpDirectoryPathForUuid:uuid] withIntermediateDirectories:true attributes:nil error:NULL];
        _tmpOutputPath = [SCAVMixManager tmpPathForIdentifier:config.identifier uuid:uuid];
        self.tmpOutputUrl = [NSURL fileURLWithPath:_tmpOutputPath];
        
//        self.writerInput = writerInput;
//        self.readerTrackOutput = readerTrackOutput;
//        self.queue = queue;
//        self.completion = completion;
    }
    return self;
}

- (BOOL)prepare {
    BOOL result = [self configAssetReader];
    if (result) {
        result = [self configWriteInput];
    }
    if (result) {
        result = [_assetReader startReading];
    }
    if (result) {
        result = [_assetWriter startWriting];
    }
    if (!result) {
        NSLog(@"prepare failure");
    }
    return result;
}

- (BOOL)startWithCompletion:(SCAVMixHandlerBlock)completion {
    BOOL result = false;
    if (SCAVMixHandlerStatusNone == self.status) {
        
        
        self.status = SCAVMixHandlerStatusRunning;
        self.completion = completion;
        result = [self prepare];
        if (result) {
            [self assertReadToAssetInput];
        }
    }
    return result;
}

- (void)complete {
    if (self.completion) {
        self.completion(self);
    }
}

- (void)cancel {
    if (SCAVMixHandlerStatusRunning == self.status) {
        self.status = SCAVMixHandlerStatusCanceled;
        [self.audioWriterInputHandler cancel];
        [self.videoWriterInputHandler cancel];

        [self complete];
    }
}

- (BOOL)configAssetReader {
    NSURL * videoUrl = self.videoInputUrl;
    NSURL * audioUrl = self.audioInputUrl;

    // 时间起点
    CMTime beginTime = CMTimeMakeWithSeconds(0, 15);
    
    CMTime nextClistartTime = kCMTimeZero;
    // 创建可变的音视频组合
    
    AVMutableComposition *comosition = [AVMutableComposition composition];

    //    AVMutableVideoComposition *comosition = [AVMutableVideoComposition composition];
    
    
    // 视频采集
    AVURLAsset * videoAsset = [[AVURLAsset alloc] initWithURL:videoUrl options:nil];
    // 视频时间范围
    
    CMTimeRange videoTimeRange = CMTimeRangeMake(beginTime, videoAsset.duration);
    // 视频通道 枚举 kCMPersistentTrackID_Invalid = 0
    AVMutableCompositionTrack *videoTrack = [comosition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
    // 视频采集通道
    AVAssetTrack *videoAssetTrack = [[videoAsset tracksWithMediaType:AVMediaTypeVideo] firstObject];
    //  把采集轨道数据加入到可变轨道之中
    if (nil == videoAssetTrack) {
        return false;
    }
    [videoTrack insertTimeRange:videoTimeRange ofTrack:videoAssetTrack atTime:nextClistartTime error:nil];
    
    comosition.naturalSize = videoAssetTrack.naturalSize;
    self.videoSize = videoAssetTrack.naturalSize;
    
    
    // 声音采集
    AVURLAsset *audioAsset = [[AVURLAsset alloc] initWithURL:audioUrl options:nil];
    // 因为视频短这里就直接用视频长度了,如果自动化需要自己写判断
    CMTimeRange audioTimeRange = videoTimeRange;
    // 音频采集通道
    AVAssetTrack * audioAssetTrack = [[audioAsset tracksWithMediaType:AVMediaTypeAudio] firstObject];
    if (nil != audioAssetTrack) {
        // 音频通道
        AVMutableCompositionTrack *audioTrack = [comosition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];

        // 加入合成轨道之中
        [audioTrack insertTimeRange:audioTimeRange ofTrack:audioAssetTrack atTime:nextClistartTime error:nil];
    }
    
    //获取资源的一个视频轨道
    AVAssetTrack * track = [[comosition tracksWithMediaType:AVMediaTypeVideo] firstObject];
    if (nil == track) {
        return false;
    }
    
    NSError * error = nil;
    AVAssetReader * assetReader = [[AVAssetReader alloc] initWithAsset:comosition error:&error];
    if (nil == assetReader && nil != error) {
        NSLog(@"AVAssetReader initWithAsset error:%@", error);
        return false;
    }
    _assetReader = assetReader;
    //指定将读取的样本数据压缩为BGRA格式
    
    NSDictionary *setting =   @{(id)kCVPixelBufferPixelFormatTypeKey:@(kCVPixelFormatType_32BGRA)};
    
    //初始化输出，指定从track轨道中读取样本数据
    
    AVAssetReaderTrackOutput * videoOutPut = [[AVAssetReaderTrackOutput alloc] initWithTrack:track outputSettings:setting];
    self.videoOutPut = videoOutPut;

    //添加输出
    if ([_assetReader canAddOutput:videoOutPut]) {
        [_assetReader addOutput:videoOutPut];
    } else {
        NSLog(@"addOutput videoOutPut error");
        return false;
    }
    
    CMTime duration = audioAsset.duration;
    
    //获取资源的一个音频轨道
    AVAssetTrack * atrack = [[comosition tracksWithMediaType:AVMediaTypeAudio] firstObject];
    if (nil != atrack) {
        //    audioAsset.duration.timescale
        NSDictionary * audioSetting = @{ AVFormatIDKey : @(kAudioFormatLinearPCM),
                                         AVNumberOfChannelsKey : @(1),
                                         //                                     AVSampleRateKey : @(audioDurationSeconds)
                                         
                                         };
        
        
        AVAssetReaderTrackOutput * audioOutPut = [[AVAssetReaderTrackOutput alloc] initWithTrack:atrack outputSettings:audioSetting];
        self.audioOutPut = audioOutPut;
        
        if ([_assetReader canAddOutput:audioOutPut]) {
            [_assetReader addOutput:audioOutPut];
        } else {
            NSLog(@"addOutput audioOutPut error");
            return false;
        }
    }
    return true;
}
- (BOOL)configWriteInput {
    NSURL * outputUrl = self.tmpOutputUrl;
    NSError * error;
    CGSize videoSize = self.videoSize;
    
    _assetWriter = [[AVAssetWriter alloc] initWithURL:outputUrl fileType:AVFileTypeMPEG4 error:&error];
    NSInteger bitRat = videoSize.width * videoSize.height * 0.25;//0.75
    NSDictionary * videoSetting = @{
                                    AVVideoCodecKey : AVVideoCodecH264,
                                    //                              AVVideoCodecKey : AVVideoCodecTypeH264,
                                    //                              (id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA),
                                    AVVideoWidthKey:@(videoSize.width),
                                    AVVideoHeightKey:@(videoSize.height),
                                    AVVideoCompressionPropertiesKey:@{
                                            AVVideoMaxKeyFrameIntervalKey : @(90),
                                            AVVideoExpectedSourceFrameRateKey : @(15),
                                            AVVideoAverageBitRateKey : @(bitRat),
                                            AVVideoProfileLevelKey : AVVideoProfileLevelH264Main31,
                                            }
                                    
                                    };
    
    AVAssetWriterInput * videoWriterInput = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeVideo outputSettings:videoSetting];
    
    self.videoWriterInputHandler = [[SCAVWriterInputHandler alloc] initWithWriterInput:videoWriterInput readerTrackOutput:self.videoOutPut queue:self.queue];
    
    self.videoWriterInputHandler.kind = SCAVWriterInputHandlerKindVideo;
    self.videoWriterInputHandler.tag = @"v";
    //添加写入器
    if ([_assetWriter canAddInput:videoWriterInput]) {
        [_assetWriter addInput:videoWriterInput];
    } else {
        NSLog(@"canAddInput:videoWriterInputHandler");
        return false;
    }
    
    if (self.audioOutPut) {
        //    @{AVNumberOfChannelsKey:@2,
        //      AVSampleRateKey:@44100,
        //      AVLinearPCMBitDepthKey:@32,
        //      AVEncoderAudioQualityKey:@(AVAudioQualityMax),
        //      AVEncoderBitRateKey:@128000};
        
        NSDictionary * audioSetting = @{
                                        //                                    AVEncoderBitRatePerChannelKey : @(28000),
                                        AVFormatIDKey : @(kAudioFormatMPEG4AAC),
                                        AVNumberOfChannelsKey : @(1),
                                        AVEncoderAudioQualityKey : @(AVAudioQualityMedium),
                                        AVSampleRateKey : @(22050),//32000
                                        //                                     AVEncoderBitRateKey : @(1),
                                        };
        
        AVAssetWriterInput * audioWriterInput = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeAudio outputSettings:audioSetting];
        self.audioWriterInputHandler = [[SCAVWriterInputHandler alloc] initWithWriterInput:audioWriterInput readerTrackOutput:self.audioOutPut queue:self.queue];
        self.audioWriterInputHandler.kind = SCAVWriterInputHandlerKindAudio;
        self.audioWriterInputHandler.tag = @"a";
        
        //添加写入器
        if ([_assetWriter canAddInput:audioWriterInput]) {
            [_assetWriter addInput:audioWriterInput];
        } else {
            NSLog(@"canAddInput:audioWriterInputHandler");
            return false;
        }
    }
    
    _assetWriter.shouldOptimizeForNetworkUse = true;
    
    self.videoWriterInputHandler.assetReader = self.assetReader;
    self.videoWriterInputHandler.assetWriter = self.assetWriter;
    
    self.audioWriterInputHandler.assetReader = self.assetReader;
    self.audioWriterInputHandler.assetWriter = self.assetWriter;

    return true;
}

-(void)assertReadToAssetInput {
    NSLog(@"===>assertReadToAssetInput.begin: %@", self.name);
    dispatch_queue_t queue = dispatch_queue_create("com.writequeue", NULL);
    
    //开启写入会话，并指定样本的开始时间
    CFAbsoluteTime beginTime = CFAbsoluteTimeGetCurrent();
    [_assetWriter startSessionAtSourceTime:kCMTimeZero];
    dispatch_group_t group = dispatch_group_create();
    dispatch_group_enter(group);
    dispatch_group_enter(group);

    dispatch_group_notify(group, queue, ^{
        [self.assetWriter finishWritingWithCompletionHandler:^{
            CFAbsoluteTime endTime = CFAbsoluteTimeGetCurrent();
            
            AVAssetWriterStatus status = self.assetWriter.status;
            if (status == AVAssetWriterStatusCompleted) {
                NSString * filePath = self.tmpOutputUrl.path;
                NSFileManager * fm = [NSFileManager defaultManager];
                CGFloat fileSize = 0;
                if ([fm fileExistsAtPath:filePath]) {
                    NSDictionary * attributesOfItem = [fm attributesOfItemAtPath:filePath error:nil];
                    fileSize = (CGFloat)[attributesOfItem[NSFileSize] integerValue];
                }
                
                NSLog(@"===>assertReadToAssetInput.finsished: %@, filePath: %@, fileSize: %.6lfmb, time:%lf", self.name, filePath, fileSize, endTime - beginTime);
                
                //                    {
                //                        AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:self.config.outputUrl options:nil];
                //                        AVAssetImageGenerator *gen = [[AVAssetImageGenerator alloc] initWithAsset:asset];
                //                        gen.appliesPreferredTrackTransform = YES;
                //                        CMTime time = CMTimeMakeWithSeconds(0.0, 15);
                //                        NSError *error = nil;
                //                        CMTime actualTime;
                //                        CGImageRef image = [gen copyCGImageAtTime:time actualTime:&actualTime error:&error];
                //                        UIImage *img = [[UIImage alloc] initWithCGImage:image];
                //                        CGImageRelease(image);
                //                        [[NSNotificationCenter defaultCenter] postNotificationName:@"onImage" object:img];
                //                    }
                
            } else {
                NSLog(@"===>assertReadToAssetInput.failure: %@, error:%@", self.name, self.assetWriter.error);
            }
            
            BOOL audioBeOk = true;
            if (self.audioWriterInputHandler) {
                audioBeOk = (SCAVWriterInputHandlerStatusFinished == self.audioWriterInputHandler.status);
            }
            BOOL videoBeOk = true;
            if (self.videoWriterInputHandler) {
                audioBeOk = (SCAVWriterInputHandlerStatusFinished == self.videoWriterInputHandler.status);
            }
            BOOL result = (AVAssetWriterStatusCompleted == status && audioBeOk && videoBeOk);
            if (result) {
                NSError * error = nil;
                [[NSFileManager defaultManager] moveItemAtPath:self.tmpOutputPath toPath:self.config.outputPath error:&error];
                if (error) {
                    NSLog(@"moveItemAtPath:toPath: error: %@", error);
                } else {
                    NSLog(@"moveItemAtPath:toPath: success: %@", self.config.outputPath);
                }
                self.status = SCAVMixHandlerStatusFinished;
            } else {
                if (SCAVWriterInputHandlerStatusCanceled == self.audioWriterInputHandler.status || SCAVWriterInputHandlerStatusCanceled == self.videoWriterInputHandler.status) {
                    self.status = SCAVMixHandlerStatusCanceled;
                } else {
                    self.status = SCAVMixHandlerStatusFailure;
                }
            }
            if (self.completion) {
                self.completion(self);
            }
        }];
    });
    
    [self.videoWriterInputHandler startWithCompletion:^(SCAVWriterInputHandler *handler) {
        dispatch_group_leave(group);
    }];
    if (self.audioWriterInputHandler) {
        [self.audioWriterInputHandler startWithCompletion:^(SCAVWriterInputHandler *handler) {
            dispatch_group_leave(group);
        }];
    } else {
        dispatch_group_leave(group);
    }
}


// 混合音乐
- (void)merge{
    NSLog(@"%@", NSHomeDirectory());
    
    NSString * itemName = @"1562663793812";
    // mbp提示框
    // 路径
    NSString *documents = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    // 声音来源
    NSURL *audioInputUrl = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:itemName ofType:@"wav"]];
    // 视频来源
    NSURL *videoInputUrl = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:itemName ofType:@"mp4"]];
    
    AVFileType fileType = AVFileTypeMPEG4;
    
    NSString * outputFileName = [NSString stringWithFormat:@"itemName_%@_merge.mp4", fileType];
    
    // 最终合成输出路径
    NSString *outPutFilePath = [documents stringByAppendingPathComponent:outputFileName];
    // 添加合成路径
    NSURL *outputFileUrl = [NSURL fileURLWithPath:outPutFilePath];
    // 时间起点
    CMTime nextClistartTime = kCMTimeZero;
    // 创建可变的音视频组合
    AVMutableComposition *comosition = [AVMutableComposition composition];
    
    // 视频采集
    AVURLAsset *videoAsset = [[AVURLAsset alloc] initWithURL:videoInputUrl options:nil];
    // 视频时间范围
    CMTimeRange videoTimeRange = CMTimeRangeMake(kCMTimeZero, videoAsset.duration);
    // 视频通道 枚举 kCMPersistentTrackID_Invalid = 0
    AVMutableCompositionTrack *videoTrack = [comosition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
    // 视频采集通道
    AVAssetTrack *videoAssetTrack = [[videoAsset tracksWithMediaType:AVMediaTypeVideo] firstObject];
    //  把采集轨道数据加入到可变轨道之中
    [videoTrack insertTimeRange:videoTimeRange ofTrack:videoAssetTrack atTime:nextClistartTime error:nil];
    
    comosition.naturalSize = videoAssetTrack.naturalSize;
    
    // 声音采集
    AVURLAsset *audioAsset = [[AVURLAsset alloc] initWithURL:audioInputUrl options:nil];
    // 因为视频短这里就直接用视频长度了,如果自动化需要自己写判断
    CMTimeRange audioTimeRange = videoTimeRange;
    // 音频通道
    AVMutableCompositionTrack *audioTrack = [comosition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
    // 音频采集通道
    AVAssetTrack *audioAssetTrack = [[audioAsset tracksWithMediaType:AVMediaTypeAudio] firstObject];
    // 加入合成轨道之中
    [audioTrack insertTimeRange:audioTimeRange ofTrack:audioAssetTrack atTime:nextClistartTime error:nil];
    
    
    // 创建一个输出
    AVAssetExportSession *assetExport = [[AVAssetExportSession alloc] initWithAsset:comosition presetName:AVAssetExportPresetHighestQuality];
    // 输出类型
    assetExport.outputFileType = fileType;
    // 输出地址
    assetExport.outputURL = outputFileUrl;
    // 优化
    assetExport.shouldOptimizeForNetworkUse = YES;
    
    // 合成完毕
    [assetExport exportAsynchronouslyWithCompletionHandler:^{
        // 回到主线程
        dispatch_async(dispatch_get_main_queue(), ^{
            // 调用播放方法
            //            [self playWithUrl:outputFileUrl];
            
            
            
            
        });
    }];
    
    //    AVAssetReader * assetReader = [[AVAssetReader alloc] initWithAsset:comosition error:nil];
    
    
}

///** 播放方法 */
//- (void)playWithUrl:(NSURL *)url{
//    // 传入地址
//    AVPlayerItem *playerItem = [AVPlayerItem playerItemWithURL:url];
//    // 播放器
//    AVPlayer *player = [AVPlayer playerWithPlayerItem:playerItem];
//    // 播放器layer
//    AVPlayerLayer *playerLayer = [AVPlayerLayer playerLayerWithPlayer:player];
//    playerLayer.frame = self.imageView.frame;
//    // 视频填充模式
//    playerLayer.videoGravity = AVLayerVideoGravityResizeAspect;
//    // 添加到imageview的layer上
//    [self.imageView.layer addSublayer:playerLayer];
//    // 隐藏提示框 开始播放
////    [MBProgressHUD hideHUD];
////    [MBProgressHUD showSuccess:@"合成完成"];
//    // 播放
//    [player play];
//}

//利用AVMutableVideoComposition在特定时间添加水印
- (void)applyVideoEffectsToComposition:(AVMutableVideoComposition *)composition size:(CGSize)size{
    // 1 - set up the overlay
    CALayer *overlayLayer = [CALayer layer];
    UIImage *overlayImage  = [UIImage imageNamed:@"waterMark"];
    
    [overlayLayer setContents:(id)[overlayImage CGImage]];
    overlayLayer.frame = CGRectMake(size.width - 146, 24, 135, 18);
    [overlayLayer setMasksToBounds:YES];
    
    // 2 - set up the parent layer
    CALayer *parentLayer = [CALayer layer];
    CALayer *videoLayer = [CALayer layer];
    parentLayer.frame = CGRectMake(0, 0, size.width, size.height);
    videoLayer.frame = CGRectMake(0, 0, size.width, size.height);
    [parentLayer addSublayer:videoLayer];
    [parentLayer addSublayer:overlayLayer];
    
    //*********** For A Special Time
    CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"opacity"];
    [animation setDuration:0];
    [animation setFromValue:[NSNumber numberWithFloat:1.0]];
    [animation setToValue:[NSNumber numberWithFloat:0.0]];
    [animation setBeginTime:5];
    [animation setRemovedOnCompletion:NO];
    [animation setFillMode:kCAFillModeForwards];
    [overlayLayer addAnimation:animation forKey:@"animateOpacity"];
    
    // 3 - apply magic
    composition.animationTool = [AVVideoCompositionCoreAnimationTool
                                 videoCompositionCoreAnimationToolWithPostProcessingAsVideoLayer:videoLayer inLayer:parentLayer];
}


//+ (void)applyVideoEffectsToComposition2:(AVVideoComposition *)composition size:(CGSize)size
//{
//    // 文字
//    //    CATextLayer *subtitle1Text = [[CATextLayer alloc] init];
//    //    //    [subtitle1Text setFont:@"Helvetica-Bold"];
//    //    [subtitle1Text setFontSize:36];
//    //    [subtitle1Text setFrame:CGRectMake(10, size.height-10-100, size.width, 100)];
//    //    [subtitle1Text setString:@"ZHIMABAOBAO"];
//    //    //    [subtitle1Text setAlignmentMode:kCAAlignmentCenter];
//    //    [subtitle1Text setForegroundColor:[[UIColor whiteColor] CGColor]];
//
//    //图片
//    CALayer*picLayer = [CALayer layer];
//    picLayer.contents = (id)[UIImage imageNamed:@"ipad app-76.png"].CGImage;
//    picLayer.frame = CGRectMake(size.width-15-76, 15, 76, 76);
//
//    // 2 - The usual overlay
//    CALayer *overlayLayer = [CALayer layer];
//    [overlayLayer addSublayer:picLayer];
//    overlayLayer.frame = CGRectMake(0, 0, size.width, size.height);
//    [overlayLayer setMasksToBounds:YES];
//
//    CALayer *parentLayer = [CALayer layer];
//    CALayer *videoLayer = [CALayer layer];
//    parentLayer.frame = CGRectMake(0, 0, size.width, size.height);
//    videoLayer.frame = CGRectMake(0, 0, size.width, size.height);
//    [parentLayer addSublayer:videoLayer];
//    [parentLayer addSublayer:overlayLayer];
//
////    AVVideoCompositionCoreAnimationTool
//
//    composition.animationTool = [AVVideoCompositionCoreAnimationTool
//                                 videoCompositionCoreAnimationToolWithPostProcessingAsVideoLayer:videoLayer inLayer:parentLayer];
//
//}


/**
 设置水印及其对应视频的位置
 
 @param composition 视频的结构
 @param size 视频的尺寸
 */
+ (void)applyVideoEffectsToComposition:(AVMutableVideoComposition *)composition size:(CGSize)size
{
    // 文字
    //    CATextLayer *subtitle1Text = [[CATextLayer alloc] init];
    //    //    [subtitle1Text setFont:@"Helvetica-Bold"];
    //    [subtitle1Text setFontSize:36];
    //    [subtitle1Text setFrame:CGRectMake(10, size.height-10-100, size.width, 100)];
    //    [subtitle1Text setString:@"ZHIMABAOBAO"];
    //    //    [subtitle1Text setAlignmentMode:kCAAlignmentCenter];
    //    [subtitle1Text setForegroundColor:[[UIColor whiteColor] CGColor]];
    
    //图片
    CALayer*picLayer = [CALayer layer];
    picLayer.contents = (id)[UIImage imageNamed:@"ipad app-76.png"].CGImage;
    picLayer.frame = CGRectMake(size.width-15-76, 15, 76, 76);
    
    // 2 - The usual overlay
    CALayer *overlayLayer = [CALayer layer];
    [overlayLayer addSublayer:picLayer];
    overlayLayer.frame = CGRectMake(0, 0, size.width, size.height);
    [overlayLayer setMasksToBounds:YES];
    
    CALayer *parentLayer = [CALayer layer];
    CALayer *videoLayer = [CALayer layer];
    parentLayer.frame = CGRectMake(0, 0, size.width, size.height);
    videoLayer.frame = CGRectMake(0, 0, size.width, size.height);
    [parentLayer addSublayer:videoLayer];
    [parentLayer addSublayer:overlayLayer];
    
    composition.animationTool = [AVVideoCompositionCoreAnimationTool
                                 videoCompositionCoreAnimationToolWithPostProcessingAsVideoLayer:videoLayer inLayer:parentLayer];
    
}


- (NSString *)description {
    return [NSString stringWithFormat:@"<SCAVMixHandler: %p, name: %@, status:%ld>", self, self.name, self.status];
}

@end




@interface SCAVMixConfig ()
@property (nonatomic, copy) NSString * identifier;
@end
@implementation SCAVMixConfig

- (instancetype)initWithIdentifier:(NSString *)identifier {
    self = [super init];
    if (self) {
        self.identifier = identifier;
    }
    return self;
}

@end
