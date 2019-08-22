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
    for(int i = 0; i < numIvars; i++) {
        Ivar ivar = vars[i];
        NSString * key = [NSString stringWithUTF8String:ivar_getName(ivar)];
        NSString * type = [NSString stringWithUTF8String:ivar_getTypeEncoding(ivar)];
        
        [info.vars addObject:[NSString stringWithFormat:@"%@: %@", key, type]];
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
    NSLog(@"vars:%@\n\n\n", viewInfo.vars);
    NSLog(@"methods:%@\n", viewInfo.methods);

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




//- (void)drawImage
//
//{
//    
//        CGImageRef cgimage = [self.imageView.image CGImage];
//    
//        
//    
//        size_t width = CGImageGetWidth(cgimage); // 图片宽度
//    
//        size_t height = CGImageGetHeight(cgimage); // 图片高度
//    
//        unsigned char *data = calloc(width * height * 4, sizeof(unsigned char)); // 取图片首地址
//    
//        size_t bitsPerComponent = 8; // r g b a 每个component bits数目
//    
//        size_t bytesPerRow = width * 4; // 一张图片每行字节数目 (每个像素点包含r g b a 四个字节)
//    
//        CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB(); // 创建rgb颜色空间
//    
//        
//    
//        CGContextRef context =
//    
//        CGBitmapContextCreate(data,
//                              
//                                                        width,
//                              
//                                                        height,
//                              
//                                                        bitsPerComponent,
//                              
//                                                        bytesPerRow,
//                              
//                                                        space,
//                              
//                                                        kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
//    
//        CGContextDrawImage(context, CGRectMake(0, 0, width, height), cgimage);
//    
//        
//    
//        for (size_t i = 0; i < height; i++)
//        
//            {
//            
//                    for (size_t j = 0; j < width; j++)
//                
//                        {
//                    
//                                size_t pixelIndex = i * width * 4 + j * 4;
//                    
//                                
//                    
//                                unsigned char red = data[pixelIndex];
//                    
//                                unsigned char green = data[pixelIndex + 1];
//                    
//                                unsigned char blue = data[pixelIndex + 2];
//                    
//                                
//                    
//                        // 修改颜色
//                    
//                                red = 0;
//                    
//                                data[pixelIndex] = red;
//                    
//                                data[pixelIndex] = green;
//                    
//                                data[pixelIndex] = blue;
//                    
//                            }
//            
//                }
//    
//        
//    
//        cgimage = CGBitmapContextCreateImage(context);
//    
//        self.imageView.image = [UIImage imageWithCGImage:cgimage];
//    
//}
