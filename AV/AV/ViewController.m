//
//  ViewController.m
//  AV
//
//  Created by vector on 2019/7/9.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"
#import <AVFoundation/AVFoundation.h>

@interface SCAVConfig : NSObject

@property (nonatomic, copy) NSURL * outputUrl;
@property (nonatomic, copy) NSURL * audioInputUrl;
@property (nonatomic, copy) NSURL * videoInputUrl;

@property (nonatomic, assign) CGSize outputSize;



@end
@implementation SCAVConfig

@end


typedef NS_ENUM(NSUInteger, SCAVWriterInputHandlerStatus) {
    SCAVWriterInputHandlerStatusNone,
    SCAVWriterInputHandlerStatusRunning,
    SCAVWriterInputHandlerStatusFinished,
    SCAVWriterInputHandlerStatusCanceled,
};

@class SCAVWriterInputHandler;
typedef void(^SCAVWriterInputHandlerBlock)(SCAVWriterInputHandler * handler);
@interface SCAVWriterInputHandler : NSObject

@property (atomic, assign) SCAVWriterInputHandlerStatus status;

@property (nonatomic, strong) dispatch_queue_t queue;
@property (nonatomic, strong) AVAssetWriterInput * writerInput;
@property (nonatomic, strong) AVAssetReaderTrackOutput * readerTrackOutput;


@property (nonatomic, copy) SCAVWriterInputHandlerBlock onFinished;
@property (nonatomic, copy) NSString * tag;


@end
@implementation SCAVWriterInputHandler

- (instancetype)initWithWriterInput:(AVAssetWriterInput *)writerInput readerTrackOutput:(AVAssetReaderTrackOutput *)readerTrackOutput {
    self = [super init];
    if (self) {
        if (writerInput) {
            self.writerInput = writerInput;
            self.readerTrackOutput = readerTrackOutput;
//            [writerInput addObserver:self forKeyPath:@"readyForMoreMediaData" options:(NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld) context:SCAVWriterInputHandlerObserverContext];
        }
    }
    return self;
}

