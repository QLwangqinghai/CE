//
//  SCViewCapture.m
//  SCViewCapture
//
//  Created by vector on 2019/11/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "SCLiveVideoRecorder.h"

#ifndef weakify
    #if DEBUG
        #if __has_feature(objc_arc)
        #define weakify(object) autoreleasepool{} __weak __typeof__(object) weak##_##object = object;
        #else
        #define weakify(object) autoreleasepool{} __block __typeof__(object) block##_##object = object;
        #endif
    #else
        #if __has_feature(objc_arc)
        #define weakify(object) try{} @finally{} {} __weak __typeof__(object) weak##_##object = object;
        #else
        #define weakify(object) try{} @finally{} {} __block __typeof__(object) block##_##object = object;
        #endif
    #endif
#endif

#ifndef strongify
    #if DEBUG
        #if __has_feature(objc_arc)
        #define strongify(object) autoreleasepool{} __typeof__(object) object = weak##_##object;
        #else
        #define strongify(object) autoreleasepool{} __typeof__(object) object = block##_##object;
        #endif
    #else
        #if __has_feature(objc_arc)
        #define strongify(object) try{} @finally{} __typeof__(object) object = weak##_##object;
        #else
        #define strongify(object) try{} @finally{} __typeof__(object) object = block##_##object;
        #endif
    #endif
#endif


@implementation SCScreenVideoRecordOptions

@end


@implementation SCVideoRecorderFrame
@end

@interface SCLiveRoomVideoRecorder ()
@property (nonatomic, assign, readwrite) BOOL isOpenVideo;
@end
@implementation SCLiveRoomVideoRecorder
- (instancetype)init {
    if (self = [super init]) {
        self.isOpenVideo = YES;
    }
    return self;
}
- (void)openVideo {
    self.isOpenVideo = YES;
}
- (void)closeVideo {
    self.isOpenVideo = NO;
}
@end


@interface SCLiveRoomAudioRecorder ()

@property (nonatomic, copy) NSString * key;
@property (nonatomic, assign) BOOL working;

@end
@implementation SCLiveRoomAudioRecorder

- (instancetype)init {
    self = [super init];
    if (self) {
        _key = [[NSString stringWithFormat:@"%p", self] copy];
    }
    return self;
}

- (void)dealloc {
}

- (void)start {
    self.working = true;
}
- (void)finish {
    self.working = false;
}

- (void)onAudioPcmBuffer:(AVAudioPCMBuffer *)buffer atTime:(uint64_t)time {
    if (self.onAudioPCMBuffer && self.working) {
        self.onAudioPCMBuffer(self, buffer, time);
    }
}
@end



@interface SCViewCaptureItem ()
{
    CVImageBufferRef _cachedImage;
}
@end
@implementation SCViewCaptureItem


- (instancetype)initWithFrame:(CGRect)frame z:(CGFloat)z{
    self = [super init];
    if (self) {
        _frame = frame;
        _z = z;
//        _key = [[NSString stringWithFormat:@"%p", self] copy];
        
//        CGBitmapContextCreate(<#void * _Nullable data#>, <#size_t width#>, <#size_t height#>, <#size_t bitsPerComponent#>, <#size_t bytesPerRow#>, <#CGColorSpaceRef  _Nullable space#>, <#uint32_t bitmapInfo#>)
        CVPixelBufferRef buffer = NULL;
        CVReturn code = CVPixelBufferCreate(NULL, frame.size.width, frame.size.height, kCVPixelFormatType_32BGRA, NULL, &buffer);
        _cachedImage = buffer;
        NSAssert(buffer, @"CVPixelBufferCreate create failure");
//        NSDictionary *bufferAttributes = @{(id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA),
//                                           (id)kCVPixelBufferCGBitmapContextCompatibilityKey : @YES,
//                                           (id)kCVPixelBufferWidthKey : @(videoSize.width),
//                                           (id)kCVPixelBufferHeightKey : @(videoSize.height),
//                                           (id)kCVPixelBufferBytesPerRowAlignmentKey : @(videoSize.width * 4)
//                                           };
        
    }
    return self;
}

- (void)dealloc {
    if (_cachedImage) {
        CVPixelBufferRelease(_cachedImage);
    }
}
- (void)draw {
    
}

@end


@interface SCViewCaptureImageItem ()
{
    CVImageBufferRef _cachedImage;
}
@end
@implementation SCViewCaptureImageItem

- (instancetype)initWithFrame:(CGRect)frame z:(CGFloat)z image:(UIImage *)image {
    self = [super initWithFrame:frame z:z];
    if (self) {
        _image = image;
    }
    return self;
}

- (void)dealloc {
    if (_cachedImage) {
        CVPixelBufferRelease(_cachedImage);
    }
}

- (void)draw:(CGContextRef)context {
    [self.image drawInRect:self.frame];
}

@end


@interface SCViewCaptureViewItem ()
@end
@implementation SCViewCaptureViewItem

- (instancetype)initWithFrame:(CGRect)frame z:(CGFloat)z view:(UIView *)view {
    self = [super initWithFrame:frame z:z];
    if (self) {
        _view = view;
    }
    return self;
}

- (void)draw:(CGContextRef)context {
    [self.view drawViewHierarchyInRect:self.frame afterScreenUpdates:NO];
}

@end




