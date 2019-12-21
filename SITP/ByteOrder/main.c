//
//  main.c
//  ByteOrder
//
//  Created by vector on 2019/12/20.
//  Copyright © 2019 angfung. All rights reserved.
//

#include <stdio.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    uint32_t a = 1;
    
    uint8_t * byte = &a;
    printf("%ld %ld\n", byte[0], byte[3]);
    
    printf("%d \n", -11 % 8);

    
    return 0;
}
