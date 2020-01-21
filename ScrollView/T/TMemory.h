//
//  Memory.h
//  T
//
//  Created by vector on 2020/1/12.
//  Copyright © 2020 angfung. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface TMemory : NSObject

- (instancetype)initWithSizeInMb:(NSInteger)sizeInMb;

- (void)test;

- (void)testMemcpy;

@end

NS_ASSUME_NONNULL_END
