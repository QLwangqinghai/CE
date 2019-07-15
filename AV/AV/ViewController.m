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
    
//    NSArray<NSString *> * items = @[@"1562930748136", @"1562928616807", @"1562663973911", @"1562662353087", @"1562662173010", @"1562661992940", @"1562663793812", @"1562641927402"];
    NSArray<NSString *> * items = @[@"1562930748136"];

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
        [item startWithQueue:[SCAVMixManager queue] completion:^(id<SCAVMixWorkItemProtocol>  _Nonnull item) {
            NSLog(@"%@", item);
        }];
        [mixs addObject:item];
    }];

    
//    [mixs enumerateObjectsUsingBlock:^(SCAVMixWorkItem * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
//        [SCAVMixManager appendWorkItem:obj];
//    }];
//    
//    self.config = [self makeConfig:@"1562663793812"];
    self.mixs = mixs;
    
    NSLog(@"items: %@", items);
    
    [SCAVMixManager resume];
}

- (SCAVMixConfig *)makeConfig:(NSString *)itemName {
    SCAVMixConfig * config = [[SCAVMixConfig alloc] initWithIdentifier:itemName];
    
    // mbp提示框
    // 路径
    NSString *documents = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    
    // 视频来源
    config.videoInputPath = [[NSBundle mainBundle] pathForResource:itemName ofType:@"mp4"];
    
    // 声音来源
//    config.audioInputPath = [[NSBundle mainBundle] pathForResource:itemName ofType:@"wav"];
    config.audioInputPath = [config.videoInputPath stringByReplacingOccurrencesOfString:@"mp4" withString:@"wav"];
    
    
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
//        [self goMixAtIndex:1];
//        [self goMixAtIndex:2];
//        [self goMixAtIndex:3];


    }
}

- (void)goMixAtIndex:(NSInteger)index {
//    SCAVMixHandler * mix = self.mixs[index];
//    [mix startWithCompletion:^(SCAVMixHandler * _Nonnull handler) {
//        NSLog(@"handler: %@", handler);
//    }];
    
}




@end
