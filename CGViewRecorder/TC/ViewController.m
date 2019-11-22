//
//  ViewController.m
//  TC
//
//  Created by vector on 2019/11/15.
//  Copyright © 2019 vector. All rights reserved.
//

#import "ViewController.h"

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    
    
    void * p = malloc(UINT32_MAX);
    //success
    NSLog(@"p", p);
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}




@end




typedef uint64_t CCIndex;
typedef uint8_t * CCPagePtr;