@interface SCViewCaptureQuickViewItem ()
{
    CVImageBufferRef _cachedImage;
    CGColorSpaceRef _colorSpace;
    CGImageRef _cache;
}
@property (nonatomic, assign) uint64_t cacheTime;//使用 mach_absolute_time(), 如果值为0 会默认需要更新
@property (nonatomic, readonly) CVImageBufferRef cachedImage;
@property (nonatomic, readonly) CGColorSpaceRef colorSpace;

@property (nonatomic, assign, readonly) CGImageRef image;

@end
@implementation SCViewCaptureQuickViewItem

- (instancetype)initWithFrame:(CGRect)frame z:(CGFloat)z view:(UIView<SCViewCaptureQuickViewProtocol> *)view {
    self = [super initWithFrame:frame z:z];
    if (self) {
        _view = view;
        CVPixelBufferRef buffer = NULL;
        CVReturn code = CVPixelBufferCreate(NULL, frame.size.width, frame.size.height, kCVPixelFormatType_32BGRA, NULL, &buffer);
        _cachedImage = buffer;
        
        
        _colorSpace = CGColorSpaceCreateDeviceRGB();

    }
    return self;
}

- (void)updateImage:(CGImageRef)image time:(uint64_t)time {
    CGImageRef old = _image;
    if (image) {
        _image = CGImageRetain(image);
    } else {
        _image = nil;
    }
    if (old) {
        CGImageRelease(old);
    }
    self.cacheTime = time;
}

- (void)dealloc {
    if (_cachedImage) {
        CVPixelBufferRelease(_cachedImage);
    }
    if (_colorSpace) {
        CGColorSpaceRelease(_colorSpace);
    }
    if (_image) {
        CGImageRelease(_image);
    }
}

- (void)draw:(CGContextRef)context {
    __strong UIView<SCViewCaptureQuickViewProtocol> * view = self.view;
    if (nil == view) {
        return;
    }
    uint64_t lastUpdateTime = view.lastUpdateTime;
    if (self.cacheTime == 0 || lastUpdateTime == 0 || self.cacheTime != lastUpdateTime) {
        
        CVPixelBufferLockBaseAddress(self.cachedImage, 0);
        CGContextRef bitmapContext = NULL;
        CGSize viewSize = self.frame.size;
        bitmapContext = CGBitmapContextCreate(CVPixelBufferGetBaseAddress(self.cachedImage),
                                              viewSize.width,
                                              viewSize.height,
                                              8, CVPixelBufferGetBytesPerRow(self.cachedImage), self.colorSpace,
                                              kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst
                                              );
        CGAffineTransform flipVertical = CGAffineTransformMake(1, 0, 0, -1, 0, viewSize.height);
        CGContextConcatCTM(bitmapContext, flipVertical);
        NSAssert(bitmapContext, @"CGBitmapContextCreate create NULL");
        CGRect bounds = CGRectMake(0, 0, viewSize.width, viewSize.height);
        @try {
            [view drawViewHierarchyInRect:bounds afterScreenUpdates:NO];
        } @catch (NSException *exception) {
            CGContextSetFillColorWithColor(bitmapContext, [UIColor.whiteColor CGColor]);
            CGContextFillRect(bitmapContext, bounds);
            NSLog(@"viewToImage - 截屏闪退Cache ");
        } @finally {

        }
        CGImageRef imageRef = CGBitmapContextCreateImage(bitmapContext);
        [self updateImage:imageRef time:lastUpdateTime];
        CGContextRelease(bitmapContext);
    }
//    CGBitmapcontextge
    if (self.image) {
        CGContextDrawImage(context, self.frame, self.image);
    }
}

@end



@interface SCViewCapture ()
@property (nonatomic, strong, readonly) NSMutableArray<SCViewCaptureItem *> * array;

@property (atomic, strong) NSArray<SCViewCaptureItem *> * mirror;

@end
@implementation SCViewCapture
- (instancetype)init {
    self = [super init];
    if (self) {
        _array = [NSMutableArray array];
        _mirror = @[];
    }
    return self;
}

- (void)insert:(SCViewCaptureItem *)item {
    if (nil == item) {
        return;
    }
    [self.array addObject:item];
    
    [self.array sortUsingComparator:^NSComparisonResult(SCViewCaptureItem * _Nonnull obj1, SCViewCaptureItem * _Nonnull obj2) {
        return obj1.z < obj2.z;
    }];
    self.mirror = [self.array copy];
}
- (void)remove:(SCViewCaptureItem *)item {
    if (nil == item) {
        return;
    }
    [self.array removeObject:item];
    self.mirror = [self.array copy];
}
- (void)removeAll {
    [self.array removeAllObjects];
    self.mirror = @[];
}

@end

@interface SCLiveVideoRecorder ()
{
    CVPixelBufferPoolRef _pixelBufferPool;
    CGColorSpaceRef _colorSpace;
}

@property (nonatomic, strong) SCLiveRoomAudioRecorder * audioRecorder;
@property (nonatomic, strong) AVAssetWriterInput * audioWriterInput;

@property (nonatomic, strong) AVAssetWriter *assetWriter;
@property (nonatomic, strong) AVAssetWriterInput *assetWriterVideoInput;
@property (nonatomic, strong) AVAssetWriterInputPixelBufferAdaptor *wVideoAdaptor;
@property (nonatomic, strong) NSDictionary  *videoSettings;
@property (nonatomic, strong) NSString      *cacheVideoPath;
@property (nonatomic,strong) NSTimer *screenTimer;
@property (nonatomic,strong) NSOperationQueue *videoFrameWriteQueue;

