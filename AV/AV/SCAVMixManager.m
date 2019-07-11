//
//  SCAVMixManager.m
//  AV
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "SCAVMixManager.h"

NSNotificationName const SCSCAVMixManagerItemUpdateNotification = @"SCSCAVMixManagerItemUpdateNotification";

@interface SCAVMixManager ()


@property (nonatomic, assign) BOOL enable;

@property (nonatomic, copy, readonly) NSString * tmpDirectoryPath;
//@property (nonatomic, copy, readonly) NSString * cacheDirectoryPath;

@property (nonatomic, strong) NSMutableArray<id<SCAVMixWorkItemProtocol>> * items;

@property (nonatomic, strong) NSMutableArray<id<SCAVMixWorkItemProtocol>> * runningItems;

@property (nonatomic, strong) id<SCAVMixWorkItemProtocol> runningItem;


@end
@implementation SCAVMixManager

+ (void)async:(void(^)(SCAVMixManager * manager))block {
    dispatch_async([self queue], ^{
        if (block) {
            block([SCAVMixManager shared]);
        }
    });
}

- (void)resume {
    self.enable = true;
    if (self.runningItem) {
        [self.runningItem resume];
    } else {
        [self next];
    }
}

- (void)next {
    if (nil != self.runningItem) {
        NSLog(@"next error, one is running");
    }
    if (!self.enable) {
        return;
    }
    NSLog(@"go next");
    
    [self.items enumerateObjectsUsingBlock:^(id<SCAVMixWorkItemProtocol>  _Nonnull item, NSUInteger idx, BOOL * _Nonnull stop) {
        BOOL startResult = [item startWithQueue:[SCAVMixManager queue] completion:^(id<SCAVMixWorkItemProtocol> _Nonnull citem) {
            if (self.runningItem == citem) {
                self.runningItem = nil;
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    [[NSNotificationCenter defaultCenter] postNotificationName:SCSCAVMixManagerItemUpdateNotification object:self];
                });
                [self next];
            } else {
            }
        }];
        self.runningItem = item;
        if (startResult) {
            NSLog(@"start item success");
        } else {
            NSLog(@"start failure");
        }
    }];
    [self.runningItems addObjectsFromArray:self.items];
    [self.items removeAllObjects];
}

//- (void)next {
//    if (nil != self.runningItem) {
//        NSLog(@"next error, one is running");
//    }
//    if (!self.enable) {
//        return;
//    }
//    NSLog(@"go next");
//
//
//    id<SCAVMixWorkItemProtocol> item = self.items.firstObject;
//    if (item) {
//        [self.items removeObjectAtIndex:0];
//        BOOL startResult = [item startWithQueue:[SCAVMixManager queue] completion:^(id<SCAVMixWorkItemProtocol> _Nonnull citem) {
//            if (self.runningItem == citem) {
//                self.runningItem = nil;
//
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    [[NSNotificationCenter defaultCenter] postNotificationName:SCSCAVMixManagerItemUpdateNotification object:self];
//                });
//                [self next];
//            } else {
//                NSLog(@"completion error");
//            }
//        }];
//        self.runningItem = item;
//        if (startResult) {
//            NSLog(@"start item success");
//        } else {
//            NSLog(@"start failure");
//        }
//    } else {
//        NSLog(@"没有更多的待转码数据了");
//    }
//
//}

- (void)suspend {
    self.enable = false;
    [self.runningItem suspend];
}
- (void)appendWorkItem:(id<SCAVMixWorkItemProtocol>)item {
    [self.items addObject:item];
    if (self.enable && nil == self.runningItem) {
        [self next];
    }
}


+ (void)resume {
    [self async:^(SCAVMixManager *manager) {
        [manager resume];
    }];
}
+ (void)suspend {
    [self async:^(SCAVMixManager *manager) {
        [manager suspend];
    }];
}
+ (void)appendWorkItem:(id<SCAVMixWorkItemProtocol>)item {
    [self async:^(SCAVMixManager *manager) {
        [manager appendWorkItem:item];
    }];
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _items = [NSMutableArray array];
        _runningItems = [NSMutableArray array];
//        _cacheDirectoryPath = [cachePath stringByAppendingPathComponent:@"course_av_mix"];
        _tmpDirectoryPath = [NSTemporaryDirectory() stringByAppendingPathComponent:@"course_av_mix"];

//        [[NSFileManager defaultManager] createDirectoryAtPath:_cacheDirectoryPath withIntermediateDirectories:true attributes:NULL error:NULL];
        [[NSFileManager defaultManager] createDirectoryAtPath:_tmpDirectoryPath withIntermediateDirectories:true attributes:NULL error:NULL];
    }
    return self;
}

+ (instancetype)shared {
    static SCAVMixManager * __shared;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __shared = [[SCAVMixManager alloc] init];
    });
    return __shared;
}

+ (dispatch_queue_t)queue {
    static dispatch_queue_t __queue;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __queue = dispatch_queue_create("com.hq.mix", NULL);
    });
    return __queue;
}


+ (NSString *)identifierWithUid:(NSString *)uid courseId:(NSString *)courseId scheduleId:(NSString *)scheduleId item:(NSString *)item {
    return [NSString stringWithFormat:@"%@_%@_%@_%@", uid ?: @"", courseId ?: @"", scheduleId ?: @"", item ?: @""];
}

//+ (NSString *)cachePathForIdentifier:(NSString *)identifier {
//    return [[SCAVMixManager shared].cacheDirectoryPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.mp4", identifier]];
//}

+ (NSString *)tmpPathForIdentifier:(NSString *)identifier uuid:(NSUUID *)uuid {
    return [[SCAVMixManager shared].tmpDirectoryPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@/%@.mp4", uuid.UUIDString, identifier]];
}

+ (NSString *)tmpDirectoryPathForUuid:(NSUUID *)uuid {
    return [[SCAVMixManager shared].tmpDirectoryPath stringByAppendingPathComponent:uuid.UUIDString];
}


@end
