//
//  SCAVMixManager.h
//  AV
//
//  Created by vector on 2019/7/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN


//@interface SCAVMixWorkItem : NSObject
//
//@property (nonatomic, readonly) NSString * identifier;
//
//@end


@interface SCAVMixManager : NSObject



    
+ (instancetype)shared;

//工作队列
+ (dispatch_queue_t)queue;

@end

NS_ASSUME_NONNULL_END
