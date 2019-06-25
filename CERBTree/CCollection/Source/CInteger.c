//
//  CInteger.c
//  CCollection
//
//  Created by vector on 2019/6/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CInteger.h"

static const int8_t CByteMostSignificant[256] = {-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

static const int8_t CByteLeastSignificant[256] = {-1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};


static const uint8_t CByteSignificantCount[256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};


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

int CUInt64LeastSignificant(uint64_t n) {
    if (0 == n) {
        return -1;
    }
    if (n & 0xFFFFFFFFull) {
        if (n & 0xFFFFull) {
            if (n & 0xFFull) {
                uint64_t v = n & 0xFF;
                return CByteLeastSignificant[v];
            } else {
                uint64_t v = (n >> 8) & 0xFF;
                return 8 + CByteLeastSignificant[v];
            }
        } else {
            if (n & 0xFF0000ull) {
                uint64_t v = (n >> 16) & 0xFF;
                return 16 + CByteLeastSignificant[v];
            } else {
                uint64_t v = (n >> 24) & 0xFF;
                return 24 + CByteLeastSignificant[v];
            }
        }
    } else {
        if (n & 0xFFFF00000000ull) {
            if (n & 0xFF00000000ull) {
                uint64_t v = (n >> 32) & 0xFF;
                return 32 + CByteLeastSignificant[v];
            } else {
                uint64_t v = (n >> 40) & 0xFF;
                return 40 + CByteLeastSignificant[v];
            }
        } else {
            if (n & 0xFF000000000000ull) {
                uint64_t v = (n >> 48) & 0xFF;
                return 48 + CByteLeastSignificant[v];
            } else {
                uint64_t v = (n >> 56) & 0xFF;
                return 56 + CByteLeastSignificant[v];
            }
        }
    }
}

int CUInt32LeastSignificant(uint32_t n) {
    if (0 == n) {
        return -1;
    }
    
    if (n & 0xFFFF00000000ull) {
        if (n & 0xFF00000000ull) {
            uint32_t v = n & 0xFF;
            return CByteLeastSignificant[v];
        } else {
            uint64_t v = (n >> 8) & 0xFF;
            return 8 + CByteLeastSignificant[v];
        }
    } else {
        if (n & 0xFF000000000000ull) {
            uint32_t v = (n >> 16) & 0xFF;
            return 16 + CByteLeastSignificant[v];
        } else {
            uint32_t v = (n >> 24) & 0xFF;
            return 24 + CByteLeastSignificant[v];
        }
    }
}

