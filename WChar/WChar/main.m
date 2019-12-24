//
//  main.m
//  WChar
//
//  Created by vector on 2019/12/24.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <wchar.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        
//        setlocale(1, NULL);
        
        // insert code here...
        NSLog(@"Hello, World!");
        
//        setlocale(LC_ALL, "");
            
        wprintf(L"content end\n");

        // wprintf和printf最好不能同时使用.
        // 如下使用printf同时打印了char字符串和wchar_t字符串
        // 因此只采用printf是比较好的方法
        char * a = setlocale(LC_ALL, "zh_CN.UTF-8");
        
        wchar_t * ws = L"国家";
        printf("%ls ----\n", ws);
        
//        wprintf(ws);
//        wprintf(L"printf content start: %ls\n", ws);
//        wprintf(L"content end\n");
        return 0;
    }
    return 0;
}
