//
//  SCAVMixWorkItem.h
//  AV
//
//  Created by vector on 2019/7/11.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, SCAVMixWorkItemStatus) {
    SCAVMixWorkItemStatusNone,
    SCAVMixWorkItemStatusSuspend,
    SCAVMixWorkItemStatusResume,
    SCAVMixWorkItemStatusFinished,
    SCAVMixWorkItemStatusCanceled,
};


@protocol SCAVMixWorkItemProtocol;
@class SCAVMixConfig;

typedef void(^SCAVMixWorkItemBlock)(id<SCAVMixWorkItemProtocol> item);

@protocol SCAVMixWorkItemProtocol <NSObject>

@property (nonatomic, readonly) NSString * identifier;//标识任务的唯一性
@property (atomic, readonly) SCAVMixWorkItemStatus status;


//cancel 或 正常结束都会执行completion，, 是由manager设定的
- (BOOL)startWithQueue:(dispatch_queue_t)queue completion:(SCAVMixWorkItemBlock)completion;

- (void)resume;
- (void)suspend;
- (void)cancel;

@end



typedef NS_ENUM(NSUInteger, SCAVMixWorkItemResult) {
    SCAVMixWorkItemResultNone,
    SCAVMixWorkItemResultSuccess,
    SCAVMixWorkItemResultFailure,
};

@class SCAVMixWorkItem;
typedef void(^SCAVMixWorkItemResultBlock)(id<SCAVMixWorkItemProtocol> item, SCAVMixWorkItemResult result);

@interface SCAVMixWorkItem : NSObject <SCAVMixWorkItemProtocol>

@property (nonatomic, readonly) NSString * identifier;
@property (atomic, readonly) SCAVMixWorkItemStatus status;

@property (nonatomic, readonly) SCAVMixWorkItemResult result;
@property (nonatomic, readonly) SCAVMixConfig * config;

- (instancetype)initWithConfig:(SCAVMixConfig *)config onResult:(SCAVMixWorkItemResultBlock)onResult;

//cancel 或 正常结束都会执行completion, 是由manager设定的
- (BOOL)startWithQueue:(dispatch_queue_t)queue completion:(SCAVMixWorkItemBlock)completion;

- (void)resume;
- (void)suspend;
- (void)cancel;


@end

/*
 
 任务执行 resume
 任务被挂起 suspend
 任务被取消 cancel

 finished
 
 
 任务结束 、失败

 
 */

NS_ASSUME_NONNULL_END
