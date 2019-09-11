//
//  SCBasePopOverViewController.m
//  SC_Lecture
//
//  Created by vector on 2019/9/11.
//  Copyright © 2019 Shituo Han. All rights reserved.
//

#import "SCBasePopOverViewController.h"
#import "SCCustomMaskView.h"

typedef NS_ENUM(NSUInteger, SCPopOverContentBubbleStyle) {
    SCPopOverContentBubbleStyleTop = 0,
    SCPopOverContentBubbleStyleLeft = 1,
    SCPopOverContentBubbleStyleBottom = 2,
    SCPopOverContentBubbleStyleRight = 3,
};

@interface SCPopOverContentBubbleMasker: NSObject <SCCustomMaskViewMaskerProtocol>
@property (nonatomic, assign) SCPopOverContentBubbleStyle style;
@property (nonatomic, assign) CGFloat cornerRadius;

@property (nonatomic, assign) CGRect bounds;
- (void)updateMaskWithBounds:(CGRect)bounds handler: (void(^)(CGPathRef path)) handler;

@end

//result = origin + (swap(offset) * scale)
static CGPoint SCPopOverContentTransform(CGPoint origin, CGPoint offset, BOOL swapOffsetXY, CGPoint scale) {
    CGPoint result = origin;
    if (swapOffsetXY) {
        CGFloat t = offset.x;
        offset.x = offset.y;
        offset.y = t;
    }
    offset.x *= scale.x;
    offset.y *= scale.y;
    result.x += offset.x;
    result.y += offset.y;
    return result;
}


//去掉尖， 从尖开始 顺时针 6个点, arrow 必须在frame 的边上
static void SCPopOverContentOffsets(CGRect frame, CGPoint arrow) {
    
    
    
    
}

//static CGPoint SCPopOverContentOffsets[6] = {{.x = 16.f, .y = 13.f}, {.x = 12.f, .y = 12.f}, {.x = 7.f, .y = 6.7f}, {.x = 4.f, .y = 3.f}, {.x = 0.67f, .y = 0.67f}};


static CGPoint SCPopOverContentArrowOffsets[5] = {{.x = 16.f, .y = 13.f}, {.x = 12.f, .y = 12.f}, {.x = 7.f, .y = 6.7f}, {.x = 4.f, .y = 3.f}, {.x = 0.67f, .y = 0.67f}};
static BOOL SCPopOverContentOffsetArrowSwap[4] = {false, true, false, true};
static CGPoint SCPopOverContentArrowOffsetScale[4] = {{.x = 1.f, .y = -1.f}, {.x = 1.f, .y = -1.f}, {.x = 1.f, .y = -1.f}, {.x = 1.f, .y = -1.f}};
static CGPoint SCPopOverContentArrowOffsetScale2[4] = {{.x = -1.f, .y = 1.f}, {.x = 1.f, .y = -1.f}, {.x = 1.f, .y = -1.f}, {.x = 1.f, .y = -1.f}};



@interface SCInsetItem : NSObject
@property (nonatomic, assign) SCPopOverContentBubbleStyle style;
@property (nonatomic, assign) UIPopoverArrowDirection direction;
@property (nonatomic, assign) CGFloat length;
@end
@implementation SCInsetItem
@end

@interface SCPopOverContentView : SCCustomMaskView

@property (nonatomic, assign) BOOL isMaskable;

@property (nonatomic, assign) CGSize contentSize;

@property (nonatomic, assign) CGRect sourceRect;
@property (nonatomic, strong) UIView * sourceView;
@property (nonatomic, assign) UIPopoverArrowDirection permittedArrowDirections;

@property (nonatomic, assign) UIPopoverArrowDirection currentDirection;

@end

@implementation SCPopOverContentView

- (void)makeSubViews {

}

