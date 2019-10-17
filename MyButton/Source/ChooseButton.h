//
//  ChooseButton.h
//  MyButton
//
//  Created by vector on 2019/10/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "CustomButton.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, ChooseStatus) {
    ChooseStatusNormal,//未被选中， 允许被选中
    ChooseStatusChoosed,//被选中， 允许被取消
    ChooseStatusNormalAndDisable,//未被选中， 不允许被选中， 不响应事件
    ChooseStatusChoosedAndDisable,//被选中， 不允许被取消， 不响应事件
};

@interface ChooseButton : CustomButton

@property (nonatomic, assign) ChooseStatus chooseStatus;
@property (nonatomic, copy, nullable) void(^onValidClick)(ChooseButton * button, BOOL toChoose);

- (void)setImage:(nullable UIImage *)image forChooseState:(ChooseStatus)chooseStatus;

@end

NS_ASSUME_NONNULL_END