@property (nonatomic,strong) NSLock * timerLock;

@property (nonatomic, strong) SCViewCapture * capture;
@property (nonatomic, strong) SCScreenVideoRecordOptions * options;


@property (nonatomic, assign) uint64_t logSequence;
@property (nonatomic, assign) uint64_t startAtNumber;
@property (nonatomic, assign) BOOL ready;
@property (nonatomic, assign) BOOL recording;

@property (nonatomic, assign) NSInteger timerCurCount;

@property (nonatomic, assign) BOOL isCompleteFristFrame;

@property (nonatomic, assign) NSInteger perFrameDrawDurationInRecordThread;
@property (nonatomic, assign) NSTimeInterval lastFrameTime;

@property (nonatomic, strong, readwrite) SCLiveRoomVideoRecorder *videoRecorder;
@property (nonatomic, strong) dispatch_semaphore_t semphore;
@end

#define SCLog NSLog

@implementation SCLiveVideoRecorder

- (instancetype)initWithCapture:(SCViewCapture *)capture options:(SCScreenVideoRecordOptions *)options filePath:(NSString *)filePath {
    if (self = [super init]) {
        self.capture = capture;
        self.options = options;
        self.duration = 3 * 60;
        self.cacheVideoPath = filePath;
        self.timerCurCount = 0;
        self.timerLock = [[NSLock alloc] init];
        unlink([_cacheVideoPath UTF8String]);
        [self prepareToRecord];
    }
    return self;
}

- (BOOL)prepareToRecord
{
    if (!_ready) {
        NSError            * error  = nil;
        AVAssetWriter      * writer = nil;
        AVAssetWriterInput * input  = nil;
        
        
        NSURL * url = [NSURL fileURLWithPath:_cacheVideoPath];
        
        writer = [[AVAssetWriter alloc] initWithURL:url fileType:AVFileTypeMPEG4 error:&error];
        
        if (error) {
            NSLog(@"%@", error);
            SCLog(@"prepareToRecord - 录屏准备失败 %@", self.strLogRoom);
            return NO;
        }
        //写入视频大小
        NSInteger numPixels = self.options.videoWidth * self.options.videoHeight;
        //每像素比特
        CGFloat bitsPerPixel = self.options.bitRateScale;
        if (bitsPerPixel < 0.295) {
            bitsPerPixel = 0.295;
        }
        NSInteger bitsPerSecond = numPixels * bitsPerPixel * (self.options.fps * bitsPerPixel);
        
        // 码率和帧率设置
        NSDictionary * compression = @{
                AVVideoAverageBitRateKey : @(bitsPerSecond),
                AVVideoExpectedSourceFrameRateKey : @(self.options.fps),
                AVVideoMaxKeyFrameIntervalKey : @(self.options.fps),
                AVVideoProfileLevelKey : AVVideoProfileLevelH264BaselineAutoLevel};
        
        NSDictionary * videoSettings = @{
                AVVideoCodecKey  : AVVideoCodecH264,
                AVVideoScalingModeKey : AVVideoScalingModeResizeAspectFill,
                AVVideoWidthKey  : @(self.options.videoWidth),
                AVVideoHeightKey : @(self.options.videoHeight),
                AVVideoCompressionPropertiesKey :  compression };
        
        input = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:videoSettings];
        input.expectsMediaDataInRealTime = YES;
        
        NSDictionary * attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey, nil];
        
        AVAssetWriterInputPixelBufferAdaptor * adaptor = [AVAssetWriterInputPixelBufferAdaptor assetWriterInputPixelBufferAdaptorWithAssetWriterInput:input sourcePixelBufferAttributes:attributes];
        
        if ([writer canAddInput:input]) {
            [writer addInput:input];
        } else {
            SCLog(@"prepareToRecord - writerInput失败 %@", self.strLogRoom);
        }
        
        NSDictionary * audioSetting = @{
                                        AVFormatIDKey : @(kAudioFormatMPEG4AAC),
                                        AVNumberOfChannelsKey : @(1),
//                                        AVSampleRateKey : @(22050),//32000
                                        AVSampleRateKey : @(32000),

                                        };
        
        AVAssetWriterInput * audioWriterInput = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeAudio outputSettings:audioSetting];
        //添加写入器
        if ([writer canAddInput:audioWriterInput]) {
            [writer addInput:audioWriterInput];
        } else {
            NSLog(@"canAddInput:audioWriterInputHandler");
            return false;
        }
        _audioWriterInput = audioWriterInput;
        
        _assetWriter           = writer;
        _assetWriterVideoInput = input;
        _wVideoAdaptor         = adaptor;
        _ready                 = YES;
        _videoSettings         = videoSettings;
        
        [_assetWriter startWriting];
        [_assetWriter startSessionAtSourceTime:kCMTimeZero];
        
    }
    
    return YES;
}

