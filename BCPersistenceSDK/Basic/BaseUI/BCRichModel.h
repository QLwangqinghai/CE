//
//  BCRichModel.h
//  BCUIKit
//
//  Created by 王青海 on 2018/8/2.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import <YYText/YYText.h>

//https://github.com/ibireme/YYText

/*
 富文本model
 */
@interface BCRichModel : NSObject

@property (nonatomic, copy) NSString *stringForCopyed;
@property (nonatomic, copy) NSAttributedString *attributedString;
@property (nonatomic, strong) YYTextLayout *layout;
@property (nonatomic, assign) CGSize size;


@end
