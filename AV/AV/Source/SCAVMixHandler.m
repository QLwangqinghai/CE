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

@property (nonatomic, copy) NSArray<SCAVWaterMask *> * waterMasks;


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

+ (CGColorSpaceRef)colorSpace {
    static CGColorSpaceRef __colorSpace;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __colorSpace = CGColorSpaceCreateDeviceRGB();
    });
    return __colorSpace;
}

- (void)drawWaterMask:(SCAVWaterMask *)waterMask context:(CGContextRef)context contextSize:(CGSize)contextSize {
    // 为媒体数据设置一个CMSampleBuffer的Core Video图像缓存对象
    
    if (CGRectIsNull(waterMask.frame)) {
        return;
    }
    CGImageRef image = waterMask.image.CGImage;
    if (nil == image) {
        return;
    }
    CGRect frame = waterMask.frame;
    CGRect rect = frame;
    rect.origin.y = contextSize.height - frame.origin.y - frame.size.height;
    CGContextDrawImage(context, rect, image);
}

- (void)addWaterMasks:(CMSampleBufferRef) sampleBuffer {
    if (self.waterMasks.count <= 0) {
        return;
    }
    
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
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
        return;
    }
    
    UIGraphicsPushContext(bitmapContext);
    
    CGSize contextSize = CGSizeMake(width, height);
    [self.waterMasks enumerateObjectsUsingBlock:^(SCAVWaterMask * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        [self drawWaterMask:obj context:bitmapContext contextSize:contextSize];
    }];

    UIGraphicsPopContext();
    CGContextRelease(bitmapContext);
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
}