- (void)creatPixelBufferPool {
    _colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGSize videoSize = [self videoSize];
    NSDictionary *bufferAttributes = @{(id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA),
                                       (id)kCVPixelBufferCGBitmapContextCompatibilityKey : @YES,
                                       (id)kCVPixelBufferWidthKey : @(videoSize.width),
                                       (id)kCVPixelBufferHeightKey : @(videoSize.height),
                                       (id)kCVPixelBufferBytesPerRowAlignmentKey : @(videoSize.width * 4)
                                       };
    
    _pixelBufferPool = NULL;
    CVPixelBufferPoolCreate(NULL, NULL, (__bridge CFDictionaryRef)(bufferAttributes), &_pixelBufferPool);
}

- (CGSize)videoSize {
    return CGSizeMake(self.options.videoWidth, self.options.videoHeight);
}


- (void)start
{
    self.isRecordFile = YES;
    
    if (_ready && !_recording) {
        self.videoFrameWriteQueue = [[NSOperationQueue alloc] init];
        self.videoFrameWriteQueue.maxConcurrentOperationCount = 1;
        
        [self creatPixelBufferPool];
        _recording = YES;
        _startAtNumber = mach_absolute_time();
        [self startTimer];
        
        self.audioRecorder = [[SCLiveRoomAudioRecorder alloc] init];
        @weakify(self);
        self.audioRecorder.onAudioPCMBuffer = ^(SCLiveRoomAudioRecorder * _Nonnull recorder, AVAudioPCMBuffer * _Nonnull buffer, uint64_t time) {
            @strongify(self);
            [self onAudioSampleBufferFrom:buffer atTime:time];
        };
        [self.audioRecorder start];
        
        
        self.videoRecorder = [[SCLiveRoomVideoRecorder alloc] init];
        self.videoRecorder.onVideoFrame = ^(CVPixelBufferRef  _Nonnull pixelBuffer, CGRect rect, CMTime prentationTime) {
            @strongify(self);
//            [self onVideoFrame:pixelBuffer frame:rect time:prentationTime];
        };
        self.semphore = dispatch_semaphore_create(1);
    }
}

- (void)pause {
    if (_recording) {
        _recording = NO;
    }
}

/**
 * 这个方法老出问题(死锁)， 不好定位问题， 加log定位问题
 */
- (void)finishedRecord:(BOOL)pausedManually {
    [self stopTimer];
    [self.audioRecorder finish];

    SCLog(@"[finishedRecord:(%@)], _recording:%@ ", @(pausedManually), @(_recording));
    if (_recording) {
        SCLog(@"finishedRecord step 0");
        self.videoRecorder = nil;
        
        SCLog(@"finishedRecord step 1");
        [self pause];
        SCLog(@"finishedRecord step 2");
        [self.videoFrameWriteQueue waitUntilAllOperationsAreFinished];
        SCLog(@"finishedRecord step 3");
        self.videoFrameWriteQueue = nil;
        SCLog(@"finishedRecord step 4");
        
        [_audioWriterInput markAsFinished];
        SCLog(@"finishedRecord step 5");

        [_assetWriterVideoInput markAsFinished];
        SCLog(@"finishedRecord step 6");

        CMTime elapsed = CMTimeMake((int)(([self getElapsed]) * 1000), 1000);
        [_assetWriter endSessionAtSourceTime:elapsed];
        SCLog(@"finishedRecord step 7");

        @weakify(self);
        [self.assetWriter finishWritingWithCompletionHandler:^{
            SCLog(@"finishedRecord step 8");

            @strongify(self);
            self.assetWriterVideoInput = nil;
            self.assetWriter = nil;
            self.wVideoAdaptor = nil;
            if ([self.delegate respondsToSelector:@selector(xScreenCaptureDidFinsished:)]) {
                [self.delegate xScreenCaptureDidFinsished:pausedManually];
            }
            SCLog(@"finishedRecord step 9");

        }];
        [self cleanUserPixelBufferPool];
        SCLog(@"finishedRecord step 10");

        SCLog(@"finishedRecord step 11");
    } else {
        SCLog(@"finishedRecord error _recording is false");
    }
}

- (void)cleanUserPixelBufferPool {
    CGColorSpaceRelease(_colorSpace);
    CVPixelBufferPoolRelease(_pixelBufferPool);
}

#pragma mark - timer
// 创建并启动一个子线程去维持一个 runloop，让 self.screenTimer 运行在此 runloop 中。
+ (NSThread *)screenRecordThread {
    static NSThread *_screenRecorderThread = nil;
    static dispatch_once_t oncePredicate;
    dispatch_once(&oncePredicate, ^{
        _screenRecorderThread = [[NSThread alloc] initWithTarget:self selector:@selector(videoRecordThread) object:nil];
        [_screenRecorderThread start];
    });
    
    return _screenRecorderThread;
}

+ (void)videoRecordThread {
    @autoreleasepool {
        [[NSThread currentThread] setName:@"com.haoqi.videorecord"];
        NSRunLoop *runloop = [NSRunLoop currentRunLoop];
        [runloop addPort:[NSMachPort port] forMode:NSRunLoopCommonModes];
        [runloop run];
    }
}


//偶尔出现 timer == nil 的情况， timer 会在主线程、和子线程 同时操作
- (void)atomicStopTimer {
    [self.timerLock lock];
    SCLog(@"atomicStopTimer");
    if ([self.screenTimer isValid]) {
        [self.screenTimer invalidate];
        self.screenTimer = nil;
    }
    [self.timerLock unlock];
}

