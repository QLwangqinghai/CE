//
//  CDMD.c
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CoreDigest.h"

void md5_compress(const void *in, uint32_t currentHash[_Nonnull 4], size_t nblocks);



static inline void CDMD5Process(uint8_t const block[_Nonnull 64], uint32_t currentHash[_Nonnull 4]) {
    size_t const kCount = 64;
    
    static const uint32_t k[kCount] = {
        0xd76aa478UL, 0xe8c7b756UL, 0x242070dbUL, 0xc1bdceeeUL, 0xf57c0fafUL, 0x4787c62aUL, 0xa8304613UL, 0xfd469501UL,
        0x698098d8UL, 0x8b44f7afUL, 0xffff5bb1UL, 0x895cd7beUL, 0x6b901122UL, 0xfd987193UL, 0xa679438eUL, 0x49b40821UL,
        0xf61e2562UL, 0xc040b340UL, 0x265e5a51UL, 0xe9b6c7aaUL, 0xd62f105dUL, 0x2441453UL, 0xd8a1e681UL, 0xe7d3fbc8UL,
        0x21e1cde6UL, 0xc33707d6UL, 0xf4d50d87UL, 0x455a14edUL, 0xa9e3e905UL, 0xfcefa3f8UL, 0x676f02d9UL, 0x8d2a4c8aUL,
        0xfffa3942UL, 0x8771f681UL, 0x6d9d6122UL, 0xfde5380cUL, 0xa4beea44UL, 0x4bdecfa9UL, 0xf6bb4b60UL, 0xbebfbc70UL,
        0x289b7ec6UL, 0xeaa127faUL, 0xd4ef3085UL, 0x4881d05UL, 0xd9d4d039UL, 0xe6db99e5UL, 0x1fa27cf8UL, 0xc4ac5665UL,
        0xf4292244UL, 0x432aff97UL, 0xab9423a7UL, 0xfc93a039UL, 0x655b59c3UL, 0x8f0ccc92UL, 0xffeff47dUL, 0x85845dd1UL,
        0x6fa87e4fUL, 0xfe2ce6e0UL, 0xa3014314UL, 0x4e0811a1UL, 0xf7537e82UL, 0xbd3af235UL, 0x2ad7d2bbUL, 0xeb86d391UL,
    };

    static const uint32_t s[kCount] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
    };
    uint32_t * hh = currentHash;
    uint32_t A = hh[0];
    uint32_t B = hh[1];
    uint32_t C = hh[2];
    uint32_t D = hh[3];
    
    uint32_t dTemp = 0;
    uint32_t F = 0;
    uint32_t j = 0;
    for (; j<16; j++) {
        F = (B & C) | ((~B) & D);
        
        dTemp = D;
        D = C;
        C = B;
        
        // break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15 and get M[g] value
        uint32_t gAdvanced = j << 2;
        
        uint32_t Mg = CUInt32MakeWithLittleEndianBytes(&block[gAdvanced]);

        B = B + CUInt32RotateLeft(A + F + k[j] + Mg, s[j]);
        A = dTemp;
    }
    
    for (; j<32; j++) {
        F = (D & B) | (~D & C);
        
        dTemp = D;
        D = C;
        C = B;
        // break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15 and get M[g] value
        uint32_t gAdvanced = ((5 * j + 1) % 16) << 2;
        
        uint32_t Mg = CUInt32MakeWithLittleEndianBytes(&block[gAdvanced]);
        
        B = B + CUInt32RotateLeft(A + F + k[j] + Mg, s[j]);
        A = dTemp;
    }
    for (; j<48; j++) {
        F = B ^ C ^ D;
        
        dTemp = D;
        D = C;
        C = B;
        // break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15 and get M[g] value
        uint32_t gAdvanced = ((3 * j + 5) % 16) << 2;
        
        uint32_t Mg = CUInt32MakeWithLittleEndianBytes(&block[gAdvanced]);

        B = B + CUInt32RotateLeft(A + F + k[j] + Mg, s[j]);
        A = dTemp;
    }
    for (; j<64; j++) {
        F = C ^ (B | (~D));

        dTemp = D;
        D = C;
        C = B;
        // break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15 and get M[g] value
        uint32_t gAdvanced = ((7 * j) % 16) << 2;
        
        uint32_t Mg = CUInt32MakeWithLittleEndianBytes(&block[gAdvanced]);

        B = B + CUInt32RotateLeft(A + F + k[j] + Mg, s[j]);
        A = dTemp;
    }

    
    hh[0] = hh[0] + A;
    hh[1] = hh[1] + B;
    hh[2] = hh[2] + C;
    hh[3] = hh[3] + D;
}

