//
//  FFmpegManager.m
//  ZJHVideoProcessing
//
//  Created by ZhangJingHao2345 on 2018/1/29.
//  Copyright © 2018年 ZhangJingHao2345. All rights reserved.
//

#import "FFmpegManager.h"
#import <AVFoundation/AVFoundation.h>

#include "ffmpeg.h"

/*
 
 命令行参数-crf
 
 其中0为无损模式，数值越大，画质越差，生成的文件却越小。
 在优先保证画面质量（也不太在乎转码时间）的情况下，使用-crf参数来控制转码是比较适宜的。这个参数的取值范围为0~51，其中0为无损模式，数值越大，画质越差，生成的文件却越小。从主观上讲，18~28是一个合理的范围。18被认为是视觉无损的（从技术角度上看当然还是有损的），它的输出视频质量和输入视频相当。
 
 */

@interface FFmpegManager ()

@property (nonatomic, assign) BOOL isRuning;
@property (nonatomic, assign) BOOL isBegin;
@property (nonatomic, assign) long long fileDuration;
@property (nonatomic, copy) void (^processBlock)(float process);
@property (nonatomic, copy) void (^completionBlock)(NSError *error);

@property (nonatomic, strong) NSThread *thread;

@end

@implementation FFmpegManager

+ (instancetype)sharedManager {
    static FFmpegManager *manager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken,^{
        manager = [[super allocWithZone:NULL] init];
    });
    return manager;
}

/**
 * 重写 +allocWithZone 方法，使得在给对象分配内存空间的时候，就指向同一份数据
 */
+ (id)allocWithZone:(struct _NSZone *)zone {
    return [self sharedManager];
}

- (instancetype)init {
    if (self = [super init]) {
    }
    return self;
}

- (BOOL)dirExistsAtPath:(NSString *)path {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL isDir = FALSE;
    BOOL isDirExist = [fileManager fileExistsAtPath:path isDirectory:&isDir];
    if(isDirExist && isDir) {
        return YES;
    }
    return NO;
}

- (void)createDirectoryAtPath:(NSString *)path {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    [fileManager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:nil];
}

- (void)compressMp4WithInputPath:(NSString *)inputPath
                      outputPath:(NSString *)outputPath
                      outputSize:(CGSize)size
                     threadCount:(NSInteger)threadCount
                             fps:(NSInteger)fps
                    videoBitRate:(NSInteger)videoBitRate
                    processBlock:(void (^)(float process))processBlock
                 completionBlock:(void (^)(NSError *error))completionBlock {
    NSString *dirPath = [outputPath stringByDeletingLastPathComponent];
    if (![self dirExistsAtPath:dirPath]) {
        [self createDirectoryAtPath:dirPath];
    }
    // ffmpeg语法，可根据需求自行更改
    // !#$ 为分割标记符，也可以使用空格代替
    
    NSInteger w = size.width;
    NSInteger h = size.height;
    if (w % 2 != 0) {
        w -= 1;
    }
    if (h % 2 != 0) {
        h -= 1;
    }
    NSString *commandStr = [NSString stringWithFormat:@"ffmpeg!#$-threads!#$%ld!#$-i!#$%@!#$-r!#$%ld!#$-crf!#$25!#$-b:a!#$24k!#$-b:v!#$%ldk!#$-c:v!#$libx264!#$-c:a!#$aac!#$-tune!#$ssim!#$-max_muxing_queue_size!#$1024!#$-s!#$%ldx%ld!#$-y!#$%@", threadCount, inputPath, fps, videoBitRate, w, h, outputPath];
    [self convertWithCommand:commandStr processBlock:processBlock completionBlock:^(NSError *error) {
        if (![[NSFileManager defaultManager] fileExistsAtPath:outputPath] && nil == error) {
            if (completionBlock) {
                completionBlock([NSError errorWithDomain:@"haoqi" code:1 userInfo:nil]);
            }
        } else {
            if ([[[NSFileManager defaultManager] attributesOfItemAtPath:outputPath error:NULL][NSFileSize] longLongValue] > 0) {
                if (completionBlock) {
                    completionBlock(error);
                }
            } else {
                if (completionBlock) {
                    completionBlock([NSError errorWithDomain:@"haoqi" code:1 userInfo:nil]);
                }
            }
        }
    }];
}


