//
//  ViewController.m
//  LiveVideoCompress
//
//  Created by vector on 2020/2/5.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "ViewController.h"
#import "FFmpegManager.h"


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
    
    
    
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent:event];
    
    NSString * path = [[NSBundle mainBundle] pathForResource:@"v1080" ofType:@"mp4"];
    
    NSString * output = [NSHomeDirectory() stringByAppendingFormat: @"/Documents/1580710816436_%u.mp4", arc4random()];

    NSLog(@"开始");

    NSTimeInterval time = CFAbsoluteTimeGetCurrent();
    [[FFmpegManager sharedManager] compressMp4WithInputPath:path outputPath:output threadCount:4 fps:15 videoBitRate:400 processBlock:^(float process) {
        NSLog(@"进度:%.02f", process);
    } completionBlock:^(NSError *error) {
        NSLog(@"结束");
        NSDictionary * dict = [[NSFileManager defaultManager] attributesOfItemAtPath:output error:NULL];
        NSLog(@"2线程压缩 文件大小 %ldKB, 耗时%.01lfs", [dict[NSFileSize] integerValue] / 1024, CFAbsoluteTimeGetCurrent() - time);
    }];
    
}



@end
