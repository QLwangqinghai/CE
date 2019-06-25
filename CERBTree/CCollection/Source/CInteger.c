//
//  CInteger.c
//  CCollection
//
//  Created by vector on 2019/6/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CInteger.h"


int highest_bit_unrolled(uint64_t n)
{
    if (n & 0xFFFFFFFF00000000ull) {
        if (n & 0xFFFF000000000000ull) {
            if (n & 0xFF00000000000000ull) {
                if (n & 0xF000000000000000ull) {
                    if (n & 0xC000000000000000ull)
                        return (n & 0x8000000000000000ull) ? 64 : 63;
                    else
                        return (n & 0x2000000000000000ull) ? 62 : 61;
                } else {
                    if (n & 0x0C00000000000000ull)
                        return (n & 0x0800000000000000ull) ? 60 : 59;
                    else
                        return (n & 0x0200000000000000ull) ? 58 : 57;
                }
            } else {
                if (n & 0x00F0000000000000ull) {
                    if (n & 0x00C0000000000000ull)
                        return (n & 0x0080000000000000ull) ? 56 : 55;
                    else
                        return (n & 0x0020000000000000ull) ? 54 : 53;
                } else {
                    if (n & 0x000C000000000000)
                        return (n & 0x0008000000000000ull) ? 52 : 51;
                    else
                        return (n & 0x0002000000000000ull) ? 50 : 49;
                }
            }
        } else {
            if (n & 0x0000FF0000000000ull) {
                if (n & 0x0000F00000000000ull) {
                    if (n & 0x0000C00000000000ull)
                        return (n & 0x0000800000000000ull) ? 48 : 47;
                    else
                        return (n & 0x0000200000000000ull) ? 46 : 45;
                } else {
                    if (n & 0x00000C0000000000)
                        return (n & 0x0000080000000000ull) ? 44 : 43;
                    else
                        return (n & 0x0000020000000000ull) ? 42 : 41;
                }
            } else {
                if (n & 0x000000F000000000ull) {
                    if (n & 0x000000C000000000ull)
                        return (n & 0x0000008000000000ull) ? 40 : 39;
                    else
                        return (n & 0x0000002000000000ull) ? 38 : 37;
                } else {
                    if (n & 0x0000000C00000000ull)
                        return (n & 0x0000000800000000ull) ? 36 : 35;
                    else
                        return (n & 0x0000000200000000ull) ? 34 : 33;
                }
            }
        }
    } else {
        if (n & 0x00000000FFFF0000ull) {
            if (n & 0x00000000FF000000ull) {
                if (n & 0x00000000F0000000ull) {
                    if (n & 0x00000000C0000000ull)
                        return (n & 0x0000000080000000ull) ? 32 : 31;
                    else
                        return (n & 0x0000000020000000ull) ? 30 : 29;
                } else {
                    if (n & 0x000000000C000000ull)
                        return (n & 0x0000000008000000ull) ? 28 : 27;
                    else
                        return (n & 0x0000000002000000ull) ? 26 : 25;
                }
            } else {
                if (n & 0x0000000000F00000ull) {
                    if (n & 0x0000000000C00000ull)
                        return (n & 0x0000000000800000ull) ? 24 : 23;
                    else
                        return (n & 0x0000000000200000ull) ? 22 : 21;
                } else {
                    if (n & 0x00000000000C0000ull)
                        return (n & 0x0000000000080000ull) ? 20 : 19;
                    else
                        return (n & 0x0000000000020000ull) ? 18 : 17;
                }
            }
        } else {
            if (n & 0x000000000000FF00ull) {
                if (n & 0x000000000000F000ull) {
                    if (n & 0x000000000000C000ull)
                        return (n & 0x0000000000008000ull) ? 16 : 15;
                    else
                        return (n & 0x0000000000002000ull) ? 14 : 13;
                } else {
                    if (n & 0x0000000000000C00ull)
                        return (n & 0x0000000000000800ull) ? 12 : 11;
                    else
                        return (n & 0x0000000000000200ull) ? 10 : 9;
                }
            } else {
                if (n & 0x00000000000000F0ull) {
                    if (n & 0x00000000000000C0ull)
                        return (n & 0x0000000000000080ull) ? 8 : 7;
                    else
                        return (n & 0x0000000000000020ull) ? 6 : 5;
                } else {
                    if (n & 0x000000000000000Cull)
                        return (n & 0x0000000000000008ull) ? 4 : 3;
                    else
                        return (n & 0x0000000000000002ull) ? 2 : (n ? 1 : 0);
                }
            }
        }
    }
}

int highest_bit(long long n)
{
    const long long mask[] = {
        0x000000007FFFFFFF,
        0x000000000000FFFF,
        0x00000000000000FF,
        0x000000000000000F,
        0x0000000000000003,
        0x0000000000000001
    };
    int hi = 64;
    int lo = 0;
    int i = 0;
    
    if (n == 0)
        return 0;
    
    for (i = 0; i < sizeof mask / sizeof mask[0]; i++) {
        int mi = lo + (hi - lo) / 2;
        
        if ((n >> mi) != 0)
            lo = mi;
        else if ((n & (mask[i] << lo)) != 0)
            hi = mi;
    }
    
    return lo + 1;
}
