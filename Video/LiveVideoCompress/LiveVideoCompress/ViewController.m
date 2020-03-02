//
//  ViewController.m
//  LiveVideoCompress
//
//  Created by vector on 2020/2/5.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "ViewController.h"
#import "FFmpegManager.h"
#include "FFmpegVideoRecorder.h"


/*
 测试结果
 输入 3分钟 10.5mb
 6p:
 
 2线程压缩 文件大小 5454KB, 耗时108.2s
 
 */

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    
    self.view.backgroundColor = [UIColor whiteColor];
    
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        [FFAVWriter test];
    });
    
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent:event];
    
    NSString * path = [NSHomeDirectory() stringByAppendingString: @"/Documents/100003231/Playback/56552_66210/output/1580710816436.mp4"];
    
    NSString * output = [NSHomeDirectory() stringByAppendingFormat: @"/Documents/100003231/Playback/56552_66210/output/1580710816436_%u.mp4", arc4random()];

    NSLog(@"开始");

    NSTimeInterval time = CFAbsoluteTimeGetCurrent();
    [[FFmpegManager sharedManager] compressMp4WithInputPath:path outputPath:output fps:15 videoBitRate:400 processBlock:^(float process) {
        NSLog(@"进度:%.02f", process);
    } completionBlock:^(NSError *error) {
        NSLog(@"结束");
        NSData * data = [NSData dataWithContentsOfFile:output];
        NSLog(@"2线程压缩 文件大小 %ldKB, 耗时%.01lfs", data.length / 1024, CFAbsoluteTimeGetCurrent() - time);
    }];
    
}



@end
