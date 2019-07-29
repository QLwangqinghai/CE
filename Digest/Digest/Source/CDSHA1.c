//
//  CDSHA1.c
//  Digest
//
//  Created by vector on 2019/7/29.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CoreDigest.h"


#define CDSHA1F0(x,y,z)  (z ^ (x & (y ^ z)))
#define CDSHA1F1(x,y,z)  (x ^ y ^ z)
#define CDSHA1F2(x,y,z)  ((x & y) | (z & (x | y)))
#define CDSHA1F3(x,y,z)  (x ^ y ^ z)


static inline void CDSHA1Process(uint8_t const * _Nonnull block, uint32_t currentHash[_Nonnull 5]) {
    uint32_t M[80] = {0};

    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t i;
    const uint8_t * iter = block;
    uint32_t * hh = currentHash;

    for (i = 0; i < 16; i++) {
        M[i] = CUInt32MakeWithBigEndianBytes(iter);
        iter += 4;
    }

    /* read state */
    a = hh[0];
    b = hh[1];
    c = hh[2];
    d = hh[3];
    e = hh[4];

    /* expand it */
    for (i = 16; i < 80; i++) {
        M[i] = CUInt32RotateLeft(M[i-3] ^ M[i-8] ^ M[i-14] ^ M[i-16], 1);
    }
    
    /* compress */
    /* round one */
#define CDSHA1FF0(a,b,c,d,e,i) e = (CUInt32RotateLeft(a, 5) + CDSHA1F0(b,c,d) + e + M[i] + 0x5a827999); b = CUInt32RotateLeft(b, 30);
#define CDSHA1FF1(a,b,c,d,e,i) e = (CUInt32RotateLeft(a, 5) + CDSHA1F1(b,c,d) + e + M[i] + 0x6ed9eba1); b = CUInt32RotateLeft(b, 30);
#define CDSHA1FF2(a,b,c,d,e,i) e = (CUInt32RotateLeft(a, 5) + CDSHA1F2(b,c,d) + e + M[i] + 0x8f1bbcdc); b = CUInt32RotateLeft(b, 30);
#define CDSHA1FF3(a,b,c,d,e,i) e = (CUInt32RotateLeft(a, 5) + CDSHA1F3(b,c,d) + e + M[i] + 0xca62c1d6); b = CUInt32RotateLeft(b, 30);
    
    
    for (i = 0; i < 20; ) {
        CDSHA1FF0(a,b,c,d,e,i++);
        CDSHA1FF0(e,a,b,c,d,i++);
        CDSHA1FF0(d,e,a,b,c,i++);
        CDSHA1FF0(c,d,e,a,b,i++);
        CDSHA1FF0(b,c,d,e,a,i++);
    }

    /* round two */
    for (; i < 40; )  {
        CDSHA1FF1(a,b,c,d,e,i++);
        CDSHA1FF1(e,a,b,c,d,i++);
        CDSHA1FF1(d,e,a,b,c,i++);
        CDSHA1FF1(c,d,e,a,b,i++);
        CDSHA1FF1(b,c,d,e,a,i++);
    }

    /* round three */
    for (; i < 60; )  {
        CDSHA1FF2(a,b,c,d,e,i++);
        CDSHA1FF2(e,a,b,c,d,i++);
        CDSHA1FF2(d,e,a,b,c,i++);
        CDSHA1FF2(c,d,e,a,b,i++);
        CDSHA1FF2(b,c,d,e,a,i++);
    }

    /* round four */
    for (; i < 80; )  {
        CDSHA1FF3(a,b,c,d,e,i++);
        CDSHA1FF3(e,a,b,c,d,i++);
        CDSHA1FF3(d,e,a,b,c,i++);
        CDSHA1FF3(c,d,e,a,b,i++);
        CDSHA1FF3(b,c,d,e,a,i++);
    }

//     store state 
    hh[0] += a;
    hh[1] += b;
    hh[2] += c;
    hh[3] += d;
    hh[4] += e;
}

void CDSHA1th160ContextInit(CDSHA1th160Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA1th160Context_s));
    context->digestVariant = CDVariantSHA1th160;
    context->values[0] = 0x67452301UL;
    context->values[1] = 0xefcdab89UL;
    context->values[2] = 0x98badcfeUL;
    context->values[3] = 0x10325476UL;
    context->values[4] = 0xc3d2e1f0UL;
}

void CDSHA1th160Update(CDSHA1th160Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    assert(context);
    assert(length >= 0);
    
    if (0 == length) {
        return;
    }
    assert(bytes);
    
    size_t blockSize = CDVariantSHA1th160BlockSize;
    uint8_t const * ptr = bytes;
    if (context->accumulatedSize > 0) {
        size_t missingLength = blockSize - context->accumulatedSize;
        if (length < missingLength) {
            memcpy(context->accumulated, ptr, length);
            context->accumulatedSize += length;
            return;
        } else {
            memcpy(context->accumulated, ptr, missingLength);
            CDSHA1Process(context->accumulated, context->values);
            context->accumulatedSize = 0;
            ptr += missingLength;
            length -= missingLength;
            context->bitCount += blockSize * 8;
        }
    }
    uint64_t const mask = 0xFFFFFFFFFFFFFFF8ULL;
    uint64_t bitCount = length;
    bitCount = bitCount & mask;
    bitCount = bitCount << 3;
    context->bitCount += bitCount;
    
    for (; length >= blockSize; length -= blockSize) {
        CDSHA1Process(ptr, context->values);
        ptr += blockSize;
    }
    
    if (length > 0) {
        memcpy(context->accumulated, ptr, length);
        context->accumulatedSize += length;
    }
}

void CDSHA1th160Final(CDSHA1th160Context_s * _Nonnull context) {
    assert(context);
    
    size_t blockSize = CDVariantSHA1th160BlockSize;
    
    size_t size = context->accumulatedSize;
    uint8_t bytes[CDVariantSHA1th160BlockSize * 2] = {};
    
    uint8_t * ptr = bytes;
    if (context->accumulatedSize > 0) {
        memcpy(bytes, context->accumulated, context->accumulatedSize);
        ptr += context->accumulatedSize;
        context->accumulatedSize = 0;
    }
    context->bitCount += context->accumulatedSize * 8;
    
    // Step 1. Append Padding Bits
    // append one bit (UInt8 with one bit) to message
    *ptr = 0x80;
    ptr += 1;
    size += 1;
    
    // Step 2. append "0" bit until message length in bits ≡ 448 (mod 512)
    size_t max = blockSize - blockSize / 8;//56
    if (size % blockSize < max) {
        ptr = bytes + max;
        
        CUInt64ToBigEndianBytes(context->bitCount, ptr);
        CDSHA1Process(bytes, context->values);
    } else {
        ptr = bytes + blockSize + max;
        CUInt64ToBigEndianBytes(context->bitCount, ptr);
        CDSHA1Process(bytes, context->values);
        CDSHA1Process(bytes + blockSize, context->values);
    }
}

void CDSHA1th160ExportHashValue(CDSHA1th160Context_s * _Nonnull context, uint8_t bytes[_Nonnull 20]) {
    assert(context);
    assert(bytes);
    
    uint8_t * ptr = bytes;
    for (size_t idx=0; idx<5; idx++) {
        uint32_t v = context->values[idx];
        CUInt32ToBigEndianBytes(v, ptr);
        ptr += 4;
    }
}
