//
//  AFVideoPlayView.m
//  CGViewRecorder
//
//  Created by vector on 2019/5/7.
//  Copyright © 2019 vector. All rights reserved.
//

#import "AFVideoPlayView.h"
#import <AVFoundation/AVFoundation.h>


@interface AFVideoPlayView ()

@property (nonatomic, strong) AVPlayer *player;//播放器对象

@property (nonatomic, strong) AVPlayerItem *currentPlayerItem;


@end
@implementation AFVideoPlayView


- (void)play:(NSString *)urlString {
    if (nil == urlString) {
        return;
    }
    NSURL * url = nil;
    
    if ([urlString hasPrefix:@"/"]) {
        url = [NSURL fileURLWithPath:urlString];
    } else {
        url = [NSURL URLWithString:urlString];
    }
    
    if (nil == url) {
        return;
    }

    AVPlayerItem *playerItem = [[AVPlayerItem alloc] initWithURL:url];
    self.currentPlayerItem = playerItem;
    self.player = [[AVPlayer alloc] initWithPlayerItem:playerItem];
    
//    AVPlayerLayer *avLayer = [AVPlayerLayer playerLayerWithPlayer:self.player];
//    avLayer.videoGravity = AVLayerVideoGravityResizeAspect;
//    avLayer.frame = self.bounds;
//    [self.layer addSublayer:avLayer];
    
    ((AVPlayerLayer *)self.layer).player = self.player;
    
    
    [self.player play];

}
- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx {
    [super drawLayer:layer inContext:ctx];

    NSLog(@"123");
}

+ (Class)layerClass {
    return [AVPlayerLayer class];
}

@end
