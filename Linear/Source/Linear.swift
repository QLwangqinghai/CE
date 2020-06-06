//
//  Linear.swift
//  Linear
//
//  Created by vector on 2020/6/6.
//  Copyright © 2020 angfung. All rights reserved.
//

import UIKit


open class BaseLinearView: UIView {

}

@interface SCBaseLinearView : UIView <SCLinearProtocol>

//快速高度监听， 用于更新约束
@property (nonatomic, copy, nullable) void(^onOverallSizeChanged)(id<SCLinearProtocol> section);

//kvo
@property (nonatomic, assign, readonly) CGFloat overallSize;

//kvo
@property (nonatomic, assign) CGFloat bodySize;

//kvo
@property (nonatomic, assign) CGFloat headerSize;

//kvo
@property (nonatomic, assign) CGFloat footerSize;

//默认-1 < -0.01时 标识高度跟不隐藏时一样
@property (nonatomic, assign) CGFloat overallSizeWhenDisable;

@property (nonatomic, assign) BOOL isDisableInLinear;


- (void)didOverallSizeChangedFrom:(CGFloat)old to:(CGFloat)to;

- (void)layoutSubitems;


- (void)didHeaderSizeChanged;
- (void)didBodySizeChanged;
- (void)didFooterSizeChanged;



@end



class Linear: NSObject {

}
