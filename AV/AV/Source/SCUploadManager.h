//
//  SCUploadManager.h
//  AV
//
//  Created by vector on 2019/7/18.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/*
 资源预处理
 
 
 资源真正上传
 */

typedef NS_ENUM(NSUInteger, SCUploadItemType) {
    SCUploadItemTypeUnknown = 0,
    SCUploadItemTypeImage = 1,//值不允许变化
    SCUploadItemTypeVideo = 2,//值不允许变化
    SCUploadItemTypePlaybackVideo = 3,//值不允许变化
};

typedef NS_ENUM(NSUInteger, SCUploadItemStatus) {
    SCUploadItemStatusNormal = 0,
    SCUploadItemStatusPreparing = 1,//值不允许变化
    SCUploadItemStatusPrepared = 2,//值不允许变化
    SCUploadItemStatusGoing = 3,//值不允许变化
    SCUploadItemStatusFailure = 4,//值不允许变化
    SCUploadItemStatusSuccess = 5,//值不允许变化
};

@interface SCUploadItem : NSObject

@property (nonatomic, assign, readonly) BOOL prepared;

@property (nonatomic, assign, readonly) SCUploadItemType type;

//本地文件路径
@property (nonatomic, copy, readonly) NSString * path;

//压缩处理后文件的输出位置，不一定会100% 做压缩操作
@property (nonatomic, copy, readonly) NSString * outputPath;

/*
 上传前压缩标识，只会尝试去压缩
 视频：会把>720p的视频转码为720p
 图片：图片>500KB的进行二次压缩
 */
@property (nonatomic, assign, readonly) BOOL needToCompress;

//上传成功后对应的服务器url地址
@property (nonatomic, copy) NSString * urlString;

//状态kvo
@property (nonatomic, assign) SCUploadItemStatus status;


@property (nonatomic, assign) NSInteger retryTimes;





@end

/*
 包含2部分， 
 */

@interface SCUploadManager : NSObject

//监听用户状态

//监听网络状态

//监听视频队列状态

//监听直播状态




@end

NS_ASSUME_NONNULL_END