- (void)compressMp4WithInputPath:(NSString *)inputPath
                      outputPath:(NSString *)outputPath
                     threadCount:(NSInteger)threadCount
                             fps:(NSInteger)fps
                    videoBitRate:(NSInteger)videoBitRate
                    processBlock:(void (^)(float process))processBlock
                 completionBlock:(void (^)(NSError *error))completionBlock {
    NSString *dirPath = [outputPath stringByDeletingLastPathComponent];
    if (![self dirExistsAtPath:dirPath]) {
        [self createDirectoryAtPath:dirPath];
    }
    // ffmpeg语法，可根据需求自行更改
    // !#$ 为分割标记符，也可以使用空格代替
    NSString *commandStr = [NSString stringWithFormat:@"ffmpeg!#$-threads!#$%ld!#$-i!#$%@!#$-r!#$%ld!#$-crf!#$25!#$-b:a!#$24k!#$-b:v!#$%ldk!#$-c:v!#$libx264!#$-c:a!#$aac!#$-tune!#$ssim!#$-max_muxing_queue_size!#$1024!#$-y!#$%@", threadCount, inputPath, fps, videoBitRate, outputPath];
        
    [self convertWithCommand:commandStr processBlock:processBlock completionBlock:^(NSError *error) {
        if (![[NSFileManager defaultManager] fileExistsAtPath:outputPath] && nil == error) {
            if (completionBlock) {
                completionBlock([NSError errorWithDomain:@"haoqi" code:1 userInfo:nil]);
            }
        } else {
            if ([[[NSFileManager defaultManager] attributesOfItemAtPath:outputPath error:NULL][NSFileSize] longLongValue] > 0) {
                if (completionBlock) {
                    completionBlock(error);
                }
            } else {
                if (completionBlock) {
                    completionBlock([NSError errorWithDomain:@"haoqi" code:1 userInfo:nil]);
                }
            }
        }
    }];
}

// 转换视频
- (void)convertWithInputPath:(NSString *)inputPath
                 outputPath:(NSString *)outputPath
               processBlock:(void (^)(float process))processBlock
            completionBlock:(void (^)(NSError *error))completionBlock {
    NSString *dirPath = [outputPath stringByDeletingLastPathComponent];
    if (![self dirExistsAtPath:dirPath]) {
        [self createDirectoryAtPath:dirPath];
    }
    // ffmpeg语法，可根据需求自行更改
    // !#$ 为分割标记符，也可以使用空格代替
    NSString *commandStr = [NSString stringWithFormat:@"ffmpeg!#$-i!#$%@!#$-c:a!#$aac!#$-y!#$%@", inputPath, outputPath];
    [self convertWithCommand:commandStr processBlock:processBlock completionBlock:completionBlock];
}

// 合并视频和音频
- (void)convertWithAudioPath:(NSString *)audioPath
                  videoPath:(NSString *)videoPath
                 outputPath:(NSString *)outputPath
               processBlock:(void (^)(float process))processBlock
            completionBlock:(void (^)(NSError *error))completionBlock {
    NSString *dirPath = [outputPath stringByDeletingLastPathComponent];
    if (![self dirExistsAtPath:dirPath]) {
        [self createDirectoryAtPath:dirPath];
    }
    // ffmpeg语法，可根据需求自行更改
    // !#$ 为分割标记符，也可以使用空格代替
    NSString *commandStr = [NSString stringWithFormat:@"ffmpeg!#$-threads!#$2!#$-i!#$%@!#$-i!#$%@!#$-r!#$15!#$-crf!#$25!#$-b:a!#$23k!#$-b:v!#$50k!#$-c:v!#$libx264!#$-c:a!#$aac!#$-tune!#$ssim!#$-y!#$%@", audioPath, videoPath, outputPath];
    [self convertWithCommand:commandStr processBlock:processBlock completionBlock:completionBlock];
}

- (void)convertWithCommand:(NSString *)commandStr
              processBlock:(void (^)(float process))processBlock
           completionBlock:(void (^)(NSError *error))completionBlock {
    self.processBlock = processBlock;
    self.completionBlock = completionBlock;
    self.isBegin = NO;
    // 放在子线程运行
    NSThread *thread = [[NSThread alloc] initWithTarget:self selector:@selector(runCmd:) object:commandStr];
    self.thread = thread;
    [thread start];
}

- (void)stopConverting {
    [self.thread cancel];
    self.thread = nil;
}

