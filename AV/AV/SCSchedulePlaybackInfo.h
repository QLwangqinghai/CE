//
//  SCSchedulePlaybackInfo.h
//  AV
//
//  Created by vector on 2019/7/11.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, SCSchedulePlaybackElementStatus) {
    SCSchedulePlaybackElementStatusNormal = 0,
    SCSchedulePlaybackElementStatusUploadSuccess = 1,
    SCSchedulePlaybackElementStatusMixErrored = 2,
};


@interface SCSchedulePlaybackElement : NSObject

// uid_courseId_scheduleId_time
@property (nonatomic, copy) NSString * identifier;

// 时间单位毫秒
@property (nonatomic, assign) int64_t createTime;


@property (nonatomic, copy, nullable) NSString * audioFilePath;
@property (nonatomic, copy, nullable) NSString * videoFilePath;

@property (nonatomic, copy, nullable) NSString * outputFilePath;

@property (nonatomic, assign) SCSchedulePlaybackElementStatus status;


- (BOOL)healthCheck;

@end


//typedef NS_ENUM(NSUInteger, SCSchedulePlaybackStatus) {
//    SCSchedulePlaybackElementStatusNormal = 0,
//    SCSchedulePlaybackElementStatusUploadSuccess = 1,
//    SCSchedulePlaybackElementStatusMixErrored = 2,
//};

@interface SCSchedulePlaybackInfo : NSObject

@end

NS_ASSUME_NONNULL_END
