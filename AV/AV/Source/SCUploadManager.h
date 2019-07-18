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


@interface SCUploadItem : NSObject

@property (nonatomic, assign, readonly) BOOL prepared;



@end



@interface SCUploadManager : NSObject

@end

NS_ASSUME_NONNULL_END
