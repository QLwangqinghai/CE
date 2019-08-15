//
//  ScrollViewCore.m
//  ScrollView
//
//  Created by vector on 2019/8/15.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ScrollViewCore.h"
#import <objc/runtime.h>


@interface ClassInfo : NSObject

@property (nonatomic, strong, readonly) NSMutableSet<NSString *> * methods;
@property (nonatomic, strong, readonly) NSMutableSet<NSString *> * vars;

@end
@implementation ClassInfo

- (instancetype)init {
    self = [super init];
    if (self) {
        _methods = [NSMutableSet set];
        _vars = [NSMutableSet set];
    }
    return self;
}

@end

ClassInfo * classInfo(Class cls) {
    ClassInfo * info = [[ClassInfo alloc] init];
    unsigned int count;
    Method *methods = class_copyMethodList(cls, &count);
    for (int i = 0; i < count; i++) {
        Method method = methods[i];
        SEL selector = method_getName(method);
        NSString *name = NSStringFromSelector(selector);
        [info.methods addObject:name];
    }
    
    unsigned int numIvars;
    Ivar *vars = class_copyIvarList(cls, &numIvars);
    NSString *key=nil;
    for(int i = 0; i < numIvars; i++) {
        
        Ivar thisIvar = vars[i];
        key = [NSString stringWithUTF8String:ivar_getName(thisIvar)];
        [info.vars addObject:key];
    }
    free(vars);
    return info;
}

@implementation ScrollViewCore

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

+ (void)log {
    ClassInfo * viewInfo = classInfo([UIView class]);
    ClassInfo * scrollViewInfo = classInfo([UIScrollView class]);
    ClassInfo * tableViewInfo = classInfo([UITableView class]);
    ClassInfo * collectionViewInfo = classInfo([UICollectionView class]);

    NSLog(@"UIView\n");
    
    NSLog(@"methods:%@\n", viewInfo.methods);
    NSLog(@"vars:%@\n\n\n", viewInfo.vars);

    NSLog(@"UIScrollView\n");
    {
        NSMutableArray * array = [NSMutableArray array];
        [scrollViewInfo.methods enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, BOOL * _Nonnull stop) {
            if (![viewInfo.methods containsObject:obj]) {
                [array addObject:obj];
            }
        }];
        NSLog(@"methods:%@\n", array);

        [array removeAllObjects];
        [scrollViewInfo.vars enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, BOOL * _Nonnull stop) {
            if (![viewInfo.vars containsObject:obj]) {
                [array addObject:obj];
            }
        }];
        NSLog(@"vars:%@\n", array);
    }
    
    
    NSLog(@"UITableView\n");
    
    {
        NSMutableArray * array = [NSMutableArray array];
        [tableViewInfo.methods enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, BOOL * _Nonnull stop) {
            if (![scrollViewInfo.methods containsObject:obj]) {
                [array addObject:obj];
            }
        }];
        NSLog(@"methods:%@\n", array);
        
        [array removeAllObjects];
        [tableViewInfo.vars enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, BOOL * _Nonnull stop) {
            if (![scrollViewInfo.vars containsObject:obj]) {
                [array addObject:obj];
            }
        }];
        NSLog(@"vars:%@\n", array);
    }
    

    NSLog(@"UICollectionView\n");
    {
        NSMutableArray * array = [NSMutableArray array];
        [collectionViewInfo.methods enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, BOOL * _Nonnull stop) {
            if (![scrollViewInfo.methods containsObject:obj]) {
                [array addObject:obj];
            }
        }];
        NSLog(@"methods:%@\n", array);
        
        [array removeAllObjects];
        [collectionViewInfo.vars enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, BOOL * _Nonnull stop) {
            if (![scrollViewInfo.vars containsObject:obj]) {
                [array addObject:obj];
            }
        }];
        NSLog(@"vars:%@\n", array);
    }
}


+ (void)load {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        
        [self log];
    });
    
}

@end
