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
@property (nonatomic, strong) CADisplayLink * displayLink;

@property (nonatomic, strong) AFRecordView * recordView;
@end

@implementation ViewController
/*
 NSCalendar * calendar = [NSCalendar currentCalendar];
 
 NSDateComponents *components = [[NSDateComponents alloc] init];
 [components setYear:2019];
 [components setMonth:6];
 
 for (NSInteger day=0; day<=32; day++) {
 [components setDay:day];
 
 NSDate * date = [calendar dateFromComponents:components];
 
 NSDateComponents * components1 = [calendar components:NSCalendarUnitDay | NSCalendarUnitWeekOfMonth | NSCalendarUnitWeekday | NSCalendarUnitMonth | NSCalendarUnitYear fromDate:date];
 
 NSLog(@"[%@ ]\n [%@]\n [%@]\n [%ld %ld]", date, components, components1, components1.weekday, components1.weekOfMonth);
 }
 return;
 */
- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    
    AFRecordView * v = [[AFRecordView alloc] initWithFrame:CGRectMake(0, 20, 1000, 600)];
    [self.view addSubview:v];
    self.recordView = v;
    
    for (NSInteger i=0; i<10; i++) {
        NSString * url = nil;
        CGSize size = CGSizeMake(220, 120);
        if (i % 4 == 0) {
            url = [[NSBundle mainBundle] pathForResource:@"m300" ofType:@"mp4"];
            size = CGSizeMake(245, 145);
        } else {
            url = [[NSBundle mainBundle] pathForResource:@"m200" ofType:@"mp4"];
        }
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(i * 1.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            AFVideoPlayView * videoView = [[AFVideoPlayView alloc] initWithFrame:CGRectMake(i % 4 * 250, i / 4 * 150, size.width, size.height)];
            [v addSubview:videoView];
            
            [videoView play:url];
        });
    }
    
    
    UIImageView * imageView = [[UIImageView alloc] initWithFrame:CGRectMake(300, 0, 200, 120)];
    [self.view addSubview:imageView];
    imageView.layer.borderColor = [UIColor blueColor].CGColor;
    imageView.layer.borderWidth = 2;
    v.onImage = ^(UIImage * image, CFTimeInterval time) {
        imageView.image = image;
    };
//
//    NSTimer * timer = [NSTimer timerWithTimeInterval:1.0/30 repeats:true block:^(NSTimer * _Nonnull timer) {
//        [v setNeedsDisplay];
//    }];
//
//    self.timer = timer;
//    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];

    // 1.初始化
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(displayLink:)];
    // 2. 设置 - 2桢回调一次，这里非时间，而是以桢为单位
    self.displayLink.preferredFramesPerSecond = 30;
    
    // 3.加入RunLoop
    [self.displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];

//    // 7.暂停
//    self.displayLink.paused = YES;
//    // 8.销毁
//    [self.displayLink invalidate];
//    self.displayLink = nil;
    
    
}

//
// 4.callback
- (void)displayLink:(CADisplayLink *)displayLink {
    [self.recordView dis];
}

//https://sv.baidu.com/videoui/page/videoland?pd=bjh&context=%7B%22nid%22:%2215804056819619379975%22,%22sourceFrom%22:%22bjh%22%7D&fr=bjhauthor&type=video

@end