- (void)atomicStartTimer {
    [self.timerLock lock];
    SCLog(@"atomicStartTimer");
    if (!self.screenTimer) {
        self.screenTimer = [NSTimer timerWithTimeInterval:1.0 / self.options.fps
                                                   target:self
                                                 selector:@selector(timerAction)
                                                 userInfo:nil
                                                  repeats:YES];
        
        //timer需要强引用自身， 因为外部释放过于及时，还没完成录制就释放掉了， 所以不要用下边注释的代码
//        @weakify(self);
//        self.screenTimer = [NSTimer timerWithTimeInterval:1.0 / self.options.fps block:^(NSTimer * _Nonnull timer) {
//            @strongify(self);
//
//            [self timerAction];
//        } repeats:true];
        if ([NSThread currentThread] == [SCLiveVideoRecorder screenRecordThread]) {
            SCLog(@"atomicStartTimer NSThread currentThread] == [SCScreenCapture screenRecordThread]");
            [self startTimerRecord:self.screenTimer];
        } else {
            [self performSelector:@selector(startTimerRecord:)
                         onThread:[SCLiveVideoRecorder screenRecordThread]
                       withObject:self.screenTimer
                    waitUntilDone:YES];
        }
    }
    [self.timerLock unlock];
}

- (void)startTimer {
    [self atomicStopTimer];
    [self atomicStartTimer];
}

- (void)timerAction {
    self.logSequence += 1;
    if (self.logSequence % (10 + self.options.fps * 20) == 0) {//健康状态log，每隔20s+ log
        SCLog(@"%p timerAction  %lld", self, self.logSequence);
    }
    
    if (self.recording == YES) {
        [self videoWriterTimerHandler:nil];
    }
    
    NSTimeInterval curTime = [[NSDate date] timeIntervalSince1970];
    if (self.lastFrameTime == 0) {
        self.lastFrameTime = curTime;
    } else if (curTime - self.lastFrameTime >= 1){
        self.timerCurCount ++;
        self.lastFrameTime = curTime;
        if (self.delegate && [self.delegate respondsToSelector:@selector(xScreenCaptureDidDuration:)]) {
            [self.delegate xScreenCaptureDidDuration:self.timerCurCount];
        }
    }
}

- (void)startTimerRecord:(NSTimer *)timer{
    [[NSRunLoop currentRunLoop] addTimer:self.screenTimer forMode:NSRunLoopCommonModes];
    NSLog(@"startTimerRecord time %lf", [NSDate date].timeIntervalSince1970);
    self.isCompleteFristFrame = false;

//    dispatch_async(dispatch_get_global_queue(0, 0), ^{
//        [self performSelector:@selector(timerAction) onThread:[SCScreenCapture screenRecordThread] withObject:nil waitUntilDone:false];
//    });
}

- (void)stopTimer {
    [self atomicStopTimer];
}

- (double)audioElapsed:(uint64_t)time {
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    
    uint64_t timeInterval = time - _startAtNumber;
    timeInterval *= info.numer;
    timeInterval /= info.denom;
    
    //纳秒转换为秒
    float duration = (timeInterval * 1.0f) / 1000000000;
    
    return duration;
}

- (double)getElapsed {
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    
    uint64_t timeInterval = mach_absolute_time() - _startAtNumber;
    
    timeInterval *= info.numer;
    timeInterval /= info.denom;
    
    //纳秒转换为秒
    float duration = (timeInterval * 1.0f) / 1000000000;
    
    return duration;
}

- (void)onAudioSampleBufferFrom:(AVAudioPCMBuffer *) pcmBuffer atTime:(uint64_t)time {
    if (!self.recording) {
        return;
    }
    AudioBufferList *audioBufferList = [pcmBuffer mutableAudioBufferList];
    AudioStreamBasicDescription asbd = *pcmBuffer.format.streamDescription;
    
    CMSampleBufferRef sampleBuffer = NULL;
    CMFormatDescriptionRef format = NULL;
    
    OSStatus error = CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &asbd, 0, NULL, 0, NULL, NULL, &format);
    if (error != noErr) {
        return;
    }
    CMTime elapsed = CMTimeMake((int)(([self audioElapsed:time]) * asbd.mSampleRate), asbd.mSampleRate);
    CMSampleTimingInfo timing = { CMTimeMake(1 / asbd.mChannelsPerFrame, asbd.mSampleRate), elapsed, kCMTimeInvalid };
    error = CMSampleBufferCreate(kCFAllocatorDefault,
                                 NULL, false, NULL, NULL, format,
                                 pcmBuffer.frameLength,
                                 1/*asbd.mChannelsPerFrame*/, &timing, 0, NULL, &sampleBuffer);
    if (error != noErr) {
        CFRelease(format);
        NSLog(@"CMSampleBufferCreate returned error: %d", (int)error);
        return;
    }
    
    error = CMSampleBufferSetDataBufferFromAudioBufferList(sampleBuffer, kCFAllocatorDefault, kCFAllocatorDefault, 0, audioBufferList);
    if (error != noErr) {
        CFRelease(format);
        NSLog(@"CMSampleBufferSetDataBufferFromAudioBufferList returned error: %d", (int)error);
        return;
    }
    
    @weakify(self);
    [self.videoFrameWriteQueue addOperationWithBlock:^{
        @strongify(self);
        
        BOOL result = [self.audioWriterInput appendSampleBuffer:sampleBuffer];
        if (result) {
        } else {
            SCLog(@"audioWriterInput appendSampleBuffer failure");
        }
        CFRelease(sampleBuffer);
    }];
    CFRelease(format);
}

