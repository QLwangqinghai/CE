//
//  ViewController.m
//  SITPNumber
//
//  Created by vector on 2019/12/23.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"


static uint8_t a __attribute__((aligned(64))) = 8;

static uint8_t b __attribute__((aligned(32))) = 18;

static uint32_t c __attribute__((aligned(128))) = 16;




@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.

    uintptr_t ap = &a;
    uintptr_t bp = &b;
    uintptr_t cp = &c;
    NSLog(@"%ld, %ld, %ld", sizeof(a), bp % 32, cp % 128);

    
    NSLog(@"%ld, %ld, %ld", ap % 64, bp % 32, cp % 128);
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