- (BOOL)startWithQueue:(dispatch_queue_t)queue onFinished:(SCAVWriterInputHandlerBlock)onFinished {
    if (SCAVWriterInputHandlerStatusNone == self.status) {
        if (queue) {
            self.queue = queue;
            self.onFinished = onFinished;
            self.status = SCAVWriterInputHandlerStatusRunning;
            
            [self assertReadToAssetInput];
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

- (BOOL)finish {
    if (SCAVWriterInputHandlerStatusRunning == self.status) {
        [self.writerInput markAsFinished];
        self.status = SCAVWriterInputHandlerStatusFinished;

        if (self.onFinished) {
            self.onFinished(self);
        }
        self.onFinished = nil;
        return true;
    } else {
        return false;
    }
}


-(void)assertReadToAssetInput {
    if (SCAVWriterInputHandlerStatusRunning != self.status) {
        return;
    }
    dispatch_queue_t queue = self.queue;
    NSString * tag = self.tag ?: @"";
    __block NSInteger count = 0;
    [self.writerInput requestMediaDataWhenReadyOnQueue:queue usingBlock:^{
        while (self.writerInput.readyForMoreMediaData) {
            //样本数据
            
            CMSampleBufferRef buffer = [self.readerTrackOutput copyNextSampleBuffer];
            if (buffer) {
                [self.writerInput appendSampleBuffer:buffer];
                CMTime videoTime = CMSampleBufferGetDuration(buffer);

                CFRelease(buffer);
                count ++;
                NSLog(@"%@: append success:%ld", tag, count);
            } else {
                NSLog(@"%@: input finish", tag);
                [self finish];
                break;
            }
        }
    }];
}


@end


@interface SCAVMix : NSObject

@property (nonatomic, strong) SCAVConfig * config;


@property (nonatomic, strong) AVAsset * videoAsset;
@property (nonatomic, strong) AVAsset * audioAsset;

@property (nonatomic, strong) AVAssetReaderTrackOutput * videoReaderTrackOutput;
@property (nonatomic, strong) AVAssetReader * assetReader;
@property (nonatomic, strong) AVAssetWriter * assetWriter;


@property (nonatomic, strong) AVAssetReaderTrackOutput * videoOutPut;
@property (nonatomic, strong) AVAssetReaderTrackOutput * audioOutPut;


@property (nonatomic, strong) SCAVWriterInputHandler * videoWriterInputHandler;
@property (nonatomic, strong) SCAVWriterInputHandler * audioWriterInputHandler;


@end
@implementation SCAVMix

-(void)configAssetReader {
    SCAVConfig * config = self.config;
    NSURL * videoUrl = config.videoInputUrl;
    NSURL * audioUrl = config.audioInputUrl;
    
    // 时间起点
    CMTime nextClistartTime = kCMTimeZero;
    // 创建可变的音视频组合
    AVMutableComposition *comosition = [AVMutableComposition composition];
    
    // 视频采集
    AVURLAsset * videoAsset = [[AVURLAsset alloc] initWithURL:videoUrl options:nil];
    // 视频时间范围
    CMTimeRange videoTimeRange = CMTimeRangeMake(kCMTimeZero, videoAsset.duration);
    // 视频通道 枚举 kCMPersistentTrackID_Invalid = 0
    AVMutableCompositionTrack *videoTrack = [comosition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
    // 视频采集通道
    AVAssetTrack *videoAssetTrack = [[videoAsset tracksWithMediaType:AVMediaTypeVideo] firstObject];
    //  把采集轨道数据加入到可变轨道之中
    [videoTrack insertTimeRange:videoTimeRange ofTrack:videoAssetTrack atTime:nextClistartTime error:nil];
    
    comosition.naturalSize = videoAssetTrack.naturalSize;
    config.outputSize = videoAssetTrack.naturalSize;
    
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
        NSLog(@"videoOutPut");
    }
    if ([_assetReader canAddOutput:audioOutPut]) {
        [_assetReader addOutput:audioOutPut];
    } else {
        NSLog(@"audioOutPut");
    }

    //开始读取过程
    
    [_assetReader startReading];
    
}
-(void)configWriteInput {
    SCAVConfig * config = self.config;
    NSURL * outputUrl = config.outputUrl;
    NSError * error;
    _assetWriter = [[AVAssetWriter alloc] initWithURL:outputUrl fileType:AVFileTypeMPEG4 error:&error];
    
    NSInteger bitRat = config.outputSize.width * config.outputSize.height * 0.75;
    NSDictionary * videoSetting = @{
//                              AVVideoCodecKey : AVVideoCodecH264,
                              AVVideoCodecKey : AVVideoCodecTypeH264,
                              AVVideoWidthKey:@(config.outputSize.width),
                              AVVideoHeightKey:@(config.outputSize.height),
                              AVVideoCompressionPropertiesKey:@{
                                      
                                      AVVideoMaxKeyFrameIntervalKey : @(75),
                                      AVVideoExpectedSourceFrameRateKey : @(15),
                                      AVVideoAverageBitRateKey : @(bitRat),
//                                      AVVideoAverageBitRateKey : @(50000),

//                                      AVVideoProfileLevelKey:AVVideoProfileLevelH264Main31,
                                      AVVideoProfileLevelKey : AVVideoProfileLevelH264Main31,
                                      
                                      
                                      }
                              
                              };
    
    AVAssetWriterInput * videoWriterInput = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeVideo outputSettings:videoSetting];
    self.videoWriterInputHandler = [[SCAVWriterInputHandler alloc] initWithWriterInput:videoWriterInput readerTrackOutput:self.videoOutPut];
    self.videoWriterInputHandler.tag = @"video";
    
        //添加写入器
    if ([_assetWriter canAddInput:videoWriterInput]) {
        [_assetWriter addInput:videoWriterInput];
    } else {
        NSLog(@"canAddInput:videoWriterInputHandler");
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
    self.audioWriterInputHandler = [[SCAVWriterInputHandler alloc] initWithWriterInput:audioWriterInput readerTrackOutput:self.audioOutPut];
    self.audioWriterInputHandler.tag = @"audio";

    //添加写入器
    if ([_assetWriter canAddInput:audioWriterInput]) {
        [_assetWriter addInput:audioWriterInput];
    } else {
        NSLog(@"canAddInput:audioWriterInputHandler");
    }

    
    [_assetWriter startWriting];

}

-(void)assertReadToAssetInput {
    dispatch_queue_t queue = dispatch_queue_create("com.writequeue", NULL);
    
    //开启写入会话，并指定样本的开始时间
    
    [_assetWriter startSessionAtSourceTime:kCMTimeZero];
    dispatch_group_t group = dispatch_group_create();
    dispatch_group_enter(group);
    dispatch_group_enter(group);
    dispatch_group_notify(group, queue, ^{
        [self.assetWriter finishWritingWithCompletionHandler:^{
            AVAssetWriterStatus status = self.assetWriter.status;
            if (status == AVAssetWriterStatusCompleted) {
                NSLog(@"finsished %@", self.config.outputUrl);
                NSString * filePath = self.config.outputUrl.path;
                NSFileManager * fm = [NSFileManager defaultManager];
                
                if ([fm fileExistsAtPath:filePath]) {
                    NSDictionary * attributesOfItem = [fm attributesOfItemAtPath:filePath error:nil];
                    CGFloat fileSize = (CGFloat)[attributesOfItem[NSFileSize] integerValue];
                    NSLog(@"fileSize %.03lf", fileSize/1000);
                }
            } else {
                NSLog(@"failure error:%@", self.assetWriter.error);
            }
        }];
    });
    [self.videoWriterInputHandler startWithQueue:queue onFinished:^(SCAVWriterInputHandler *handler) {
        dispatch_group_leave(group);
    }];
    [self.audioWriterInputHandler startWithQueue:queue onFinished:^(SCAVWriterInputHandler *handler) {
        dispatch_group_leave(group);
    }];
}



@end



@interface ViewController ()
@property (nonatomic, assign) NSInteger status;
@property (nonatomic, strong) SCAVConfig * config;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.status = 0;
    
    self.config = [[SCAVConfig alloc] init];
    NSString * itemName = @"1562663793812";

    // mbp提示框
    // 路径
    NSString *documents = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    // 声音来源
    self.config.audioInputUrl = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:itemName ofType:@"wav"]];
    // 视频来源
    self.config.videoInputUrl = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:itemName ofType:@"mp4"]];
    
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"HHmmss";
    NSString * dateString = [formatter stringFromDate:[NSDate date]];
    NSString * outputFileName = [NSString stringWithFormat:@"%@_%@_merge.mp4", itemName, dateString];
    // 最终合成输出路径
    NSString *outPutFilePath = [documents stringByAppendingPathComponent:outputFileName];
    // 添加合成路径
    self.config.outputUrl = [NSURL fileURLWithPath:outPutFilePath];

}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent:event];
    if (0 == self.status) {
        self.status = 1;
//        [self merge];
        
        
        SCAVMix * mix = [[SCAVMix alloc] init];
        mix.config = self.config;
        
        [mix configAssetReader];
        [mix configWriteInput];
        [mix assertReadToAssetInput];
        
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(30 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            SCAVMix * mix1 = mix;
            NSLog(@"after : %@", mix1);
        });
    }
}

- (IBAction)mergeAction:(UIButton *)sender {
    [self merge];
}




- (void)mergeByReaderWriter {
    
    
    
    
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


@end