//- (void)onVideoFrame:(CVPixelBufferRef)pixelBuffer frame:(CGRect)rect time:(CMTime)presentationTime {
//    if (self.videoRecordMode == 0) {
//        return;
//    }
//    if (self.recordFrameArray.count >= 3) {
//        return;
//    }
//    CFRetain(pixelBuffer);
//    SCVideoRecorderFrame *videoRecordFrame = [SCVideoRecorderFrame new];
//    UIImage *image = [self imageFromPixelBuffer:pixelBuffer];
//    videoRecordFrame.image = image;
//    videoRecordFrame.rect = rect;
//    videoRecordFrame.presentationTime = presentationTime;
//    if (pixelBuffer) {
//        CFRelease(pixelBuffer);
//    }
//
//    [self.recordFrameArray addObject:videoRecordFrame];
//}

//- (void)drawViewHierarchy:(UIImage *)image time:(CMTime)time{
//    if (!self.drawViewHierarchyQueue) {
//        return;
//    }
//    if (dispatch_semaphore_wait(self.semphore, DISPATCH_TIME_NOW)) {
//        return;
//    }
//
//    [self checkForeStatus];
//
//    @weakify(self);
//    [self.drawViewHierarchyQueue addOperationWithBlock:^{
//        @strongify(self);
//        UIImage *hierarchyImage = [self drawImageWithFore:self.recordFrameArray.firstObject Back:image];
//        CVPixelBufferRef pixelBuffer = [self creatPixelBufferWithImage:hierarchyImage];
//        [self writeVideoToFile:pixelBuffer time:time];
//        if (pixelBuffer) {
//            CFRelease(pixelBuffer);
//        }
//
//        if (self.recordFrameArray.count > 1) {
//            [self.recordFrameArray removeFirstObject];
//        }
//        dispatch_semaphore_signal(self.semphore);
//    }];
//}
//
//- (void)checkForeStatus {
//    if (!(self.videoRecorder.isOpenVideo)) {
//        [self.recordFrameArray removeAllObjects];
//    }
//}

- (UIImage *)drawImageWithFore:(SCVideoRecorderFrame *)frame Back:(UIImage *)image {
    UIImage *foreImage = frame.image;
    UIImage *backImage = image;
    CGRect rect = frame.rect;
    
    CGSize videoSize = [self videoSize];
    CGFloat scale = videoSize.width/ backImage.size.width;
    CGRect foreRect = CGRectMake(rect.origin.x * scale, rect.origin.y * scale, rect.size.width * scale, rect.size.height * scale);
    
    UIGraphicsBeginImageContext(videoSize);
    [backImage drawInRect:CGRectMake(0, 0, videoSize.width, videoSize.height)];
    if(!CGRectEqualToRect(foreRect, CGRectZero) || foreImage) {
        [foreImage drawInRect:foreRect];
    }
    
    UIImage *hiImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return hiImage;
}

- (UIImage *)imageFromPixelBuffer:(CVPixelBufferRef)pixelBuffer {
    CVImageBufferRef imageBuffer =  pixelBuffer;
    
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    size_t bufferSize = CVPixelBufferGetDataSize(imageBuffer);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    
    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, baseAddress, bufferSize, NULL);
    
    CGImageRef cgImage = CGImageCreate(width, height, 8, 32, bytesPerRow, rgbColorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst, provider, NULL, true, kCGRenderingIntentDefault);
    UIImage *image = [UIImage imageWithCGImage:cgImage];
    CGImageRelease(cgImage);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(rgbColorSpace);
    
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    
    UIImage *copyImage = [UIImage imageWithData:UIImagePNGRepresentation(image)];
    return copyImage;
}

- (CGContextRef)creatBitmapContextWithPixelBuffer:(CVPixelBufferRef *)pixelBuffer {
    CVPixelBufferPoolCreatePixelBuffer(NULL, _pixelBufferPool, pixelBuffer);
    CVPixelBufferLockBaseAddress(*pixelBuffer, 0);
    CGContextRef bitmapContext = NULL;
    CGSize _viewSize = [self videoSize];
    bitmapContext = CGBitmapContextCreate(CVPixelBufferGetBaseAddress(*pixelBuffer),
                                          CVPixelBufferGetWidth(*pixelBuffer),
                                          CVPixelBufferGetHeight(*pixelBuffer),
                                          8, CVPixelBufferGetBytesPerRow(*pixelBuffer), _colorSpace,
                                          kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst
                                          );
    CGAffineTransform flipVertical = CGAffineTransformMake(1, 0, 0, -1, 0, _viewSize.height);
    CGContextConcatCTM(bitmapContext, flipVertical);
    return bitmapContext;
}

