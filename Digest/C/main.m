//
//  main.m
//  C
//
//  Created by vector on 2019/8/10.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <math.h>

#define CDSHA2Process32Key_0 0x428a2f98UL
#define CDSHA2Process32Key_1 0x71374491UL
#define CDSHA2Process32Key_2 0xb5c0fbcfUL
#define CDSHA2Process32Key_3 0xe9b5dba5UL
#define CDSHA2Process32Key_4 0x3956c25bUL
#define CDSHA2Process32Key_5 0x59f111f1UL
#define CDSHA2Process32Key_6 0x923f82a4UL
#define CDSHA2Process32Key_7 0xab1c5ed5UL
#define CDSHA2Process32Key_8 0xd807aa98UL
#define CDSHA2Process32Key_9 0x12835b01UL
#define CDSHA2Process32Key_10 0x243185beUL
#define CDSHA2Process32Key_11 0x550c7dc3UL
#define CDSHA2Process32Key_12 0x72be5d74UL
#define CDSHA2Process32Key_13 0x80deb1feUL
#define CDSHA2Process32Key_14 0x9bdc06a7UL
#define CDSHA2Process32Key_15 0xc19bf174UL
#define CDSHA2Process32Key_16 0xe49b69c1UL
#define CDSHA2Process32Key_17 0xefbe4786UL
#define CDSHA2Process32Key_18 0xfc19dc6UL
#define CDSHA2Process32Key_19 0x240ca1ccUL
#define CDSHA2Process32Key_20 0x2de92c6fUL
#define CDSHA2Process32Key_21 0x4a7484aaUL
#define CDSHA2Process32Key_22 0x5cb0a9dcUL
#define CDSHA2Process32Key_23 0x76f988daUL
#define CDSHA2Process32Key_24 0x983e5152UL
#define CDSHA2Process32Key_25 0xa831c66dUL
#define CDSHA2Process32Key_26 0xb00327c8UL
#define CDSHA2Process32Key_27 0xbf597fc7UL
#define CDSHA2Process32Key_28 0xc6e00bf3UL
#define CDSHA2Process32Key_29 0xd5a79147UL
#define CDSHA2Process32Key_30 0x6ca6351UL
#define CDSHA2Process32Key_31 0x14292967UL
#define CDSHA2Process32Key_32 0x27b70a85UL
#define CDSHA2Process32Key_33 0x2e1b2138UL
#define CDSHA2Process32Key_34 0x4d2c6dfcUL
#define CDSHA2Process32Key_35 0x53380d13UL
#define CDSHA2Process32Key_36 0x650a7354UL
#define CDSHA2Process32Key_37 0x766a0abbUL
#define CDSHA2Process32Key_38 0x81c2c92eUL
#define CDSHA2Process32Key_39 0x92722c85UL
#define CDSHA2Process32Key_40 0xa2bfe8a1UL
#define CDSHA2Process32Key_41 0xa81a664bUL
#define CDSHA2Process32Key_42 0xc24b8b70UL
#define CDSHA2Process32Key_43 0xc76c51a3UL
#define CDSHA2Process32Key_44 0xd192e819UL
#define CDSHA2Process32Key_45 0xd6990624UL
#define CDSHA2Process32Key_46 0xf40e3585UL
#define CDSHA2Process32Key_47 0x106aa070UL
#define CDSHA2Process32Key_48 0x19a4c116UL
#define CDSHA2Process32Key_49 0x1e376c08UL
#define CDSHA2Process32Key_50 0x2748774cUL
#define CDSHA2Process32Key_51 0x34b0bcb5UL
#define CDSHA2Process32Key_52 0x391c0cb3UL
#define CDSHA2Process32Key_53 0x4ed8aa4aUL
#define CDSHA2Process32Key_54 0x5b9cca4fUL
#define CDSHA2Process32Key_55 0x682e6ff3UL
#define CDSHA2Process32Key_56 0x748f82eeUL
#define CDSHA2Process32Key_57 0x78a5636fUL
#define CDSHA2Process32Key_58 0x84c87814UL
#define CDSHA2Process32Key_59 0x8cc70208UL
#define CDSHA2Process32Key_60 0x90befffaUL
#define CDSHA2Process32Key_61 0xa4506cebUL
#define CDSHA2Process32Key_62 0xbef9a3f7UL
#define CDSHA2Process32Key_63 0xc67178f2UL

#define CDSHA2Process32Key(index) CDSHA2Process32Key_##index

static const uint32_t k[80] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL,
};



int main(int argc, const char * argv[]) {
    @autoreleasepool {
//        // insert code here...
//        NSLog(@"Hello, World!");
//
//        NSLog(@"%x", (uint8_t)(-1));
//        NSLog(@"%x", (uint8_t)(-2));
//        NSLog(@"%x", (uint8_t)(-128));
//
//        uint32_t a = 1;
//        int16_t b = -1;
//        uint32_t d = (uint32_t)b;
//
//        uint32_t c = a - b;
        uint32 v = CDSHA2Process32Key(1);

        
        // Main loop
        for (uint32_t i=0; i<64; i+=8) {
            printf("CDSHA2th256RND(A,B,C,D,E,F,G,H,%ld);\n", i+0);
            printf("CDSHA2th256RND(H,A,B,C,D,E,F,G,%ld);\n", i+1);
            printf("CDSHA2th256RND(G,H,A,B,C,D,E,F,%ld);\n", i+2);
            printf("CDSHA2th256RND(F,G,H,A,B,C,D,E,%ld);\n", i+3);
            printf("CDSHA2th256RND(E,F,G,H,A,B,C,D,%ld);\n", i+4);
            printf("CDSHA2th256RND(D,E,F,G,H,A,B,C,%ld);\n", i+5);
            printf("CDSHA2th256RND(C,D,E,F,G,H,A,B,%ld);\n", i+6);
            printf("CDSHA2th256RND(B,C,D,E,F,G,H,A,%ld);\n", i+7);

            printf("\n");

            
//            CDSHA2th256RND(A,B,C,D,E,F,G,H,i+0);
//            CDSHA2th256RND(H,A,B,C,D,E,F,G,i+1);
//            CDSHA2th256RND(G,H,A,B,C,D,E,F,i+2);
//            CDSHA2th256RND(F,G,H,A,B,C,D,E,i+3);
//            CDSHA2th256RND(E,F,G,H,A,B,C,D,i+4);
//            CDSHA2th256RND(D,E,F,G,H,A,B,C,i+5);
//            CDSHA2th256RND(C,D,E,F,G,H,A,B,i+6);
//            CDSHA2th256RND(B,C,D,E,F,G,H,A,i+7);
        }

        
        
        for (NSInteger i=0; i<64; i++) {
//            uint32 v = CDSHA2Process32Key(i);
            printf("#define CDSHA2Process32Key_%ld 0x%xUL\n", i, v);
        }
        
        
    }
    return 0;
}
