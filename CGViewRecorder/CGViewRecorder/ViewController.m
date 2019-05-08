//
//  ViewController.m
//  CGViewRecorder
//
//  Created by vector on 2019/5/7.
//  Copyright © 2019 vector. All rights reserved.
//

#import "ViewController.h"
#import "AFRecordView.h"
#import "AFVideoPlayView.h"



@interface ViewController ()

@property (nonatomic, strong) NSTimer * timer;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    AFRecordView * v = [[AFRecordView alloc] initWithFrame:CGRectMake(0, 100, 800, 800)];
    [self.view addSubview:v];
    
    AFVideoPlayView * videoView = [[AFVideoPlayView alloc] initWithFrame:CGRectMake(0, 0, 240, 135)];
    [v addSubview:videoView];
    [videoView play:@"https://vd4.bdstatic.com/mda-ib6g1fa7637hsx1r/hd/mda-ib6g1fa7637hsx1r.mp4?playlist=%5B%22hd%22%2C%22sc%22%5D&auth_key=1557242514-0-0-fd455e39ae91268409b699f679d73861&bcevod_channel=searchbox_feed&pd=bjh"];
    
    
    AFVideoPlayView * videoView1 = [[AFVideoPlayView alloc] initWithFrame:CGRectMake(0, 250, 240, 135)];
    [v addSubview:videoView1];

    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [videoView1 play:@"https://vd4.bdstatic.com/mda-ib6g1fa7637hsx1r/hd/mda-ib6g1fa7637hsx1r.mp4?playlist=%5B%22hd%22%2C%22sc%22%5D&auth_key=1557242514-0-0-fd455e39ae91268409b699f679d73861&bcevod_channel=searchbox_feed&pd=bjh"];

        AFVideoPlayView * videoView2 = [[AFVideoPlayView alloc] initWithFrame:CGRectMake(300, 250, 240, 135)];
        [v addSubview:videoView2];
        
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [videoView2 play:@"https://vd4.bdstatic.com/mda-ib6g1fa7637hsx1r/hd/mda-ib6g1fa7637hsx1r.mp4?playlist=%5B%22hd%22%2C%22sc%22%5D&auth_key=1557242514-0-0-fd455e39ae91268409b699f679d73861&bcevod_channel=searchbox_feed&pd=bjh"];
            
        });
        

    
    });
    
    
    
    
    
    NSTimer * timer = [NSTimer timerWithTimeInterval:1.0/120 repeats:true block:^(NSTimer * _Nonnull timer) {
        UIImage * img = [v screenShot];
    }];
    
    self.timer = timer;
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];
    
    
    
}
//https://sv.baidu.com/videoui/page/videoland?pd=bjh&context=%7B%22nid%22:%2215804056819619379975%22,%22sourceFrom%22:%22bjh%22%7D&fr=bjhauthor&type=video

@end
