//
//  SCAVMixHandler.h
//  AV
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "SCAVMixWorkItem.h"

NS_ASSUME_NONNULL_BEGIN



@interface SCAVMixConfig : NSObject

/*
 user_course_schedule_time
 */
@property (nonatomic, readonly) NSString * identifier;
@property (nonatomic, copy) NSString * outputPath;
@property (nonatomic, copy) NSString * audioInputPath;
@property (nonatomic, copy) NSString * videoInputPath;


- (instancetype)initWithIdentifier:(NSString *)identifier;

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