void CDMD5ContextInit(CDMD5Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDMD5Context_s));
    context->digestVariant = CDVariantMD5;
    context->values[0] = 0x67452301UL;
    context->values[1] = 0xefcdab89UL;
    context->values[2] = 0x98badcfeUL;
    context->values[3] = 0x10325476UL;
}


void CDMD5Final(CDMD5Context_s * _Nonnull context) {
    assert(context);
    
    size_t blockSize = CDVariantMD5BlockSize;
    
    size_t size = context->accumulatedSize;
    uint8_t bytes[CDVariantMD5BlockSize * 2] = {};
    
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
        CDMD5Process(bytes, context->values);
    } else {
        ptr = bytes + blockSize + max;
        CUInt64ToLittleEndianBytes(context->bitCount, ptr);
        CDMD5Process(bytes, context->values);
        CDMD5Process(bytes + blockSize, context->values);
    }
}

void CDMD5Update2(CDMD5Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    assert(context);
    assert(length >= 0);
    
    if (0 == length) {
        return;
    }
    assert(bytes);
    
    size_t blockSize = CDVariantMD5BlockSize;
    uint8_t const * ptr = bytes;
    if (context->accumulatedSize > 0) {
        size_t missingLength = blockSize - context->accumulatedSize;
        if (length < missingLength) {
            memcpy(context->accumulated, ptr, length);
            context->accumulatedSize += length;
            return;
        } else {
            memcpy(context->accumulated, ptr, missingLength);
            md5_compress(context->accumulated, context->values, 1);
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
    
    md5_compress(ptr, context->values, length / blockSize);
    ptr += length / blockSize * blockSize;
    length -= length / blockSize * blockSize;
    
    if (length > 0) {
        memcpy(context->accumulated, ptr, length);
        context->accumulatedSize += length;
    }
}
void CDMD5Update(CDMD5Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    assert(context);
    assert(length >= 0);
    
    if (0 == length) {
        return;
    }
    assert(bytes);
    
    size_t blockSize = CDVariantMD5BlockSize;
    uint8_t const * ptr = bytes;
    if (context->accumulatedSize > 0) {
        size_t missingLength = blockSize - context->accumulatedSize;
        if (length < missingLength) {
            memcpy(context->accumulated, ptr, length);
            context->accumulatedSize += length;
            return;
        } else {
            memcpy(context->accumulated, ptr, missingLength);
            CDMD5Process(context->accumulated, context->values);
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
        CDMD5Process(ptr, context->values);
        ptr += blockSize;
    }
    
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










#define CCMD5_BLOCK_SIZE   64
#define CCMD5_OUTPUT_SIZE  16
#define CCMD5_STATE_SIZE   16


typedef size_t  cc_size;
typedef uint64_t cc_unit;

typedef cc_unit (*addsub_t)(cc_size count, cc_unit *r, const cc_unit *s, const cc_unit *t);


#if defined(_MSC_VER)
#if defined(__clang__)
#define CC_ALIGNED(x) __attribute__ ((aligned(x))) //clang compiler
#else
#define CC_ALIGNED(x) __declspec(align(x)) //MS complier
#endif
#else
#if __clang__ || CCN_UNIT_SIZE==8
#define CC_ALIGNED(x) __attribute__ ((aligned(x)))
#else
#define CC_ALIGNED(x) __attribute__ ((aligned((x)>8?8:(x))))
#endif
#endif

struct ccdigest_ctx {
    union {
        uint8_t u8;
        uint32_t u32;
        uint64_t u64;
        cc_unit ccn;
    } state;
} CC_ALIGNED(8);

typedef union {
    struct ccdigest_ctx *hdr;
} ccdigest_ctx_t __attribute__((transparent_union));

struct ccdigest_state {
    union {
        uint8_t u8;
        uint32_t u32;
        uint64_t u64;
        cc_unit ccn;
    } state;
} CC_ALIGNED(8);
typedef union {
    struct ccdigest_state *hdr;
    struct ccdigest_ctx *_ctx;
    ccdigest_ctx_t _ctxt;
} ccdigest_state_t __attribute__((transparent_union));


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

#define ccdigest_u32(_state_)            (&((ccdigest_state_t)(_state_)).hdr->state.u32)


void md5_compress(const void *in, uint32_t currentHash[_Nonnull 4], size_t nblocks) {
    uint32_t i, W[16], a, b, c, d;
    uint32_t *s = currentHash;
    const unsigned char *buf = in;
    
    while(nblocks--) {
        
        /* copy the state into 512-bits into W[0..15] */
        for (i = 0; i < 16; i++) {
            W[i] = CUInt32MakeWithLittleEndianBytes(buf + (4*i));
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
        
        buf+=CCMD5_BLOCK_SIZE;
    }
}
