//
//  ViewController.m
//  Demo2
//
//  Created by vector on 2019/5/3.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#import "ViewController.h"
#import <CoreEvent/CESem.h>

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    CESemRef sem = CESemInit(0);

    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
    
}


@end
