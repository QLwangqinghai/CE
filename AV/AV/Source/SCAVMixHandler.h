//
//  SCAVMixHandler.h
//  AV
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SCAVWaterMask : NSObject
@property (nonatomic, assign) CGRect frame;
@property (nonatomic, strong) UIImage * image;
@end


@interface SCAVMixConfig : NSObject

/*
 user_course_schedule_time
 */
@property (nonatomic, copy, readonly) NSString * identifier;
@property (nonatomic, copy) NSString * outputPath;

@property (nonatomic, strong, nullable, readonly) AVAsset * audioAsset;
@property (nonatomic, strong, nullable, readonly) AVAsset * videoAsset;

@property (nonatomic, assign) CGSize outputVideoSize;//videoAsset 非空时有效， 宽高都>=SCAVMixSizeMin时有效
@property (nonatomic, assign) CGFloat outputVideoFrameRate;//videoAsset 非空时有效，

@property (nonatomic, copy, readonly) NSArray<SCAVWaterMask *> * waterMasks;

//video input 需要是有效的
- (instancetype)initWithIdentifier:(NSString *)identifier videoInputPath:(NSString *)videoInputPath audioInputPath:(nullable NSString *)audioInputPath;
- (instancetype)initWithIdentifier:(NSString *)identifier videoAsset:(AVAsset *)videoAsset audioAsset:(nullable AVAsset *)audioAsset;

- (void)addWaterMask:(SCAVWaterMask *)waterMask;

@end


typedef NS_ENUM(NSUInteger, SCAVMixHandlerStatus) {
    SCAVMixHandlerStatusNone = 0,
    SCAVMixHandlerStatusRunning,
    SCAVMixHandlerStatusFinished,
    SCAVMixHandlerStatusFailure,
    SCAVMixHandlerStatusCanceled,
};

@class SCAVMixHandler;
typedef void(^SCAVMixHandlerBlock)(SCAVMixHandler * handler);

@interface SCAVMixHandler : NSObject

@property (nonatomic, assign, readonly) SCAVMixHandlerStatus status;
@property (nonatomic, strong) SCAVMixConfig * config;

- (instancetype)initWithConfig:(SCAVMixConfig *)config queue:(dispatch_queue_t)queue;

- (BOOL)startWithCompletion:(SCAVMixHandlerBlock)completion;
- (void)cancel;

@end



NS_ASSUME_NONNULL_END
