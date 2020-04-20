//
//  main.m
//  CCallFunc
//
//  Created by vector on 2020/3/31.
//  Copyright © 2020 haoqi. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CCallFunc-Swift.h"



const char * astring = "Foundationasdfewarqwrqr";
const char * bstring = "Foundationasdfewarqwrqr";




void fff(int a, int b, int c) {
    printf("%d, %d, %d\n", a, b, c);
}



int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
//        NSLog(@"Hello, World!");
//
//        printf("%p \n%p\n", astring, bstring);
//
//        void(*f)(int a) = fff;
//
//
//        int c = 1;
//        int b = 2;
//        int a = 3;
//        int d = 14;
//
//        f(d);
        
        C * c = [[C alloc] init];
        
        NSMutableData * data = [NSMutableData data];
        [data appendData:[@"data" dataUsingEncoding:NSUTF8StringEncoding]];
        [data appendData:[@"utf8string" dataUsingEncoding:NSUTF8StringEncoding]];
        NSString * string = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        
        NSLog(string);
        char * str = (char *)(string.UTF8String);
        str[5] = 'a';
        NSLog(string);
    }
    return 0;
}
