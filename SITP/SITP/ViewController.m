//
//  ViewController.m
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"
#include "SITPParser.h"

SITPParserCode SITPParserParseCallbackFunc(void * _Nullable context, SITPField_t field) {
    NSLog(@"%ld %ld", field.index, field.type);
    return SITPParserCodeSuccess;
}



@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    uint8_t buffer[512] = {
        0x2,
        0x4,
        0x1, 0x2, 0x3, 0x4,
        0x21,

        
    };
    
    
    SITPByteBuffer_t page = {
        .content = buffer,
    };
    page.range = SITPByteRangeMake(0, 7);
    
    SITPParserCode code = SITPParserParseData(NULL, &page, 1, SITPByteRangeMake(0, 7), SITPParserParseCallbackFunc);
    NSLog(@"code %ld", code);

//    [self description];
    
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
