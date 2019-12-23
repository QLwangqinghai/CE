//
//  main.c
//  ByteOrder
//
//  Created by vector on 2019/12/20.
//  Copyright © 2019 angfung. All rights reserved.
//

#include <stdio.h>


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


int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    uint32_t a = 1;
    
    uint8_t * byte = &a;
    printf("%ld %ld\n", byte[0], byte[3]);
    
    printf("%d %d\n", sizeof(SITPTime2), sizeof(SITPTime1));

    
    return 0;
}
