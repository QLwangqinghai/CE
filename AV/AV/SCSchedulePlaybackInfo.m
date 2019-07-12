//
//  SCSchedulePlaybackInfo.m
//  AV
//
//  Created by vector on 2019/7/11.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "SCSchedulePlaybackInfo.h"

NSString * const SCPlaybackAudioDirectoryName = @"audio";
NSString * const SCPlaybackVideoDirectoryName = @"video";
NSString * const SCPlaybackOutputDirectoryName = @"output";




@interface SCSchedulePlaybackElement ()

//@property (nonatomic, copy) NSString * identifier;

@end
@implementation SCSchedulePlaybackElement




@end



@implementation SCSchedulePlaybackInfo

+ (NSArray<SCSchedulePlaybackInfo *> *)playbackInfoWithUid:(NSString *)uid directoryPath:(NSString *)directoryPath {
    NSMutableArray<SCSchedulePlaybackInfo *> * result = [NSMutableArray array];
    if (directoryPath.length <= 0) {
        return result;
    }
    NSFileManager * fileManager = [NSFileManager defaultManager];
    NSError * error;
    NSArray<NSString *> * courseList = [fileManager contentsOfDirectoryAtPath:directoryPath error:&error];
    if (error) {
        NSLog(@"[fileManager contentsOfDirectoryAtPath:%@] error:%@", directoryPath, error);
    }
    
    [courseList enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        // 排除 .xxx
        if([obj hasPrefix:@"."]) {
            return;
        }
        
        NSArray<NSString *> * components = [obj componentsSeparatedByString:@"_"];
        if (components.count != 2) {
            return;
        }
        
        NSString * courseId = components[0];
        NSString * scheduleId = components[1];

        if (courseId.length <= 0 || scheduleId.length <= 0) {
            NSLog(@"courseId(%@), scheduleId(%@) error!", courseId, scheduleId);
            return;
        }
        
        
        NSString * audioDirectoryPath = [[directoryPath stringByAppendingPathComponent:obj] stringByAppendingPathComponent:SCPlaybackAudioDirectoryName];
        NSString * videoDirectoryPath = [[directoryPath stringByAppendingPathComponent:obj] stringByAppendingPathComponent:SCPlaybackVideoDirectoryName];

        NSError * error1;
        NSArray<NSString *> * contents = [fileManager contentsOfDirectoryAtPath:videoDirectoryPath error:&error1];
        if (error1) {
            NSLog(@"[fileManager contentsOfDirectoryAtPath:%@] error:%@", videoDirectoryPath, error1);
        }
        //视频为主要维度， 不判断音频文件存在不存在

        NSMutableSet<NSString *> * videoSet = [NSMutableSet set];
        if (contents) {
            [videoSet addObjectsFromArray:contents];
        }
        
//
//        if (![SCFileHelper dirExistsAtPath:coursePath]) {
//            return;
//        }
//
//        NSArray *components = [obj componentsSeparatedByString:@"_"];
//        if (components.count != 2) {
//            return;
//        }
//        NSInteger courseId = [components[0] integerValue];
//        NSInteger courseScheduleId = [components[1] integerValue];
//        if (courseScheduleId <= 0) {
//            return;
//        }
//
//        NSArray<SCVideoUploadModel *> *videos = [self videosWithCourseId:courseId courseScheduleId:courseScheduleId uploadType:uploadType];
//        SCLog(@"video count : %ld", videos.count);
//        NSArray<SCImageUploadModel *> *images = [self imagesWithCourseId:courseId courseScheduleId:courseScheduleId uploadType:uploadType];
//        SCLog(@"image count : %ld", images.count);
//
//        if (videos.count > 0) {// 只要有视频文件，就认为是可上传的
//            SCCourseUploadModel *upload = [[SCCourseUploadModel alloc] initWithCourseId:courseId courseScheduleId:courseScheduleId];
//            upload.videos = [videos mutableCopy];
//            upload.images = [images mutableCopy];
//            upload.type = uploadType;
//
//            __block NSInteger encodedCount = 0;
//            __block NSInteger uploadedCount = 0;
//            [videos enumerateObjectsUsingBlock:^(SCVideoUploadModel * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
//                if (obj.encoded) {
//                    encodedCount += 1;
//                };
//                if (obj.uploaded) {
//                    uploadedCount += 1;
//                }
//            }];
//
//            [images enumerateObjectsUsingBlock:^(SCImageUploadModel * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
//                if (obj.uploaded) {
//                    uploadedCount += 1;
//                }
//            }];
//
//            // encodeCount
//            upload.encodeCount = upload.videos.count;
//
//            // encodedCount
//            upload.encodedCount = encodedCount;
//
//            // uploadCount
//            upload.uploadCount = videos.count + images.count;
//
//            // uploadedCount
//            upload.uploadedCount = uploadedCount;
//
//            // 判断 state
//            upload.state = [self uploadStateWithUploadModel:upload];
//            if (upload.state != SCUploadStateDone) {
//                [result addObject:upload];
//            } else {
//                // 删除
//                [self removeCourseUploadFilesWithUploadModel:upload];
//            }
//        }
        NSLog(@"result count : %ld", result.count);
    }];
    
    return result;
}





@end
