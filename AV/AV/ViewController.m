//
//  ViewController.m
//  AV
//
//  Created by vector on 2019/7/9.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "SCAVMixHandler.h"
#import "SCAVMixManager.h"
#import "SCRecoverManager.h"
#import <OpenGLES/EAGL.h>

#import <CommonCrypto/CommonDigest.h>
#import <CommonCrypto/CommonHMAC.h>

@interface ViewController ()
@property (nonatomic, assign) NSInteger status;
@property (nonatomic, copy) NSArray<SCAVMixHandler *> * mixs;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    [SCRecoverManager shared];
    
    [[NSNotificationCenter defaultCenter] addObserverForName:SCAVMixManagerItemUpdateNotification object:nil queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification * _Nonnull note) {
        NSLog(@"\nupdate\n");
    }];
    
    
    UIImageView * imageView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 300, 200)];
    [self.view addSubview:imageView];
    
    [[NSNotificationCenter defaultCenter] addObserverForName:@"onImage" object:nil queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification * _Nonnull note) {
        imageView.image = note.object;
    }];
    
    self.status = 0;
    
    NSArray<NSString *> * items = @[@"1562930748136", @"1562928616807", @"1562663973911", @"1562662353087", @"1562662173010", @"1562661992940", @"1562663793812", @"1562641927402"];
//    NSArray<NSString *> * items = @[@"1562930748136"];

//    NSArray<NSString *> * items = @[@"1562663973911", @"1562662353087", @"1562662173010", @"1562661992940"];

    
    NSMutableArray<SCAVMixWorkItem *> * mixs = [NSMutableArray array];

    [items enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        SCAVMixWorkItem * item = [[SCAVMixWorkItem alloc] initWithConfig:[self makeConfig:obj] onResult:^(id<SCAVMixWorkItemProtocol>  _Nonnull citem, SCAVMixWorkItemResult result) {
            NSLog(@"onresult: %@, result: %ld", citem, result);
            if (SCAVMixWorkItemResultSuccess == result) {
                SCAVMixWorkItem * obj = (SCAVMixWorkItem *)citem;
//                [[SCRecoverManager shared] recoverPlaybackGeneratedVideoAtPath:obj.config.outputPath];
                
//                [[SCRecoverManager shared] recoverPlaybackOriginalFileAtPath:obj.config.audioInputPath rename:[NSString stringWithFormat:@"%@.wav", obj.config.identifier]];
//                [[SCRecoverManager shared] recoverPlaybackOriginalFileAtPath:obj.config.videoInputPath rename:[NSString stringWithFormat:@"%@.mp4", obj.config.identifier]];
            }
            
            
        }];
//        [item startWithQueue:[SCAVMixManager queue] completion:^(id<SCAVMixWorkItemProtocol>  _Nonnull item) {
//            NSLog(@"%@", item);
//        }];
        [mixs addObject:item];
    }];

    
    [mixs enumerateObjectsUsingBlock:^(SCAVMixWorkItem * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        [SCAVMixManager appendWorkItem:obj];
    }];
    
//    self.config = [self makeConfig:@"1562663793812"];
    self.mixs = mixs;
    
    NSLog(@"items: %@", items);
    
    [SCAVMixManager resume];
}

- (SCAVMixConfig *)makeConfig:(NSString *)itemName {
    
    // mbp提示框
    // 路径
    NSString * documents = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    
    // 视频来源
    NSString * videoInputPath = [[NSBundle mainBundle] pathForResource:itemName ofType:@"mp4"];
    
    // 声音来源
    NSString * audioInputPath = [[NSBundle mainBundle] pathForResource:itemName ofType:@"wav"];
    
    SCAVMixConfig * config = [[SCAVMixConfig alloc] initWithIdentifier:itemName videoInputPath:videoInputPath audioInputPath:audioInputPath];

    
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"HHmmSS";
    NSString * dateString = [formatter stringFromDate:[NSDate date]];
    NSString * outputFileName = [NSString stringWithFormat:@"%@_%@", itemName, dateString];
    // 最终合成输出路径
    NSString *outPutFilePath = [documents stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.mp4", outputFileName]];
    // 添加合成路径
    config.outputPath = outPutFilePath;
    
    return config;
}


- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent:event];
    if (0 == self.status) {
        self.status = 1;
//        [self merge];
        
        [self goMixAtIndex:0];
        [self goMixAtIndex:1];
        [self goMixAtIndex:2];
        [self goMixAtIndex:3];


    }
}

- (void)goMixAtIndex:(NSInteger)index {
//    SCAVMixHandler * mix = self.mixs[index];
//    [mix startWithCompletion:^(SCAVMixHandler * _Nonnull handler) {
//        NSLog(@"handler: %@", handler);
//    }];
    
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

@end
