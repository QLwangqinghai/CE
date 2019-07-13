//
//  SCRecoverManager.h
//  AV
//
//  Created by vector on 2019/7/12.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * 回收站
 */
@interface SCRecoverManager : NSObject

//回收 课程回放的原始音视频文件
- (void)recoverPlaybackOriginalFileAtPath:(NSString *)path rename:(nullable NSString *)fileName;

//回收 课程回放的转码后的
- (void)recoverPlaybackGeneratedVideoAtPath:(NSString *)path rename:(nullable NSString *)fileName;


+ (instancetype)shared;

@end

NS_ASSUME_NONNULL_END