// 执行指令
- (void)runCmd:(NSString *)commandStr {
    // 判断转换状态
    if (self.isRuning) {
        NSLog(@"正在转换,稍后重试");
        return;
    }
    self.isRuning = YES;
    
    // 根据 !#$ 将指令分割为指令数组
    NSArray *argv_array = [commandStr componentsSeparatedByString:(@"!#$")];
    // 将OC对象转换为对应的C对象
    int argc = (int)argv_array.count;
    char** argv = (char**)malloc(sizeof(char*)*argc);
    for(int i = 0; i < argc; i++) {
        argv[i] = (char*)malloc(sizeof(char)*1024);
        strcpy(argv[i],[[argv_array objectAtIndex:i] UTF8String]);
    }
    
    // 打印日志
    NSString *finalCommand = @"ffmpeg 运行参数:";
    for (NSString *temp in argv_array) {
        finalCommand = [finalCommand stringByAppendingFormat:@"%@ ",temp];
    }
//    NSLog(@"%@", finalCommand);
    
    // 传入指令数及指令数组
    ffmpeg_main(argc, argv);
    
    for(int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
}

- (BOOL)checkWithVideoPath:(NSString *)videoPath
                 audioPath:(NSString *)audioPath {
    //检查video时长
    AVAsset *videoAsset = [AVAsset assetWithURL:[NSURL fileURLWithPath:videoPath]];
    if (!videoAsset || !videoAsset.isPlayable) {
        return NO;
    }
    CMTime videoDuration = videoAsset.duration;
    float videoSeconds = CMTimeGetSeconds(videoDuration);
    
    AVAsset *audioAsset = [AVAsset assetWithURL:[NSURL fileURLWithPath:audioPath]];
    if (!audioAsset || !audioAsset.isPlayable) {
        return NO;
    }
    CMTime audioDuration = audioAsset.duration;
    float audioSeconds = CMTimeGetSeconds(audioDuration);
    
    // 小于 1 秒认为不合法
    if (videoSeconds < 1 || audioSeconds < 1) {
        return NO;
    }
    
    // 判断差距，误差 5% 或 小于 1 秒
    float error = 0.05;
    float delta = fabsf(videoSeconds - audioSeconds);
    if (delta <= 1 || delta / videoSeconds <= error) {
        return YES;
    }
    return NO;
}

- (BOOL)checkConvertWithInputPath:(NSString *)inputPath
                       outputPath:(NSString *)outputPath {
    //检查video时长
    AVAsset *inputAsset = [AVAsset assetWithURL:[NSURL fileURLWithPath:inputPath]];
    if (!inputAsset || !inputAsset.playable) {
        return NO;
    }
    CMTime inputDuration = inputAsset.duration;
    float inputSeconds = CMTimeGetSeconds(inputDuration);
    
    AVAsset *outputAsset = [AVAsset assetWithURL:[NSURL fileURLWithPath:outputPath]];
    if (!outputAsset || !outputAsset.playable) {
        return NO;
    }
    CMTime outputDuration = outputAsset.duration;
    float outputSeconds = CMTimeGetSeconds(outputDuration);
    
    // 小于 1 秒认为不合法
    if (inputSeconds < 1 || outputSeconds < 1) {
        return NO;
    }
    
    // 判断差距，误差 5% 或 小于 1 秒
    float error = 0.05;
    float delta = fabsf(outputSeconds - inputSeconds);
    if (delta <= 1 || delta / inputSeconds <= error) {
        return YES;
    }
    return NO;
}

// 设置总时长
+ (void)setDuration:(long long)time {
    [FFmpegManager sharedManager].fileDuration = time;
}

// 设置当前时间
+ (void)setCurrentTime:(long long)time {
    FFmpegManager *mgr = [FFmpegManager sharedManager];
    mgr.isBegin = YES;
    
    if (mgr.processBlock && mgr.fileDuration) {
        float process = time/(mgr.fileDuration * 1.00);
        if (process < 0 || process > 1) {
            process = 0.f;
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            mgr.processBlock(process);
        });
    }
}

+ (void)stopRuning {
    FFmpegManager *mgr = [FFmpegManager sharedManager];
    NSError *error = nil;
    // 判断是否开始过
    if (!mgr.isBegin) {
        // 没开始过就设置失败
        error = [NSError errorWithDomain:@"转换失败,请检查源文件的编码格式!"
                                    code:0
                                userInfo:nil];
    }
    if (mgr.completionBlock) {
        dispatch_async(dispatch_get_main_queue(), ^{
            mgr.completionBlock(error);
        });
    }
    
    mgr.isRuning = NO;
}

@end
