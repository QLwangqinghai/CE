//
//  SCAVMixHandler.m
//  AV
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "SCAVMixHandler.h"


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

@property (nonatomic, strong) dispatch_queue_t queue;
@property (nonatomic, strong) AVAssetWriterInput * writerInput;
@property (nonatomic, strong) AVAssetReaderTrackOutput * readerTrackOutput;


@property (nonatomic, copy) SCAVWriterInputHandlerBlock completion;
@property (nonatomic, copy) NSString * tag;

@property (atomic, assign) NSInteger breakFlag;// > 0 标识取消， < 0 标识取消完成

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

- (void)prepareCancel {
    self.breakFlag += 1;
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

- (BOOL)finish {
    if (SCAVWriterInputHandlerStatusRunning == self.status) {
        [self.writerInput markAsFinished];
        self.status = SCAVWriterInputHandlerStatusFinished;
        
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
    [self.writerInput requestMediaDataWhenReadyOnQueue:queue usingBlock:^{
        NSInteger breakFlag = self.breakFlag;
        while (self.writerInput.readyForMoreMediaData) {
            //样本数据
            CMSampleBufferRef buffer = [self.readerTrackOutput copyNextSampleBuffer];
            if (buffer) {
                [self.writerInput appendSampleBuffer:buffer];
                //                CMTime videoTime = CMSampleBufferGetDuration(buffer);
                
                CFRelease(buffer);
            } else {
                NSLog(@"%@: input finish", tag);
                [self finish];
                break;
            }
            if (self.breakFlag != breakFlag) {
                break;
            }
        }
    }];
}

@end



@class SCAVMixHandler;
typedef void(^SCAVMixHandlerBlock)(SCAVMixHandler * handler);

@interface SCAVMixHandler ()

//@property (nonatomic, copy) NSString * name;

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
@property (nonatomic, copy) NSURL * tmpOutputUrl;
@property (nonatomic, strong) dispatch_queue_t queue;

@property (nonatomic, assign) CGSize videoSize;

@end
@implementation SCAVMixHandler

- (instancetype)initWithConfig:(SCAVMixConfig *)config
                         queue:(dispatch_queue_t)queue
                    completion:(SCAVMixHandlerBlock)completion {
    self = [super init];
    if (self) {
        self.config = config;
        self.queue = queue;
        
        self.videoInputUrl = [NSURL fileURLWithPath:config.videoInputPath];
        self.audioInputUrl = [NSURL fileURLWithPath:config.audioInputPath];
        
        NSString * temporaryDirectory = NSTemporaryDirectory();
        NSString * directoryName = [NSUUID UUID].UUIDString;
        self.tmpOutputUrl = [NSURL fileURLWithPath:[temporaryDirectory stringByAppendingPathComponent:directoryName]];
        
//        self.writerInput = writerInput;
//        self.readerTrackOutput = readerTrackOutput;
//        self.queue = queue;
//        self.completion = completion;
    }
    return self;
}



- (BOOL)prepare {
    BOOL result = [self prepareDirectory];

    if (result) {
        result = [self configAssetReader];
    }
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
        
        
    }
    return result;
}
- (BOOL)run {
    BOOL result = true;
    [self assertReadToAssetInput];
    return result;
}

- (BOOL)cancel {
    BOOL result = true;

    return result;
}

//准备文件目录
- (BOOL)prepareDirectory {
    BOOL result = true;
//[NSFileManager defaultManager] createDirectoryAtURL:<#(nonnull NSURL *)#> withIntermediateDirectories:<#(BOOL)#> attributes:<#(nullable NSDictionary<NSFileAttributeKey,id> *)#> error:<#(NSError * _Nullable __autoreleasing * _Nullable)#>
    
    return result;
}

- (BOOL)configAssetReader {
    NSURL * videoUrl = self.videoInputUrl;
    NSURL * audioUrl = self.audioInputUrl;

    // 时间起点
    CMTime beginTime = CMTimeMakeWithSeconds(0, 15);
    
    CMTime nextClistartTime = kCMTimeZero;
    // 创建可变的音视频组合
    AVMutableComposition *comosition = [AVMutableComposition composition];
    
    // 视频采集
    AVURLAsset * videoAsset = [[AVURLAsset alloc] initWithURL:videoUrl options:nil];
    // 视频时间范围
    
    CMTimeRange videoTimeRange = CMTimeRangeMake(beginTime, videoAsset.duration);
    // 视频通道 枚举 kCMPersistentTrackID_Invalid = 0
    AVMutableCompositionTrack *videoTrack = [comosition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
    // 视频采集通道
    AVAssetTrack *videoAssetTrack = [[videoAsset tracksWithMediaType:AVMediaTypeVideo] firstObject];
    //  把采集轨道数据加入到可变轨道之中
    [videoTrack insertTimeRange:videoTimeRange ofTrack:videoAssetTrack atTime:nextClistartTime error:nil];
    
    comosition.naturalSize = videoAssetTrack.naturalSize;
    self.videoSize = videoAssetTrack.naturalSize;
    // 声音采集
    AVURLAsset *audioAsset = [[AVURLAsset alloc] initWithURL:audioUrl options:nil];
    // 因为视频短这里就直接用视频长度了,如果自动化需要自己写判断
    CMTimeRange audioTimeRange = videoTimeRange;
    // 音频通道
    AVMutableCompositionTrack *audioTrack = [comosition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
    // 音频采集通道
    AVAssetTrack *audioAssetTrack = [[audioAsset tracksWithMediaType:AVMediaTypeAudio] firstObject];
    // 加入合成轨道之中
    [audioTrack insertTimeRange:audioTimeRange ofTrack:audioAssetTrack atTime:nextClistartTime error:nil];
    
    
    
    //获取资源的一个视频轨道
    AVAssetTrack * track = [[comosition tracksWithMediaType:AVMediaTypeVideo] firstObject];
    
    
    NSError * error;
    _assetReader = [[AVAssetReader alloc] initWithAsset:comosition error:&error];
    
    //指定将读取的样本数据压缩为BGRA格式
    
    NSDictionary *setting =   @{(id)kCVPixelBufferPixelFormatTypeKey:@(kCVPixelFormatType_32BGRA)};
    
    //初始化输出，指定从track轨道中读取样本数据
    
    AVAssetReaderTrackOutput * videoOutPut = [[AVAssetReaderTrackOutput alloc] initWithTrack:track outputSettings:setting];
    
    CMTime duration = audioAsset.duration;
    
    //    audioAsset.duration.timescale
    NSDictionary * audioSetting = @{ AVFormatIDKey : @(kAudioFormatLinearPCM),
                                     AVNumberOfChannelsKey : @(1),
                                     //                                     AVSampleRateKey : @(audioDurationSeconds)
                                     
                                     };
    
    AVAssetReaderTrackOutput * audioOutPut = [[AVAssetReaderTrackOutput alloc] initWithTrack:[[comosition tracksWithMediaType:AVMediaTypeAudio] firstObject] outputSettings:audioSetting];
    self.videoOutPut = videoOutPut;
    self.audioOutPut = audioOutPut;
    
    
    //添加输出
    if ([_assetReader canAddOutput:videoOutPut]) {
        [_assetReader addOutput:videoOutPut];
    } else {
        NSLog(@"addOutput videoOutPut error");
        return false;
    }
    if ([_assetReader canAddOutput:audioOutPut]) {
        [_assetReader addOutput:audioOutPut];
    } else {
        NSLog(@"addOutput audioOutPut error");
        return false;
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
    self.videoWriterInputHandler.tag = @"video";
    //添加写入器
    if ([_assetWriter canAddInput:videoWriterInput]) {
        [_assetWriter addInput:videoWriterInput];
    } else {
        NSLog(@"canAddInput:videoWriterInputHandler");
        return false;
    }
    
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
    self.videoWriterInputHandler.kind = SCAVWriterInputHandlerKindAudio;
    self.audioWriterInputHandler.tag = @"audio";
    
    //添加写入器
    if ([_assetWriter canAddInput:audioWriterInput]) {
        [_assetWriter addInput:audioWriterInput];
    } else {
        NSLog(@"canAddInput:audioWriterInputHandler");
        return false;
    }
    _assetWriter.shouldOptimizeForNetworkUse = true;
    
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
        }];
    });
    [self.videoWriterInputHandler startWithCompletion:^(SCAVWriterInputHandler *handler) {
        dispatch_group_leave(group);
    }];
    [self.audioWriterInputHandler startWithCompletion:^(SCAVWriterInputHandler *handler) {
        dispatch_group_leave(group);
    }];
}






@end