- (void)aa {
    if (self.sourceView.window && self.window) {
        CGRect frame = [self.sourceView convertRect:self.sourceRect toView:self.sourceView.window];
        CGRect bounds = self.sourceView.window.bounds;
        
        //计算距离四边inset
        NSMutableArray<SCInsetItem *> * insets = [NSMutableArray array];
        {
            SCInsetItem * item = [[SCInsetItem alloc] init];
            item.style = SCPopOverContentBubbleStyleTop;
            item.direction = UIPopoverArrowDirectionUp;
            item.length = frame.origin.y;
            [insets addObject:item];
        }
        {
            SCInsetItem * item = [[SCInsetItem alloc] init];
            item.style = SCPopOverContentBubbleStyleLeft;
            item.direction = UIPopoverArrowDirectionLeft;
            item.length = frame.origin.x;
            [insets addObject:item];
        }
        {
            SCInsetItem * item = [[SCInsetItem alloc] init];
            item.style = SCPopOverContentBubbleStyleBottom;
            item.direction = UIPopoverArrowDirectionDown;
            item.length = bounds.size.height - frame.origin.y - frame.size.height;
            [insets addObject:item];
        }
        {
            SCInsetItem * item = [[SCInsetItem alloc] init];
            item.style = SCPopOverContentBubbleStyleRight;
            item.direction = UIPopoverArrowDirectionRight;
            item.length = bounds.size.width - frame.origin.x - frame.size.width;
            [insets addObject:item];
        }
        //insets 排序， 大的在前
        [insets sortUsingComparator:^NSComparisonResult(SCInsetItem * _Nonnull obj1, SCInsetItem * _Nonnull obj2) {
            return obj1.length > obj2.length;
        }];

        UIPopoverArrowDirection permittedArrowDirections = self.permittedArrowDirections;
        SCInsetItem * itemused = nil;
        for (SCInsetItem * item in insets) {
            if ((item.direction & permittedArrowDirections) == item.direction) {
                itemused = item;
            }
        }
        if (nil == itemused) {
            itemused = insets.firstObject;
        }
        
        
        
    }
}



@end
@implementation SCPopOverContentBubbleMasker

- (void)updateMaskWithBounds:(CGRect)bounds handler: (void(^)(CGPathRef path)) handler {
    self.bounds = bounds;
//    if (_isLeft) {
//        [self setupLeftWithHandler:handler];
//    } else {
//        [self setupRightWithHandler:handler];
//    }
}

- (void)setupRightWithHandler: (void(^)(CGPathRef path)) handler
{
    CGMutablePathRef bubblePath = CGPathCreateMutable();
    CGPoint origin = self.bounds.origin;
    CGSize size = self.bounds.size;
    CGFloat cornerRadius = 5;
    
    CGFloat arrowTipRadiusA = 5;//
    CGFloat arrowTipRadiusB = 1;//尖部分
    CGFloat arrowTipWidth = arrowTipRadiusA;
    CGFloat arrowTipToTop = 14;
    
    CGPathMoveToPoint(bubblePath, NULL, origin.x, origin.y + cornerRadius);
    //左上角半圆
    CGPathAddArcToPoint(bubblePath, NULL, origin.x, origin.y, origin.x + cornerRadius, origin.y, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - cornerRadius - arrowTipWidth, origin.y);
    
    //右上角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + size.width - arrowTipWidth, origin.y, origin.x + size.width - arrowTipWidth, origin.y + cornerRadius, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - arrowTipWidth, origin.y + arrowTipToTop);
    
    //尖 - begin
    CGPathAddArc(bubblePath, NULL, origin.x + size.width, origin.y + arrowTipToTop, arrowTipRadiusA, M_PI, M_PI * 2.0 / 3, 1);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - arrowTipRadiusB, origin.y + arrowTipToTop + arrowTipRadiusA);
    CGPathAddArc(bubblePath, NULL, origin.x + size.width - arrowTipRadiusB, origin.y + arrowTipToTop + arrowTipRadiusA + arrowTipRadiusB, arrowTipRadiusB, - M_PI_2 , M_PI_2, 0);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - arrowTipRadiusA/2, origin.y + arrowTipToTop + arrowTipRadiusA + arrowTipRadiusB * 2 + (arrowTipRadiusA - sqrt(3) * arrowTipRadiusA/2));
    CGPathAddArc(bubblePath, NULL, origin.x + size.width, origin.y + arrowTipToTop + arrowTipRadiusA * 2 + arrowTipRadiusB * 2, arrowTipRadiusA, -(M_PI_2 + M_PI / 3), M_PI, 1);
    //尖 - end
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - arrowTipWidth, origin.y + size.height - cornerRadius);
    
    //右下角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + size.width - arrowTipWidth, origin.y + size.height, origin.x + size.width - cornerRadius - arrowTipWidth, origin.y + size.height, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + cornerRadius, origin.y + size.height);
    
    //左下角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x, origin.y + size.height, origin.x, origin.y + size.height - cornerRadius, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x, origin.y + cornerRadius);
    
    CGPathCloseSubpath(bubblePath);
    if (handler) {
        handler(bubblePath);
    }
    CGPathRelease(bubblePath);
}

