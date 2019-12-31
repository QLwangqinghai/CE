//
//  main.c
//  ByteOrder
//
//  Created by vector on 2019/12/20.
//  Copyright © 2019 angfung. All rights reserved.
//

#include <Foundation/Foundation.h>


typedef struct {
    int32_t sub;
    int64_t timestamp;
} SITPTime;

#pragma pack(push, 4)
typedef struct {
    int32_t sub;
    int64_t timestamp;
} SITPTime2;
#pragma pack(pop)

typedef struct {

    int64_t timestamp;
    int32_t sub;

} SITPTime1 __attribute__((aligned));


static const char __CCBase64ByteToCharMappings[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '_', '-',
};

void lll() {
    {
        NSCharacterSet * set = [NSCharacterSet URLUserAllowedCharacterSet];
        NSLog(@"URLUserAllowedCharacterSet %@", set);
    }
    {
        NSCharacterSet * set = [NSCharacterSet URLPasswordAllowedCharacterSet];
        NSLog(@"URLPasswordAllowedCharacterSet %@", set);
    }
    {
        NSCharacterSet * set = [NSCharacterSet URLHostAllowedCharacterSet];
        NSLog(@"URLHostAllowedCharacterSet %@", set);
    }
    {
        NSCharacterSet * set = [NSCharacterSet URLPathAllowedCharacterSet];
        NSLog(@"URLPathAllowedCharacterSet %@", set);
    }
    {
        NSCharacterSet * set = [NSCharacterSet URLQueryAllowedCharacterSet];
        NSLog(@"URLQueryAllowedCharacterSet %@", set);
    }
    {
        NSCharacterSet * set = [NSCharacterSet URLFragmentAllowedCharacterSet];
        NSLog(@"URLFragmentAllowedCharacterSet %@", set);
    }
}


int main(int argc, const char * argv[]) {
    // insert code here...

    NSMutableURLRequest * req = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:@"https://www.baidu.com"]];
    
    uint32_t a = 1;
    
    uint8_t * byte = &a;
    printf("%ld %ld\n", byte[0], byte[3]);
    
    printf("%d %d\n", sizeof(SITPTime2), sizeof(struct timeval));
    printf("\n");
    printf("\n");
    
    NSMutableString * url = [NSMutableString string];
    [url appendString:@"http://baidu.com?a="];
    NSMutableDictionary<NSString *, NSString *> * dict = [NSMutableDictionary dictionary];
    for (uint8_t i=0; i<64; i++) {
//        dict[[NSString stringWithFormat:@"%c", __CCBase64ByteToCharMappings[i]]] = @(i);
        dict[[NSString stringWithFormat:@"%d",  __CCBase64ByteToCharMappings[i]]] = [NSString stringWithFormat:@"%d", i];

        [url appendFormat:@"%c", __CCBase64ByteToCharMappings[i]];
    }
    [url appendString:@"~"];
    NSString * url2 = [url stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];

    printf("\n");
    printf("\n");

    NSLog(url);
    NSLog(url2);
    
    BOOL equaled = [url isEqualToString:url2];
    

    printf("\n");
    printf("\n");

    NSMutableString * str = [NSMutableString string];
    for (uint16_t i=0; i<128; i++) {
        NSString * v = dict[[NSString stringWithFormat:@"%d", i]];
        if (nil == v) {
            v = @"255";
        }
        
        if (str.length > 0) {
            if (i % 32 == 0) {
                [str appendString:@",\n"];
            } else {
                [str appendString:@", "];
            }
        }
        [str appendString:v];
    }
    NSLog(@"\n%@\n", str);

//
//    for (uint8_t i=65; i<91; i++) {
//        printf("'%c', ", i);
//    }
//    printf("\n");
//    for (uint8_t i=97; i<123; i++) {
//        printf("'%c', ", i);
//
//    }
//    printf("\n");
//    for (uint8_t i=48; i<58; i++) {
//        printf("'%c', ", i);
//    }
//    printf("\n");

    
    return 0;
}
