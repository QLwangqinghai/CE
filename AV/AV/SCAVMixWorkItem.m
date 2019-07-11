//
//  SCAVMixWorkItem.m
//  AV
//
//  Created by vector on 2019/7/11.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "SCAVMixWorkItem.h"
#import "SCAVMixHandler.h"


@interface SCAVMixWorkItem ()
{
    NSString * _identifier;
}

@property (nonatomic, copy) SCAVMixWorkItemBlock completion;
@property (nonatomic, strong) dispatch_queue_t queue;




@property (nonatomic, copy) SCAVMixWorkItemResultBlock onResult;

@property (atomic, assign) SCAVMixWorkItemStatus status;
@property (nonatomic, assign) SCAVMixWorkItemResult result;

@property (nonatomic, strong) SCAVMixHandler * handler;


@end
@implementation SCAVMixWorkItem

- (NSString *)identifier {
    return self.config.identifier;
}

- (instancetype)initWithConfig:(SCAVMixConfig *)config onResult:(SCAVMixWorkItemResultBlock)onResult {
    self = [super init];
    if (self) {
        _config = config;
        self.onResult = onResult;
    }
    return self;
}

- (BOOL)startWithQueue:(dispatch_queue_t)queue completion:(SCAVMixWorkItemBlock)completion {
    if (SCAVMixWorkItemStatusNone == self.status) {
        self.queue = queue;
        self.completion = completion;
        
        
        //查看是否真的需要转码， 判断目标文件存在不存在、文件大小
        NSString * filePath = self.config.outputPath;
        NSFileManager * fm = [NSFileManager defaultManager];
        NSInteger fileSize = 0;
        BOOL isDirectory = false;
        if ([fm fileExistsAtPath:filePath isDirectory:&isDirectory]) {
            if (isDirectory) {
                [fm removeItemAtPath:filePath error:NULL];
            } else {
                NSDictionary * attributesOfItem = [fm attributesOfItemAtPath:filePath error:nil];
                id obj = attributesOfItem[NSFileSize];
                if ([obj isKindOfClass:[NSNumber class]]) {
                    fileSize = [obj integerValue];
                } else if ([obj isKindOfClass:[NSString class]]) {
                    fileSize = [obj integerValue];
                }
                
                if (fileSize <= 0) {
                    [fm removeItemAtPath:filePath error:NULL];
                }
            }
        }
        
        if (fileSize > 0) {
            NSLog(@"不需要转码 文件已经存在 %@", filePath);
            
            self.status = SCAVMixWorkItemStatusFinished;
            self.result = SCAVMixWorkItemResultSuccess;
            dispatch_async(queue, ^{
                [self complete];
            });
            return true;
        }
        //初始化handler
        self.status = SCAVMixWorkItemStatusResume;
        
        [self run];
        return true;
    }
    return false;
}
- (void)resume {
    [self run];
}
- (void)suspend {
    [self stop];
    
}
- (void)cancel {
    [self stop];
    [self complete];
}

- (void)complete {
    if (self.onResult) {
        self.onResult(self, self.result);
    }
    if (self.completion) {
        self.completion(self);
    }
}

- (void)runWithRetryDuration:(NSInteger)duration times:(NSInteger)times {
    SCAVMixHandler * handler = [[SCAVMixHandler alloc] initWithConfig:self.config queue:self.queue];
    __weak SCAVMixWorkItem * weakSelf = self;
    BOOL startResult = [handler startWithCompletion:^(SCAVMixHandler * _Nonnull handler) {
        __strong SCAVMixWorkItem * strongSelf = weakSelf;
        if (nil == strongSelf) {
            return;
        }
        if (strongSelf.handler != handler) {
            return;
        }
        if (SCAVMixWorkItemStatusCanceled != strongSelf.status) {
            //成功执行完成
            strongSelf.status = SCAVMixWorkItemStatusFinished;

            if (SCAVMixHandlerStatusFinished == handler.status) {
                //成功回调
                self.result = SCAVMixWorkItemResultSuccess;
            } else if (SCAVMixHandlerStatusFailure == handler.status) {
                self.result = SCAVMixWorkItemResultFailure;
            }
            [self complete];
        }
    }];
    if (startResult) {
        self.handler = handler;
    } else {
        if (times > 5) {//重试次数超过5次
            NSLog(@"重试次数超过5次, 任务失败：%@", self.config.identifier);
            
        } else {
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(duration * NSEC_PER_SEC)), self.queue, ^{
                __strong SCAVMixWorkItem * strongSelf = weakSelf;
                if (nil == strongSelf) {
                    return;
                }
                [strongSelf runWithRetryDuration:duration * 2 times:times + 1];
            });
        }
    }
}

- (void)run {
    [self runWithRetryDuration:4 times:1];
}
- (void)stop {
    
    
}

@end
