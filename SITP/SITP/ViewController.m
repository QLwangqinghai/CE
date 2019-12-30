//
//  ViewController.m
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"
#include "SITPParser.h"
#include "CCByte.h"

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
    
    NSData * data = [@"0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 62," dataUsingEncoding:NSUTF8StringEncoding];
    NSMutableData * mdata = [NSMutableData dataWithLength:data.length * 2];
    NSMutableData * mdata2 = [NSMutableData dataWithLength:data.length];

    CCInt le = CCByteBase64EncodeBytes(data.bytes, data.length, mdata.mutableBytes, mdata.length, false);
    
    
    CCInt le2 = CCByteBase64DecodeBytes(mdata.mutableBytes, le, mdata2.mutableBytes, mdata2.length);
    
    
    BOOL r = [data isEqualToData:mdata2];
    

}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
