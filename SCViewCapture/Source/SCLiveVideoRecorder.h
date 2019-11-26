//
//  SCViewCapture.h
//  SCViewCapture
//
//  Created by vector on 2019/11/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <mach/mach_time.h>

NS_ASSUME_NONNULL_BEGIN

@interface SCScreenVideoRecordOptions : NSObject
@property (nonatomic, assign) NSInteger fps;
@property (nonatomic, assign) NSInteger videoWidth;
@property (nonatomic, assign) NSInteger videoHeight;
@property (nonatomic, assign) NSInteger bitRate;
@property (nonatomic, assign) double bitRateScale;

@end

@interface SCVideoRecorderFrame : NSObject
@property (nonatomic, strong) UIImage *image;
@property (nonatomic, assign) CGRect rect;
@property (nonatomic, assign) CMTime presentationTime;
@end


@interface SCLiveRoomVideoRecorder : NSObject
@property (nonatomic, assign, readonly) BOOL isOpenVideo;
@property (nonatomic, copy) void (^onVideoFrame)(CVPixelBufferRef pixelBuffer, CGRect rect, CMTime prentationTime);

- (void)openVideo;
- (void)closeVideo;
@end


@interface SCLiveRoomAudioRecorder : NSObject

@property (nonatomic, copy) void (^onAudioPCMBuffer)(SCLiveRoomAudioRecorder * recorder, AVAudioPCMBuffer * buffer, uint64_t time);

- (void)start;
- (void)finish;

@end


//抽象类
@interface SCViewCaptureItem : NSObject

//- (void)start;
//- (void)finish;

//在录屏中的frame
@property (nonatomic, assign, readonly) CGRect frame;

//z 轴位置
@property (nonatomic, assign, readonly) CGFloat z;

//@property (nonatomic, strong) CGImage frame;

//frame 1 倍模式
- (instancetype)initWithFrame:(CGRect)frame z:(CGFloat)z;

- (void)draw:(CGContextRef)context;

@end


@interface SCViewCaptureImageItem : SCViewCaptureItem

@property (nonatomic, assign, readonly) UIImage * image;

//为了更好的性能，外部传进来一个frame.size 大小的图片
- (instancetype)initWithFrame:(CGRect)frame z:(CGFloat)z image:(UIImage *)image;


@end

@interface SCViewCaptureViewItem : SCViewCaptureItem

@property (nonatomic, weak, readonly) UIView * view;

//为了更好的性能，外部传进来一个frame.size 大小的图片
- (instancetype)initWithFrame:(CGRect)frame z:(CGFloat)z view:(UIView *)view;

@end


@protocol SCViewCaptureQuickViewProtocol <NSObject>

@property (atomic, assign, readonly) uint64_t lastUpdateTime;//使用 mach_absolute_time(), 如果值为0 会默认需要更新

@end

@interface SCViewCaptureQuickViewItem : SCViewCaptureItem

@property (nonatomic, weak, readonly) UIView<SCViewCaptureQuickViewProtocol> * view;

//为了更好的性能，外部传进来一个frame.size 大小的图片
- (instancetype)initWithFrame:(CGRect)frame z:(CGFloat)z view:(UIView<SCViewCaptureQuickViewProtocol> *)view;

@end


@interface SCViewCapture : NSObject

//只能在主线程操作
- (void)insert:(SCViewCaptureItem *)item;

//只能在主线程操作
- (void)remove:(SCViewCaptureItem *)item;

- (void)removeAll;

@end


@protocol SCScreenCaptureDelegate <NSObject>

- (void)xScreenCaptureDidProgress:(float)progress;
- (void)xScreenCaptureDidFinsished:(BOOL)pausedManually;
- (void)xScreenCaptureDidDuration:(CGFloat)duration;
@end
@interface SCLiveVideoRecorder : NSObject

@property (nonatomic,copy) NSString* strLogRoom;
@property (nonatomic, strong) UIView *captureView;
@property (nonatomic, assign) CGFloat duration;
@property (nonatomic, assign) BOOL isRecordFile;

@property (nonatomic, strong, readonly) SCLiveRoomVideoRecorder *videoRecorder;
@property (nonatomic, weak) id<SCScreenCaptureDelegate> delegate;

- (instancetype)initWithCapture:(SCViewCapture *)capture options:(SCScreenVideoRecordOptions *)options filePath:(NSString *)filePath;

//-(instancetype)initWithVideoIndex:(NSString*)videoPath andlogRoomID:(NSString*)logroomID;
//- (BOOL)prepareToRecord;
- (void)start;
//- (void)pause;//这个方法不支持
- (void)finishedRecord:(BOOL)pausedManually;


@end

NS_ASSUME_NONNULL_END
