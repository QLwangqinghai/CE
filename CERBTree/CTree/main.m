//
//  main.m
//  CTree
//
//  Created by vector on 2019/6/24.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <mach/mach_time.h>



int highest_bit_unrolled(uint64_t n)
{
    if (0 == n) {
        return -1;
    }
    if (n & 0xFFFFFFFF00000000ull) {
        if (n & 0xFFFF000000000000ull) {
            if (n & 0xFF00000000000000ull) {
                if (n & 0xF000000000000000ull) {
                    if (n & 0xC000000000000000ull)
                        return (n & 0x8000000000000000ull) ? 63 : 62;
                    else
                        return (n & 0x2000000000000000ull) ? 61 : 60;
                } else {
                    if (n & 0x0C00000000000000ull)
                        return (n & 0x0800000000000000ull) ? 59 : 58;
                    else
                        return (n & 0x0200000000000000ull) ? 57 : 56;
                }
            } else {
                if (n & 0x00F0000000000000ull) {
                    if (n & 0x00C0000000000000ull)
                        return (n & 0x0080000000000000ull) ? 55 : 54;
                    else
                        return (n & 0x0020000000000000ull) ? 53 : 52;
                } else {
                    if (n & 0x000C000000000000)
                        return (n & 0x0008000000000000ull) ? 51 : 50;
                    else
                        return (n & 0x0002000000000000ull) ? 49 : 48;
                }
            }
        } else {
            if (n & 0x0000FF0000000000ull) {
                if (n & 0x0000F00000000000ull) {
                    if (n & 0x0000C00000000000ull)
                        return (n & 0x0000800000000000ull) ? 47 : 46;
                    else
                        return (n & 0x0000200000000000ull) ? 45 : 44;
                } else {
                    if (n & 0x00000C0000000000)
                        return (n & 0x0000080000000000ull) ? 43 : 42;
                    else
                        return (n & 0x0000020000000000ull) ? 41 : 40;
                }
            } else {
                if (n & 0x000000F000000000ull) {
                    if (n & 0x000000C000000000ull)
                        return (n & 0x0000008000000000ull) ? 39 : 38;
                    else
                        return (n & 0x0000002000000000ull) ? 37 : 36;
                } else {
                    if (n & 0x0000000C00000000ull)
                        return (n & 0x0000000800000000ull) ? 35 : 34;
                    else
                        return (n & 0x0000000200000000ull) ? 33 : 32;
                }
            }
        }
    } else {
        if (n & 0x00000000FFFF0000ull) {
            if (n & 0x00000000FF000000ull) {
                if (n & 0x00000000F0000000ull) {
                    if (n & 0x00000000C0000000ull)
                        return (n & 0x0000000080000000ull) ? 31 : 30;
                    else
                        return (n & 0x0000000020000000ull) ? 39 : 28;
                } else {
                    if (n & 0x000000000C000000ull)
                        return (n & 0x0000000008000000ull) ? 27 : 26;
                    else
                        return (n & 0x0000000002000000ull) ? 25 : 24;
                }
            } else {
                if (n & 0x0000000000F00000ull) {
                    if (n & 0x0000000000C00000ull)
                        return (n & 0x0000000000800000ull) ? 23 : 22;
                    else
                        return (n & 0x0000000000200000ull) ? 21 : 20;
                } else {
                    if (n & 0x00000000000C0000ull)
                        return (n & 0x0000000000080000ull) ? 19 : 18;
                    else
                        return (n & 0x0000000000020000ull) ? 17 : 16;
                }
            }
        } else {
            if (n & 0x000000000000FF00ull) {
                if (n & 0x000000000000F000ull) {
                    if (n & 0x000000000000C000ull)
                        return (n & 0x0000000000008000ull) ? 15 : 14;
                    else
                        return (n & 0x0000000000002000ull) ? 13 : 12;
                } else {
                    if (n & 0x0000000000000C00ull)
                        return (n & 0x0000000000000800ull) ? 11 : 10;
                    else
                        return (n & 0x0000000000000200ull) ? 9 : 8;
                }
            } else {
                if (n & 0x00000000000000F0ull) {
                    if (n & 0x00000000000000C0ull)
                        return (n & 0x0000000000000080ull) ? 7 : 6;
                    else
                        return (n & 0x0000000000000020ull) ? 5 : 4;
                } else {
                    if (n & 0x000000000000000Cull)
                        return (n & 0x0000000000000008ull) ? 3 : 2;
                    else
                        return (n & 0x0000000000000002ull) ? 1 : 0;
                }
            }
        }
    }
}