- (CMSampleBufferRef _Nullable)nextSampleBuffer {
    CMSampleBufferRef buffer = [self.readerTrackOutput copyNextSampleBuffer];
    BOOL isVideo = (SCAVWriterInputHandlerKindVideo == self.kind);
    if (isVideo) {
        [self addWaterMasks:buffer];
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



@interface SCAVMixHandler ()

@property (nonatomic, assign) SCAVMixHandlerStatus status;
@property (nonatomic, copy) SCAVMixHandlerBlock completion;

@property (nonatomic, strong) AVAssetReader * assetReader;

@property (nonatomic, strong) AVAssetReaderTrackOutput * videoOutPut;
@property (nonatomic, strong) AVAssetReaderTrackOutput * audioOutPut;


@property (nonatomic, strong) AVAssetWriter * assetWriter;

@property (nonatomic, strong) SCAVWriterInputHandler * videoWriterInputHandler;
@property (nonatomic, strong) SCAVWriterInputHandler * audioWriterInputHandler;


//文件临时输出地方， 最后成功后移动文件位置
@property (nonatomic, copy) NSString * tmpOutputPath;
@property (nonatomic, copy) NSString * outputCachePath;

@property (nonatomic, copy) NSURL * tmpOutputUrl;
@property (nonatomic, strong) dispatch_queue_t queue;

@end
@implementation SCAVMixHandler

- (instancetype)initWithConfig:(SCAVMixConfig *)config queue:(dispatch_queue_t)queue {
    self = [super init];
    if (self) {
        self.config = config;
        self.queue = queue;
        
        NSUUID * uuid = [NSUUID UUID];
        NSString * tmpDirectoryPath = [NSTemporaryDirectory() stringByAppendingPathComponent:uuid.UUIDString];
        [[NSFileManager defaultManager] createDirectoryAtPath:tmpDirectoryPath withIntermediateDirectories:true attributes:nil error:NULL];
        _tmpOutputPath = [tmpDirectoryPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.mp4", config.identifier]];
        self.tmpOutputUrl = [NSURL fileURLWithPath:_tmpOutputPath];
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
    SCAVMixConfig * config = self.config;
    
    // 时间起点
    CMTime beginTime = CMTimeMakeWithSeconds(0, 15);
    
    CMTime nextClistartTime = kCMTimeZero;
    // 创建可变的音视频组合
    
    AVMutableComposition *comosition = [AVMutableComposition composition];
    
    // 视频采集
    AVAsset * videoAsset = config.videoAsset;
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
    
    // 声音采集
    AVAsset * audioAsset = config.audioAsset;
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
    CGSize videoSize = self.config.outputVideoSize;
    
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
                                            AVVideoExpectedSourceFrameRateKey : @(self.config.outputVideoFrameRate),
                                            AVVideoAverageBitRateKey : @(bitRat),
                                            AVVideoProfileLevelKey : AVVideoProfileLevelH264Main31,
                                            }
                                    };
    
    AVAssetWriterInput * videoWriterInput = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeVideo outputSettings:videoSetting];
    
    self.videoWriterInputHandler = [[SCAVWriterInputHandler alloc] initWithWriterInput:videoWriterInput readerTrackOutput:self.videoOutPut queue:self.queue];
    self.videoWriterInputHandler.waterMasks = self.config.waterMasks;
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
    NSLog(@"===>assertReadToAssetInput.begin: %@", self.config.identifier);
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
                
                NSLog(@"===>assertReadToAssetInput.finsished: %@, filePath: %@, fileSize: %.6lfmb, time:%lf", self.config.identifier, filePath, fileSize, endTime - beginTime);
            } else {
                NSLog(@"===>assertReadToAssetInput.failure: %@, error:%@", self.config.identifier, self.assetWriter.error);
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

- (NSString *)description {
    return [NSString stringWithFormat:@"<SCAVMixHandler: %p, name: %@, status:%ld>", self, self.config.identifier, self.status];
}

@end



@interface SCAVMixConfig ()

@property (nonatomic, strong, readonly) NSMutableArray<SCAVWaterMask *> * waterMasksStorage;

@end
@implementation SCAVMixConfig

- (instancetype)initWithIdentifier:(NSString *)identifier {
    self = [super init];
    if (self) {
        _waterMasksStorage = [NSMutableArray array];
        _identifier = [identifier copy];
        if (nil == identifier) {
            NSLog(@"identifier is nil");
        }
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier videoAsset:(AVAsset *)videoAsset audioAsset:(nullable AVAsset *)audioAsset {
    self = [self initWithIdentifier:identifier];
    if (self) {
        if (nil == videoAsset) {
            NSLog(@"video is nil");
        }
        _videoAsset = videoAsset;
        _audioAsset = audioAsset;
        
        if (videoAsset) {
            AVAssetTrack *videoAssetTrack = [[videoAsset tracksWithMediaType:AVMediaTypeVideo] firstObject];
            self.outputVideoSize = videoAssetTrack.naturalSize;
        }
        _outputVideoFrameRate = 15;
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier videoInputPath:(NSString *)videoInputPath audioInputPath:(nullable NSString *)audioInputPath {
    AVURLAsset * videoAsset = nil;
    if (videoInputPath) {
        videoAsset = [[AVURLAsset alloc] initWithURL:[NSURL fileURLWithPath:videoInputPath] options:nil];
        if (!videoAsset.playable) {
            NSLog(@"video is not playable");
            videoAsset = nil;
        }
    }

    AVURLAsset * audioAsset = nil;
    if (audioInputPath) {
        audioAsset = [[AVURLAsset alloc] initWithURL:[NSURL fileURLWithPath:audioInputPath] options:nil];
        if (!audioAsset.playable) {
            NSLog(@"audio is not playable");
            audioAsset = nil;
        }
    }

    return [self initWithIdentifier:identifier videoAsset:videoAsset audioAsset:audioAsset];
}

- (void)addWaterMask:(SCAVWaterMask *)waterMask {
    if (waterMask) {
        [self.waterMasksStorage addObject:waterMask];
    }
}
- (NSArray<SCAVWaterMask *> *)waterMasks {
    return [self.waterMasksStorage copy];
}

@end


@implementation SCAVWaterMask
@end