- (CVPixelBufferRef)creatPixelBufferWithImage:(UIImage *)cusImage {
    CGImageRef spriteImage = cusImage.CGImage;
    
    
    CVPixelBufferRef pixelbuffer = NULL;
    
    CGSize videoSize = [self videoSize];
    size_t frameWidth = videoSize.width; //CGImageGetWidth(spriteImage) / cusImage.scale;
    size_t frameHeight = videoSize.height; ////CGImageGetHeight(spriteImage) / cusImage.scale;

    NSDictionary *options = @{
                              (NSString*)kCVPixelBufferCGImageCompatibilityKey : @YES,
                              (NSString*)kCVPixelBufferCGBitmapContextCompatibilityKey : @YES,
                              (NSString*)kCVPixelBufferIOSurfacePropertiesKey: [NSDictionary dictionary],
                              
                              };
    
    CVPixelBufferCreate(kCFAllocatorDefault, frameWidth, frameHeight,
                        kCVPixelFormatType_32BGRA,
                        (__bridge CFDictionaryRef) options,
                        &pixelbuffer);
    
    CVPixelBufferLockBaseAddress(pixelbuffer, 0);
    void *pxdata = CVPixelBufferGetBaseAddress(pixelbuffer);
    NSParameterAssert(pxdata != NULL);

    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGContextRef context = CGBitmapContextCreate(pxdata,
                                                 frameWidth,
                                                 frameHeight,
                                                 8,
                                                 CVPixelBufferGetBytesPerRow(pixelbuffer),
                                                 rgbColorSpace,
                                                 kCGBitmapByteOrder32Little|kCGImageAlphaNoneSkipFirst);
    NSParameterAssert(context);
    CGContextConcatCTM(context, CGAffineTransformIdentity);
    CGContextDrawImage(context, CGRectMake(0,
                                           0,
                                           frameWidth,
                                           frameHeight),
                       spriteImage);
    CGColorSpaceRelease(rgbColorSpace);
    CGContextRelease(context);
    
    CVPixelBufferUnlockBaseAddress(pixelbuffer, 0);
    
    return pixelbuffer;
}

- (void)videoWriterTimerHandler:(NSTimer *)timer
{
    if (_recording) {
        
        @autoreleasepool {
            CMTime elapsed = kCMTimeZero;
            if (self.isCompleteFristFrame) {
                elapsed = CMTimeMake((int)(([self getElapsed]) * 1000), 1000);
            } else {
                elapsed = CMTimeMake(0, 1000);
            }
            
            CGSize size = [self videoSize];
            __block CVPixelBufferRef pixelBuffer = nil;
            __block CGContextRef bitmapContext = nil;
            bitmapContext = [self creatBitmapContextWithPixelBuffer:&pixelBuffer];

            @try {
                UIGraphicsPushContext(bitmapContext);
                [self.captureView drawViewHierarchyInRect:CGRectMake(0, 0, size.width, size.height) afterScreenUpdates:NO];
                //NSLog(@"++++++++%f", [[NSDate date] timeIntervalSince1970] - lastTime);
                self.isCompleteFristFrame = true;
            } @catch (NSException *exception) {
                CGContextRef context = UIGraphicsGetCurrentContext();
                CGContextSetFillColorWithColor(context, [[UIColor blackColor] CGColor]);
                CGContextFillRect(context, [UIScreen mainScreen].bounds);
                SCLog(@"viewToImage - 截屏闪退Cache - %@", self.strLogRoom);
            } @finally {
                UIGraphicsPopContext();
                CGContextRelease(bitmapContext);
            }
                
            [self writeVideoToFile:pixelBuffer time:elapsed];
            if (pixelBuffer) {
                CFRelease(pixelBuffer);
            }
        }
    }
}

- (void)writeVideoToFile:(CVPixelBufferRef)pixelBuffer time:(CMTime)elapsed{
    if ([_assetWriterVideoInput isReadyForMoreMediaData] && _recording) {
        CFRetain(pixelBuffer);
        if (_wVideoAdaptor) {
            @try {
                @weakify(self);
                [self.videoFrameWriteQueue addOperationWithBlock:^{
                    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
                    @strongify(self);
                    if (![_wVideoAdaptor appendPixelBuffer:pixelBuffer withPresentationTime:elapsed]) {
                        SCLog(@"appendPixelBuffer - 插针Cache失败 %@", self.strLogRoom);
                        [self finishedRecord:NO];
                        if (pixelBuffer) {
                            CFRelease(pixelBuffer);
                        }
                        
                        return;
                    }
                    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
                    if (pixelBuffer) {
                        CFRelease(pixelBuffer);
                    }
                }];
            } @catch (NSException *exception) {
                SCLog(@"appendPixelBuffer - 插针Cache - %@ - %@", self.strLogRoom, exception);
                NSError *error = [_assetWriter error];
                NSString * errStr = [NSString stringWithFormat:@"failed to append sbuf: %@",error];
                SCLog(@"appendPixelBuffer - %@ %@", self.strLogRoom, errStr);
                [self finishedRecord:NO];
                if (pixelBuffer) {
                    CFRelease(pixelBuffer);
                }
                return;
            } @finally {

            }

            CGFloat progress = (CMTimeGetSeconds(elapsed) / self.duration);

            if (progress >= 1.0f) {
                [self finishedRecord:NO];
                return;
            } else {
                if ([self.delegate respondsToSelector:@selector(xScreenCaptureDidProgress:)]) {
                    [self.delegate xScreenCaptureDidProgress:(CMTimeGetSeconds(elapsed) / self.duration)];
                }
            }
        }
    }
}

