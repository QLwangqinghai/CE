//
//  SCAVMixHandler.h
//  AV
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, SCAVMixHandlerStatus) {
    SCAVMixHandlerStatusNone,
    SCAVMixHandlerStatusRunning,
    SCAVMixHandlerStatusFinished,
    SCAVMixHandlerStatusFailure,
    SCAVMixHandlerStatusCanceled,
};

@interface SCAVMixConfig : NSObject

@property (nonatomic, copy) NSString * outputPath;
@property (nonatomic, copy) NSString * audioInputPath;
@property (nonatomic, copy) NSString * videoInputPath;

@end
@implementation SCAVMixConfig

@end


@interface SCAVMixHandler : NSObject

@property (nonatomic, copy) NSString * name;
@property (nonatomic, strong) SCAVMixConfig * config;




@end

NS_ASSUME_NONNULL_END
