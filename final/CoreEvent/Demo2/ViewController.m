//
//  ViewController.m
//  Demo2
//
//  Created by vector on 2019/5/3.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#import "ViewController.h"

#pragma pack(push)
#pragma pack(1)

struct __CETypeBaseInfo111 {
    uint8_t objectSize1;
    uint16_t objectSize2;
};

#pragma pack(pop)

struct __CETypeBaseInfo112 {
    uint8_t objectSize1;
    uint16_t objectSize2;
};

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
//    CESemRef sem = CESemInit(0);
//
    NSLog(@"%ld", sizeof(struct __CETypeBaseInfo111));
    NSLog(@"%ld", sizeof(struct __CETypeBaseInfo112));

    
    NSInteger i=3;
    
    while (i < 243) {
        int j=0;
        while (j < 16) {
            j ++;
            printf("%ld, ", i);
            
            i++;
        }
        
        printf("\n");

    }

    
    
//
    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
    
}


@end