//// #import "libyuv.h"
//static OSType KVideoPixelFormatType = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
//
//-(CVPixelBufferRef)yuvPixelBufferWithData:(NSData *)dataFrame
//                         presentationTime:(CMTime)presentationTime
//                                    width:(size_t)w
//                                   heigth:(size_t)h
//{
//    unsigned char* buffer = (unsigned char*) dataFrame.bytes;
//    CVPixelBufferRef getCroppedPixelBuffer = [self copyDataFromBuffer:buffer toYUVPixelBufferWithWidth:w Height:h];
//    return getCroppedPixelBuffer;
//}
//
//
//-(CVPixelBufferRef)copyDataFromBuffer:(const unsigned char*)buffer toYUVPixelBufferWithWidth:(size_t)w Height:(size_t)h
//{
////通过宏使模拟器能够通过编译
//#if TARGET_IPHONE_SIMULATOR
//    NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSDictionary dictionary],kCVPixelBufferOpenGLESTextureCacheCompatibilityKey,
//                                           nil];
//#else
//    NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSDictionary dictionary],kCVPixelBufferIOSurfaceOpenGLESTextureCompatibilityKey,
//                                           nil];
//#endif
//
//
//    CVPixelBufferRef pixelBuffer;
//    CVPixelBufferCreate(NULL, w, h, KVideoPixelFormatType, (__bridge CFDictionaryRef)(pixelBufferAttributes), &pixelBuffer);
//
//    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
//
//    size_t d = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
//    const unsigned char* src = buffer;
//    unsigned char* dst = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
//
//    for (unsigned int rIdx = 0; rIdx < h; ++rIdx, dst += d, src += w) {
//        memcpy(dst, src, w);
//    }
//
//    d = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
//    dst = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
//    h = h >> 1;
//    for (unsigned int rIdx = 0; rIdx < h; ++rIdx, dst += d, src += w) {
//        memcpy(dst, src, w);
//    }
//
//    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
//
//    return pixelBuffer;
//
//}
//
//
//- (CVPixelBufferRef) newPixelBufferFromCGImage: (CGImageRef) image
//{
//    CVPixelBufferRef pxbuffer = NULL;
//
//    CFDictionaryRef option = (__bridge_retained CFDictionaryRef)_videoSettings;
//
//    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault,
//                                          self.options.videoWidth,
//                                          self.options.videoHeight,
//                                          kCVPixelFormatType_32ARGB,
//                                          option,
//                                          &pxbuffer
//                                          );
//
//    CFRelease(option);
//
//    NSParameterAssert(status == kCVReturnSuccess && pxbuffer != NULL);
//
//    CVPixelBufferLockBaseAddress(pxbuffer, 0);
//    void *pxdata = CVPixelBufferGetBaseAddress(pxbuffer);
//
//    NSParameterAssert(pxdata != NULL);
//
//    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
//
//#if __IPHONE_OS_VERSION_MAX_ALLOWED > __IPHONE_6_1
//    int bitmapInfo = (CGBitmapInfo)kCGImageAlphaNoneSkipFirst;
//#else
//    int bitmapInfo = kCGImageAlphaNoneSkipFirst;
//#endif
//
//    CGContextRef context = CGBitmapContextCreate(pxdata,
//                                                 self.options.videoWidth,
//                                                 self.options.videoHeight,
//                                                 8,
//                                                 4*self.options.videoWidth,
//                                                 rgbColorSpace,
//                                                 bitmapInfo
//                                                 );
//
//    //    CGContextRef context = CGBitmapContextCreate(pxdata, self.size.width, self.size.height, 8, 4*self.size.width, rgbColorSpace,kCGImageAlphaNoneSkipFirst);
//
//    NSParameterAssert(context);
//
//    //    CGFloat iw = CGImageGetWidth(image);
//    CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
//    CGContextFillRect(context, CGRectMake(0, 0, self.options.videoWidth, self.options.videoHeight));
//
//    CGContextDrawImage(context, CGRectMake(0, 0, self.options.videoWidth, self.options.videoHeight), image);
//
//    CGColorSpaceRelease(rgbColorSpace);
//    CGContextRelease(context);
//
//    CVPixelBufferUnlockBaseAddress(pxbuffer, 0);
//
//    return pxbuffer;
//}
//
//
//- (UIImage *)viewToImage:(UIView *)view
//{
//    if (self.uploadType == SCUploadTypePlayback) {
//        UIGraphicsBeginImageContextWithOptions(CGSizeMake([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height), NO, 1.0);
//    } else {
//        UIGraphicsBeginImageContextWithOptions(CGSizeMake([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height), NO,[UIScreen mainScreen].scale);
//    }
//    CGContextSetInterpolationQuality(UIGraphicsGetCurrentContext(), kCGInterpolationNone);
//    @try {
//        [view drawViewHierarchyInRect:CGRectMake(0, 0,[UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height) afterScreenUpdates:NO];
//    } @catch (NSException *exception) {
//        CGContextRef context = UIGraphicsGetCurrentContext();
//        CGContextSetFillColorWithColor(context, [UIColorHex(0xFFFFFF) CGColor]);
//        CGContextFillRect(context, [UIScreen mainScreen].bounds);
//        SCLog(@"viewToImage - 截屏闪退Cache - %@", self.strLogRoom);
//    } @finally {
//
//    }
//    UIImage * image = UIGraphicsGetImageFromCurrentImageContext();
//    UIGraphicsEndImageContext();
//    return image;
//}


@end
