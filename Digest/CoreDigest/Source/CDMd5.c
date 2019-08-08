//
//  CDMd5.c
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CDAPI.h"

#define F(x,y,z)  (z ^ (x & (y ^ z)))
#define G(x,y,z)  (y ^ (z & (y ^ x)))
#define H(x,y,z)  (x^y^z)
#define I(x,y,z)  (y^(x|(~z)))


#define FF(a,b,c,d,M,s,t) \
a = (a + F(b,c,d) + M + t); a = CUInt32RotateLeft(a, s) + b;

#define GG(a,b,c,d,M,s,t) \
a = (a + G(b,c,d) + M + t); a = CUInt32RotateLeft(a, s) + b;

#define HH(a,b,c,d,M,s,t) \
a = (a + H(b,c,d) + M + t); a = CUInt32RotateLeft(a, s) + b;

#define II(a,b,c,d,M,s,t) \
a = (a + I(b,c,d) + M + t); a = CUInt32RotateLeft(a, s) + b;


void CDMD5Process(void * _Nonnull currentHash, size_t blockCount, const void * _Nonnull in) {
    uint32_t i, W[16], a, b, c, d;
    uint32_t * s = currentHash;
    const uint8_t * ptr = in;
    
    while (blockCount--) {
        /* copy the state into 512-bits into W[0..15] */
        for (i = 0; i < 16; i++) {
            W[i] = CUInt32MakeWithLittleEndianBytes(ptr);
            ptr += 4;
        }
        
        /* copy state */
        a = s[0];
        b = s[1];
        c = s[2];
        d = s[3];
        
        FF(a,b,c,d,W[0],7,0xd76aa478)
        FF(d,a,b,c,W[1],12,0xe8c7b756)
        FF(c,d,a,b,W[2],17,0x242070db)
        FF(b,c,d,a,W[3],22,0xc1bdceee)
        FF(a,b,c,d,W[4],7,0xf57c0faf)
        FF(d,a,b,c,W[5],12,0x4787c62a)
        FF(c,d,a,b,W[6],17,0xa8304613)
        FF(b,c,d,a,W[7],22,0xfd469501)
        FF(a,b,c,d,W[8],7,0x698098d8)
        FF(d,a,b,c,W[9],12,0x8b44f7af)
        FF(c,d,a,b,W[10],17,0xffff5bb1)
        FF(b,c,d,a,W[11],22,0x895cd7be)
        FF(a,b,c,d,W[12],7,0x6b901122)
        FF(d,a,b,c,W[13],12,0xfd987193)
        FF(c,d,a,b,W[14],17,0xa679438e)
        FF(b,c,d,a,W[15],22,0x49b40821)
        GG(a,b,c,d,W[1],5,0xf61e2562)
        GG(d,a,b,c,W[6],9,0xc040b340)
        GG(c,d,a,b,W[11],14,0x265e5a51)
        GG(b,c,d,a,W[0],20,0xe9b6c7aa)
        GG(a,b,c,d,W[5],5,0xd62f105d)
        GG(d,a,b,c,W[10],9,0x02441453)
        GG(c,d,a,b,W[15],14,0xd8a1e681)
        GG(b,c,d,a,W[4],20,0xe7d3fbc8)
        GG(a,b,c,d,W[9],5,0x21e1cde6)
        GG(d,a,b,c,W[14],9,0xc33707d6)
        GG(c,d,a,b,W[3],14,0xf4d50d87)
        GG(b,c,d,a,W[8],20,0x455a14ed)
        GG(a,b,c,d,W[13],5,0xa9e3e905)
        GG(d,a,b,c,W[2],9,0xfcefa3f8)
        GG(c,d,a,b,W[7],14,0x676f02d9)
        GG(b,c,d,a,W[12],20,0x8d2a4c8a)
        HH(a,b,c,d,W[5],4,0xfffa3942)
        HH(d,a,b,c,W[8],11,0x8771f681)
        HH(c,d,a,b,W[11],16,0x6d9d6122)
        HH(b,c,d,a,W[14],23,0xfde5380c)
        HH(a,b,c,d,W[1],4,0xa4beea44)
        HH(d,a,b,c,W[4],11,0x4bdecfa9)
        HH(c,d,a,b,W[7],16,0xf6bb4b60)
        HH(b,c,d,a,W[10],23,0xbebfbc70)
        HH(a,b,c,d,W[13],4,0x289b7ec6)
        HH(d,a,b,c,W[0],11,0xeaa127fa)
        HH(c,d,a,b,W[3],16,0xd4ef3085)
        HH(b,c,d,a,W[6],23,0x04881d05)
        HH(a,b,c,d,W[9],4,0xd9d4d039)
        HH(d,a,b,c,W[12],11,0xe6db99e5)
        HH(c,d,a,b,W[15],16,0x1fa27cf8)
        HH(b,c,d,a,W[2],23,0xc4ac5665)
        II(a,b,c,d,W[0],6,0xf4292244)
        II(d,a,b,c,W[7],10,0x432aff97)
        II(c,d,a,b,W[14],15,0xab9423a7)
        II(b,c,d,a,W[5],21,0xfc93a039)
        II(a,b,c,d,W[12],6,0x655b59c3)
        II(d,a,b,c,W[3],10,0x8f0ccc92)
        II(c,d,a,b,W[10],15,0xffeff47d)
        II(b,c,d,a,W[1],21,0x85845dd1)
        II(a,b,c,d,W[8],6,0x6fa87e4f)
        II(d,a,b,c,W[15],10,0xfe2ce6e0)
        II(c,d,a,b,W[6],15,0xa3014314)
        II(b,c,d,a,W[13],21,0x4e0811a1)
        II(a,b,c,d,W[4],6,0xf7537e82)
        II(d,a,b,c,W[11],10,0xbd3af235)
        II(c,d,a,b,W[2],15,0x2ad7d2bb)
        II(b,c,d,a,W[9],21,0xeb86d391)
        
        /* store state */
        s[0] += a;
        s[1] += b;
        s[2] += c;
        s[3] += d;
    }
}

