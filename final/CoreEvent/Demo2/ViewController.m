//
//  ViewController.m
//  Demo2
//
//  Created by vector on 2019/5/3.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#import "ViewController.h"
#import <CoreEvent/CESem.h>



struct __CETypeBaseInfo111 {
    void * _Nonnull type;
    uint16_t version;
    uint16_t masks;
    uint32_t objectSize;//objectSize 为0 时 size 为变长，getCustomSize 必须非空
    size_t (* _Nullable getCustomSize)(CETypeBase_s * _Nonnull type, void * _Nonnull object);
    
    CETypeBase_s const * _Nullable alloctor;//CMAlloctor_s * description
    void (* _Nonnull init)(void * _Nonnull object);
    void (* _Nonnull deinit)(void * _Nonnull object);
    
    char * _Nonnull name;
    size_t (* _Nonnull descript)(void * _Nonnull object, char * _Nonnull buffer, size_t bufferSize);
    
    void * _Nonnull (* _Nonnull retain)(void * _Nonnull object);
    void * _Nullable (* _Nonnull tryRetain)(void * _Nonnull object);
    void (* _Nonnull release)(void * _Nonnull object);
};

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    CESemRef sem = CESemInit(0);

    NSLog(@"%ld", sizeof(struct __CETypeBaseInfo111));
    
    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
    
}


@end
