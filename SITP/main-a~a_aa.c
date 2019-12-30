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


static const uint8_t __CCBase64ByteToCharMappings[64] __attribute__((64)) = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
};



int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    uint32_t a = 1;
    
    uint8_t * byte = &a;
    printf("%ld %ld\n", byte[0], byte[3]);
    
    printf("%d %d\n", sizeof(SITPTime2), sizeof(struct timeval));
    printf("\n");
    printf("\n");

    
    for (uint8_t i=65; i<91; i++) {
        printf("'%c', ", i);
    }
    printf("\n");
    for (uint8_t i=97; i<123; i++) {
        printf("'%c', ", i);
        
    }
    printf("\n");
    for (uint8_t i=48; i<58; i++) {
        printf("'%c', ", i);
    }
    printf("\n");

    
    return 0;
}
