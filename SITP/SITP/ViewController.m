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

//SITPParserCode SITPParserParseCallbackFunc(void * _Nullable context, SITPField_t field) {
//    NSLog(@"%ld %ld", field.index, field.type);
//    return SITPParserCodeSuccess;
//}



@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    uint8_t buffer[512] = {
        0x2,
        0x2,
        0x4,
        0x1, 0x2, 0x3, 0x4,
        0x21,

        
    };
    
    NSUTF8StringEncoding;
    
    SITPByteBuffer_t page = {
        .content = buffer,
    };
    page.range = SITPByteRangeMake(0, 7);
    
    SITPParserErrorRef error = NULL;
    SITPParserParseMessageResult_t pr = SITPParserParseMessage(&page, 1, SITPByteRangeMake(0, 7), &error);
    
//    SITPParserCode code = SITPParserParseData(NULL, &page, 1, SITPByteRangeMake(0, 7), NULL);
//    NSLog(@"code %ld", code);
    NSMutableData * data = [NSMutableData dataWithLength:116];
    SecRandomCopyBytes(kSecRandomDefault, data.length, data.mutableBytes);
//    [self description];
    NSMutableData * mdata = [NSMutableData dataWithLength:data.length * 2];
    NSMutableData * mdata2 = [NSMutableData dataWithLength:data.length];

    CFTimeInterval a = CACurrentMediaTime();
    CCInt le = CCByteBase64EncodeBytes(data.bytes, data.length, mdata.mutableBytes, mdata.length);
    CCInt le2 = CCByteBase64DecodeBytes(mdata.mutableBytes, le, mdata2.mutableBytes, mdata2.length);
    CFTimeInterval b = CACurrentMediaTime();
    [data base64EncodedDataWithOptions:0];
    CFTimeInterval c = CACurrentMediaTime();
    NSLog(@"mtime:%.07lf", b - a);
    NSLog(@"ctime:%.07lf", c - b);

    
    NSString * encoded = [[NSString alloc] initWithData:[mdata subdataWithRange:NSMakeRange(0, le)] encoding:NSUTF8StringEncoding];
    NSLog(@"encoded:\n%@", encoded);

    BOOL r = [data isEqualToData:mdata2];
    assert(r);
    
    
    

}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
