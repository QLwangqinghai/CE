//
//  FFmpegManager.h
//  ZJHVideoProcessing
//
//  Created by ZhangJingHao2345 on 2018/1/29.
//  Copyright © 2018年 ZhangJingHao2345. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FFmpegManager : NSObject

+ (FFmpegManager *)sharedManager;


// bitRate的单位是Kbs

/**
压缩视频

@param inputPath 输入视频路径
@param outputPath 输出视频路径
@param fps fps
@param videoBitRate 单位是Kbs
@param processBlock 进度回调
@param completionBlock 结束回调
*/
- (void)compressMp4WithInputPath:(NSString *)inputPath
                      outputPath:(NSString *)outputPath
                             fps:(NSInteger)fps
                    videoBitRate:(NSInteger)videoBitRate
                    processBlock:(void (^)(float process))processBlock
                 completionBlock:(void (^)(NSError *error))completionBlock;



/**
 合并视频和音频

 @param audioPath 输入视频路径
 @param videoPath 输入音频路径
 @param outputPath 输出视频路径
 @param processBlock 进度回调
 @param completionBlock 结束回调
 */
- (void)convertWithAudioPath:(NSString *)audioPath
                  videoPath:(NSString *)videoPath
                 outputPath:(NSString *)outputPath
               processBlock:(void (^)(float process))processBlock
            completionBlock:(void (^)(NSError *error))completionBlock;

- (void)stopConverting;


/**
 检查音视频文件是否匹配

 @param videoPath videoPath
 @param audioPath audioPath
 @return 是否匹配
 */
- (BOOL)checkWithVideoPath:(NSString *)videoPath
                 audioPath:(NSString *)audioPath;

/**
 检查文件是否转换成功
 
 @param inputPath 输入视频路径
 @param outputPath 输出视频路径
 @return 是否成功
 */
- (BOOL)checkConvertWithInputPath:(NSString *)inputPath
                       outputPath:(NSString *)outputPath;

// 设置总时长
+ (void)setDuration:(long long)time;

// 设置当前时间
+ (void)setCurrentTime:(long long)time;

+ (void)stopRuning;

@end
