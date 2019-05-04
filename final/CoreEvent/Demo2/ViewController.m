//
//  ViewController.m
//  Demo2
//
//  Created by vector on 2019/5/3.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#import "ViewController.h"
#import <CoreEvent/CESem.h>

struct _CEParamItem {
    uint32_t index: 6;
    uint32_t type: 4;
    uint32_t hasDealloc: 1;//type 为 CETaskParamItemTypePtr 时 使能
    uint32_t hasTypeName: 1;//type 为 CETaskParamItemTypePtr 或者 CETaskParamItemTypeStruct 时 使能
    uint32_t paddingSize: 4;//[0, 15]
    uint32_t contentSize: 16;
};

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    CESemRef sem = CESemInit(0);

    NSLog(@"%ld", sizeof(struct _CEParamItem));
    
    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
    
}


@end