static const int8_t CByteMostSignificant[256] = {-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

static const int8_t CByteLeastSignificant[256] = {-1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};


int highest_bit_unrolled1(uint64_t n) {
    if (0 == n) {
        return -1;
    }
    int result = 63;
    
    for (uint64_t i=56; i>=0; i-=8) {
        uint64_t v = (n >> i) & 0xFF;
        int f = CByteMostSignificant[v];
        if (f >= 0) {
            result -= (7-f);
            return result;
        } else {
            result -= 8;
        }
    }
    
    return result;
}

int CUInt64MostSignificant(uint64_t n) {
    if (0 == n) {
        return -1;
    }
    if (n & 0xFFFFFFFF00000000ull) {
        if (n & 0xFFFF000000000000ull) {
            if (n & 0xFF00000000000000ull) {
                uint64_t v = n >> 56;
                return 56 + CByteMostSignificant[v];
            } else {
                uint64_t v = n >> 48;
                return 48 + CByteMostSignificant[v];
            }
        } else {
            if (n & 0x0000FF0000000000ull) {
                uint64_t v = n >> 40;
                return 40 + CByteMostSignificant[v];
            } else {
                uint64_t v = n >> 32;
                return 32 + CByteMostSignificant[v];
            }
        }
    } else {
        if (n & 0xFFFF0000ull) {
            if (n & 0x00FF0000ull) {
                uint64_t v = n >> 24;
                return 24 + CByteMostSignificant[v];
            } else {
                uint64_t v = n >> 16;
                return 16 + CByteMostSignificant[v];
            }
        } else {
            if (n & 0xFF00ull) {
                uint64_t v = n >> 8;
                return 8 + CByteMostSignificant[v];
            } else {
                return CByteMostSignificant[n];
            }
        }
    }
}
int CUInt32MostSignificant(uint32_t n) {
    if (0 == n) {
        return -1;
    }

    if (n & 0xFFFF0000ull) {
        if (n & 0x00FF0000ull) {
            uint32_t v = n >> 24;
            return 24 + CByteMostSignificant[v];
        } else {
            uint32_t v = n >> 16;
            return 16 + CByteMostSignificant[v];
        }
    } else {
        if (n & 0xFF00ull) {
            uint32_t v = n >> 8;
            return 8 + CByteMostSignificant[v];
        } else {
            return CByteMostSignificant[n];
        }
    }
}



int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        

//        for (uint i=0; i<256; i++) {
//            int idx = -1;
//            for (int offset=0; offset<8; offset++) {
//                if ((i >> offset) & 1) {
//                    idx = offset;
//                    break;
//                }
//            }
//            printf("%d, ", idx);
//        }
//        return 0;

        
        uint64_t items[128] = {};
        
        
        for (int i=0; i<128; i++) {
            uint64_t v = arc4random();
            v = v << 32;
            v += arc4random();
            items[i] = v;
        }
        
        uint64_t start = mach_absolute_time();
        for (int i=0; i<128; i++) {
            int r0 = highest_bit_unrolled(items[i]);
            int r1 = highest_bit_unrolled1(items[i]);
            int r2 = CUInt64MostSignificant(items[i]);
            assert(r0 == r1 && r1 == r2);
        }

        
//        uint64_t start = mach_absolute_time();
//        for (int i=0; i<128; i++) {
//            int r = highest_bit_unrolled(items[i]);
//        }
//        uint64_t end = mach_absolute_time();
//        uint64_t elapsed = end - start;
//
//        uint64_t start1 = mach_absolute_time();
//        for (int i=0; i<128; i++) {
//            int r = highest_bit_unrolled1(items[i]);
//        }
//        uint64_t end1 = mach_absolute_time();
//        uint64_t elapsed1 = end1 - start1;
//
//        uint64_t start2 = mach_absolute_time();
//        for (int i=0; i<128; i++) {
//            int r = CUInt64MostSignificant(items[i]);
//        }
//        uint64_t end2 = mach_absolute_time();
//        uint64_t elapsed2 = end2 - start2;
        
        
    }
    return 0;
}