- (void)setupLeftWithHandler: (void(^)(CGPathRef path)) handler
{
    CGMutablePathRef bubblePath = CGPathCreateMutable();
    CGPoint origin = self.bounds.origin;
    CGSize size = self.bounds.size;
    CGFloat cornerRadius = 5;
    
    CGFloat arrowTipRadiusA = 5;//
    CGFloat arrowTipRadiusB = 1;//尖部分
    CGFloat arrowTipWidth = arrowTipRadiusA;
    CGFloat arrowTipToTop = 14;
    
    CGPathMoveToPoint(bubblePath, NULL, origin.x + arrowTipWidth, origin.y + cornerRadius);
    
    //左上角半圆
    CGPathAddArcToPoint(bubblePath, NULL, origin.x + arrowTipWidth, origin.y, origin.x + cornerRadius + arrowTipWidth, origin.y, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width - cornerRadius, origin.y);
    
    //右上角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + size.width, origin.y, origin.x + size.width , origin.y + cornerRadius, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + size.width, origin.y + size.height - cornerRadius);
    
    //右下角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + size.width, origin.y + size.height, origin.x + size.width - cornerRadius, origin.y + size.height, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + cornerRadius + arrowTipWidth, origin.y + size.height);
    
    //左下角半圆
    CGPathAddArcToPoint(bubblePath, NULL,  origin.x + arrowTipWidth, origin.y + size.height, origin.x + arrowTipWidth, origin.y + size.height - cornerRadius, cornerRadius);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + arrowTipWidth, origin.y + arrowTipRadiusA * 2 + arrowTipRadiusB * 2 + arrowTipToTop);
    
    //尖 - begin
    CGPathAddArc(bubblePath, NULL, origin.x, origin.y + arrowTipToTop + arrowTipRadiusA * 2 + arrowTipRadiusB * 2, arrowTipRadiusA, 0, -(M_PI / 3), 1);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + arrowTipRadiusB, origin.y + arrowTipToTop + arrowTipRadiusA + arrowTipRadiusB * 2);
    CGPathAddArc(bubblePath, NULL, origin.x + arrowTipRadiusB, origin.y + arrowTipToTop + arrowTipRadiusA + arrowTipRadiusB, arrowTipRadiusB, M_PI_2 , 3 * M_PI_2, 0);
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + arrowTipRadiusA/2, origin.y + arrowTipToTop + arrowTipRadiusA * sqrt(3) / 2);
    CGPathAddArc(bubblePath, NULL, origin.x, origin.y + arrowTipToTop, arrowTipRadiusA, M_PI / 3, 0, 1);
    //尖 - end
    
    CGPathAddLineToPoint(bubblePath, NULL, origin.x + arrowTipWidth, origin.y + cornerRadius);
    CGPathCloseSubpath(bubblePath);
    if (handler) {
        handler(bubblePath);
    }
    CGPathRelease(bubblePath);
}
@end




@interface SCBasePopOverViewController ()
@property (nonatomic, assign) CGRect sourceRect;
@property (nonatomic, strong) UIView * sourceView;
@property (nonatomic, assign) UIPopoverArrowDirection permittedArrowDirections;

@end

@implementation SCBasePopOverViewController

- (instancetype)initWithSourceView:(UIView *)sourceView {
    return [self initWithSourceView:sourceView sourceRect:sourceView.bounds];
}

- (instancetype)initWithSourceView:(UIView *)sourceView sourceRect:(CGRect)sourceRect {
    return [self initWithSourceView:sourceView sourceRect:sourceRect permittedArrowDirections:UIPopoverArrowDirectionAny];
}
- (instancetype)initWithSourceView:(UIView *)sourceView sourceRect:(CGRect)sourceRect permittedArrowDirections:(UIPopoverArrowDirection)permittedArrowDirections {
    self = [super initWithNibName:nil bundle:nil];
    if (self) {

    }
    return self;
}



- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (__kindof UIView *)makeContentView {
    
    
    
    
    return nil;
}


@end
