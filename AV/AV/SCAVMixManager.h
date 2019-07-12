//
//  SCAVMixManager.h
//  AV
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SCAVMixHandler.h"


NS_ASSUME_NONNULL_BEGIN

extern NSNotificationName const SCAVMixManagerItemUpdateNotification;


/*
 tmp/UUID 临时工作区间 => 结束后， 移动文件到目标地方， 移动输入的音频视频文件到回收站
 */
@interface SCAVMixManager : NSObject

+ (void)resume;
+ (void)suspend;


+ (void)appendWorkItem:(id<SCAVMixWorkItemProtocol>)item;






+ (NSString *)identifierWithUid:(NSString *)uid courseId:(NSString *)courseId scheduleId:(NSString *)scheduleId item:(NSString *)item;

//+ (NSString *)cachePathForIdentifier:(NSString *)identifier;


+ (NSString *)tmpDirectoryPathForUuid:(NSUUID *)uuid;
+ (NSString *)tmpPathForIdentifier:(NSString *)identifier uuid:(NSUUID *)uuid;

@end

NS_ASSUME_NONNULL_END