void CDMD5ContextInit(CDMD5Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDMD5Context_s));
    context->digestVariant = CCDigestTypeMd5;
    context->values[0] = 0x67452301UL;
    context->values[1] = 0xefcdab89UL;
    context->values[2] = 0x98badcfeUL;
    context->values[3] = 0x10325476UL;
}


void CDMD5Final(CDMD5Context_s * _Nonnull context) {
    assert(context);
    
    size_t blockSize = CCDigestMd5BlockSize;
    
    size_t size = context->accumulatedSize;
    uint8_t bytes[CCDigestMd5BlockSize * 2] = {};
    
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
        
        CUInt64ToLittleEndianBytes(context->bitCount, ptr);
        CDMD5Process(context->values, 1, bytes);
    } else {
        ptr = bytes + blockSize + max;
        CUInt64ToLittleEndianBytes(context->bitCount, ptr);
        CDMD5Process(context->values, 2, bytes);
    }
}

void CDMD5Update(CDMD5Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    assert(context);
    assert(length >= 0);
    
    if (0 == length) {
        return;
    }
    assert(bytes);
    
    size_t blockSize = CCDigestMd5BlockSize;
    uint8_t const * ptr = bytes;
    if (context->accumulatedSize > 0) {
        size_t missingLength = blockSize - context->accumulatedSize;
        if (length < missingLength) {
            memcpy(context->accumulated, ptr, length);
            context->accumulatedSize += length;
            return;
        } else {
            memcpy(context->accumulated, ptr, missingLength);
            CDMD5Process(context->values, 1, context->accumulated);
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
    
    size_t blockCount = length >> 6;
    if (blockCount > 0) {
        CDMD5Process(context->values, blockCount, ptr);
        ptr += (blockCount << 6);
    }
    length = length % CCDigestMd5BlockSize;

    if (length > 0) {
        memcpy(context->accumulated, ptr, length);
        context->accumulatedSize += length;
    }
}


void CDMD5ExportHashValue(CDMD5Context_s * _Nonnull context, uint8_t bytes[_Nonnull 16]) {
    assert(context);
    assert(bytes);
    
    uint8_t * ptr = bytes;
    for (size_t idx=0; idx<4; idx++) {
        uint32_t v = context->values[idx];
        CUInt32ToLittleEndianBytes(v, ptr);
        ptr += 4;
    }
}

void CCDigestSetupMd5(CCDigestContext_s * _Nonnull context, void * _Nonnull states, uint8_t * _Nonnull accumulated) {
    assert(context);
    assert(states);
    assert(accumulated);
    memset(context, 0, sizeof(CCDigestContext_s));
    context->digestType = CCDigestTypeMd5;
    
}


void CCDigestContextInitMd5(CCDigestContext_s * _Nonnull context, void * _Nonnull states, uint8_t * _Nonnull accumulated) {
    assert(context);
    assert(states);
    assert(accumulated);
    memset(context, 0, sizeof(CCDigestContext_s));
    context->digestType = CCDigestTypeMd5;

}
//void CCDigestContextInitSha1(CCDigestContext_s * _Nonnull context, void * _Nonnull states, uint8_t * _Nonnull accumulated);
//void CCDigestContextInitSha2(CCDigestContext_s * _Nonnull context, CCDigestType_e type, void * _Nonnull states, uint8_t * _Nonnull accumulated);
//void CCDigestContextInitSha3(CCDigestContext_s * _Nonnull context, CCDigestType_e type, void * _Nonnull states, uint8_t * _Nonnull accumulated);
