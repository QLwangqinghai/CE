//
//  CDSHA2.c
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CDAPI.h"

#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))

static inline void CDSHA2Process64(const uint8_t block[_Nonnull 128], uint64_t currentHash[_Nonnull 8]) {
    // break chunk into sixteen 64-bit words M[j], 0 ≤ j ≤ 15, big-endian
    // Extend the sixteen 64-bit words into eighty 64-bit words:
    size_t const kCount = 80;
    
    static const uint64_t k[kCount] = {
        0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
        0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
        0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
        0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
        0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
        0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
        0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
        0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
        0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
        0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
        0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
        0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
        0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
        0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
        0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
        0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL,
    };
    
    uint64_t M[kCount] = {0};
    uint64_t * hh = currentHash;
    uint8_t const * iter = block;
    size_t idx = 0;
    for (; idx<16; idx++) {
        M[idx] = CUInt64MakeWithBigEndianBytes(iter);
        iter += 8;
    }
    
#define CDSHA2th512Gamma0(x)       (CUInt64RotateRight(x, 1) ^ CUInt64RotateRight(x, 8) ^ (x >> 7))
#define CDSHA2th512Gamma1(x)       (CUInt64RotateRight(x, 19) ^ CUInt64RotateRight(x, 61) ^ (x >> 6))

    
    for (; idx<kCount; idx++) {
        M[idx] = CDSHA2th512Gamma1(M[idx - 2]) + M[idx - 7] + CDSHA2th512Gamma0(M[idx - 15]) + M[idx - 16];
    }
    
    uint64_t S[8];
    for (int i = 0; i < 8; i++) {
        S[i] = hh[i];
    }
    
    uint64_t t0, t1;
    
#define CDSHA2th512RND(a,b,c,d,e,f,g,h,i)                    \
t0 = h + (CUInt64RotateRight(e, 14) ^ CUInt64RotateRight(e, 18) ^ CUInt64RotateRight(e, 41)) + Ch(e, f, g) + k[i] + M[i];   \
t1 = (CUInt64RotateRight(a, 28) ^ CUInt64RotateRight(a, 34) ^ CUInt64RotateRight(a, 39)) + Maj(a, b, c);                  \
d += t0;                                        \
h  = t0 + t1;

    for (size_t i = 0; i < 80; i += 8) {
        CDSHA2th512RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i+0);
        CDSHA2th512RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],i+1);
        CDSHA2th512RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],i+2);
        CDSHA2th512RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],i+3);
        CDSHA2th512RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],i+4);
        CDSHA2th512RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],i+5);
        CDSHA2th512RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],i+6);
        CDSHA2th512RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],i+7);
    }
    
    for (int i = 0; i < 8; i++) {
        hh[i] = hh[i] + S[i];
    }
}

// mutating currentHash in place is way faster than returning new result
static inline void CDSHA2Process32(uint8_t const block[_Nonnull 64], uint32_t currentHash[_Nonnull 8]) {
    // break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15, big-endian
    // Extend the sixteen 32-bit words into sixty-four 32-bit words:
    size_t const kCount = 64;
    
    static const uint32_t k[kCount] = {
        0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
        0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
        0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
        0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
        0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
        0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
        0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
        0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL,
    };
    uint32_t M[kCount] = {0};
    uint32_t * hh = currentHash;
    uint8_t const * iter = block;
    size_t idx = 0;
    for (; idx<16; idx++) {
        M[idx] = CUInt32MakeWithBigEndianBytes(iter);
        iter += 4;
    }
    
#define CDSHA2th256Gamma0(x)       (CUInt32RotateRight(x, 7) ^ CUInt32RotateRight(x, 18) ^ (x >> 3))
#define CDSHA2th256Gamma1(x)       (CUInt32RotateRight(x, 17) ^ CUInt32RotateRight(x, 19) ^ (x >> 10))
    
    for (; idx<kCount; idx++) {
        M[idx] = CDSHA2th256Gamma1(M[idx - 2]) + M[idx - 7] + CDSHA2th256Gamma0(M[idx - 15]) + M[idx - 16];
    }
    
    
#define CDSHA2th256RND(a,b,c,d,e,f,g,h,i)                    \
t0 = h + (CUInt32RotateRight(e, 6) ^ CUInt32RotateRight(e, 11) ^ CUInt32RotateRight(e, 25)) + Ch(e, f, g) + k[i] + M[i];   \
t1 = (CUInt32RotateRight(a, 2) ^ CUInt32RotateRight(a, 13) ^ CUInt32RotateRight(a, 22)) + Maj(a, b, c);                  \
d += t0;                                        \
h  = t0 + t1;

    
    uint32_t A = hh[0];
    uint32_t B = hh[1];
    uint32_t C = hh[2];
    uint32_t D = hh[3];
    uint32_t E = hh[4];
    uint32_t F = hh[5];
    uint32_t G = hh[6];
    uint32_t H = hh[7];
    uint32_t t0, t1;

    // Main loop
    for (uint32_t i=0; i<kCount; i+=8) {
        CDSHA2th256RND(A,B,C,D,E,F,G,H,i+0);
        CDSHA2th256RND(H,A,B,C,D,E,F,G,i+1);
        CDSHA2th256RND(G,H,A,B,C,D,E,F,i+2);
        CDSHA2th256RND(F,G,H,A,B,C,D,E,i+3);
        CDSHA2th256RND(E,F,G,H,A,B,C,D,i+4);
        CDSHA2th256RND(D,E,F,G,H,A,B,C,i+5);
        CDSHA2th256RND(C,D,E,F,G,H,A,B,i+6);
        CDSHA2th256RND(B,C,D,E,F,G,H,A,i+7);
    }
    
    hh[0] = (hh[0] + A);
    hh[1] = (hh[1] + B);
    hh[2] = (hh[2] + C);
    hh[3] = (hh[3] + D);
    hh[4] = (hh[4] + E);
    hh[5] = (hh[5] + F);
    hh[6] = (hh[6] + G);
    hh[7] = (hh[7] + H);
}

static inline void _CDSHA2th224ContextSetHashInitialValue(uint32_t values[_Nonnull 8]) {
    assert(values);
    values[0] = 0xc1059ed8UL;
    values[1] = 0x367cd507UL;
    values[2] = 0x3070dd17UL;
    values[3] = 0xf70e5939UL;
    values[4] = 0xffc00b31UL;
    values[5] = 0x68581511UL;
    values[6] = 0x64f98fa7UL;
    values[7] = 0xbefa4fa4UL;
}
static inline void _CDSHA2th256ContextSetHashInitialValue(uint32_t values[_Nonnull 8]) {
    assert(values);
    values[0] = 0x6a09e667UL;
    values[1] = 0xbb67ae85UL;
    values[2] = 0x3c6ef372UL;
    values[3] = 0xa54ff53aUL;
    values[4] = 0x510e527fUL;
    values[5] = 0x9b05688cUL;
    values[6] = 0x1f83d9abUL;
    values[7] = 0x5be0cd19UL;
}

static inline void _CDSHA2th384ContextSetHashInitialValue(uint64_t values[_Nonnull 8]) {
    assert(values);
    values[0] = 0xcbbb9d5dc1059ed8ULL;
    values[1] = 0x629a292a367cd507ULL;
    values[2] = 0x9159015a3070dd17ULL;
    values[3] = 0x152fecd8f70e5939ULL;
    values[4] = 0x67332667ffc00b31ULL;
    values[5] = 0x8eb44a8768581511ULL;
    values[6] = 0xdb0c2e0d64f98fa7ULL;
    values[7] = 0x47b5481dbefa4fa4ULL;
}
static inline void _CDSHA2th512ContextSetHashInitialValue(uint64_t values[_Nonnull 8]) {
    assert(values);
    values[0] = 0x6a09e667f3bcc908ULL;
    values[1] = 0xbb67ae8584caa73bULL;
    values[2] = 0x3c6ef372fe94f82bULL;
    values[3] = 0xa54ff53a5f1d36f1ULL;
    values[4] = 0x510e527fade682d1ULL;
    values[5] = 0x9b05688c2b3e6c1fULL;
    values[6] = 0x1f83d9abfb41bd6bULL;
    values[7] = 0x5be0cd19137e2179ULL;
}

void CDSHA2ContextInit(CDSHA2Context_s * _Nonnull context, CDVariant_e e) {
    assert(context);
    memset(context, 0, sizeof(CDSHA2Context_s));
    context->digestVariant = e;
    
    switch (e) {
        case CDVariantSHA2th224:
            _CDSHA2th224ContextSetHashInitialValue(context->states.u32Values);
            break;
        case CDVariantSHA2th256:
            _CDSHA2th256ContextSetHashInitialValue(context->states.u32Values);
            break;
        case CDVariantSHA2th384:
            _CDSHA2th384ContextSetHashInitialValue(context->states.u64Values);
            break;
        case CDVariantSHA2th512:
            _CDSHA2th512ContextSetHashInitialValue(context->states.u64Values);
            break;
        default:
            abort();
            break;
    }
}



static inline void _CDSHA2Update256(CDSHA2Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    assert(context);
    assert(length >= 0);
    
    if (0 == length) {
        return;
    }
    assert(bytes);
    
    size_t blockSize = CDVariantSHA2th256BlockSize;
    uint8_t const * ptr = bytes;
    if (context->accumulatedSize > 0) {
        size_t missingLength = blockSize - context->accumulatedSize;
        if (length < missingLength) {
            memcpy(context->accumulated, ptr, length);
            context->accumulatedSize += length;
            return;
        } else {
            memcpy(context->accumulated, ptr, missingLength);
            CDSHA2Process32(context->accumulated, context->states.u32Values);
            context->accumulatedSize = 0;
            ptr += missingLength;
            length -= missingLength;
            context->bitCountLow += blockSize * 8;
        }
    }
    uint64_t const mask = 0xFFFFFFFFFFFFFFF8ULL;
    uint64_t bitCount = length;
    bitCount = bitCount & mask;
    bitCount = bitCount << 3;
    context->bitCountLow += bitCount;
    
    for (; length >= blockSize; length -= blockSize) {
        CDSHA2Process32(ptr, context->states.u32Values);
        ptr += blockSize;
    }
    
    if (length > 0) {
        memcpy(context->accumulated, ptr, length);
        context->accumulatedSize += length;
    }
}


static inline void _CDSHA2Update512(CDSHA2Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    assert(context);
    assert(length >= 0);
    
    if (0 == length) {
        return;
    }
    assert(bytes);
    
    size_t blockSize = CDVariantSHA2th512BlockSize;
    uint8_t const * ptr = bytes;
    if (context->accumulatedSize > 0) {
        size_t missingLength = blockSize - context->accumulatedSize;
        if (length < missingLength) {
            memcpy(context->accumulated, ptr, length);
            context->accumulatedSize += length;
            return;
        } else {
            memcpy(context->accumulated, ptr, missingLength);
            CDSHA2Process64(context->accumulated, context->states.u64Values);
            context->accumulatedSize = 0;
            ptr += missingLength;
            length -= missingLength;
            
            uint64_t bitCountLow = context->bitCountLow + blockSize * 8;
            if (bitCountLow < context->bitCountLow) {
                context->bitCountHigh += 1;
            }
            context->bitCountLow = bitCountLow;
        }
    }
    uint64_t const mask = 0xFFFFFFFFFFFFFFF8ULL;
    uint64_t bitCount = length;
    bitCount = bitCount & mask;
    context->bitCountHigh += (bitCount >> 61);
    bitCount = bitCount << 3;
    
    uint64_t bitCountLow = context->bitCountLow + bitCount;
    if (bitCountLow < context->bitCountLow) {
        context->bitCountHigh += 1;
    }
    context->bitCountLow = bitCountLow;
    
    for (; length >= blockSize; length -= blockSize) {
        CDSHA2Process64(ptr, context->states.u64Values);
        ptr += blockSize;
    }
    
    if (length > 0) {
        memcpy(context->accumulated, ptr, length);
        context->accumulatedSize += length;
    }
}
void CDSHA2Update(CDSHA2Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    switch (context->digestVariant) {
        case CDVariantSHA2th224:
        case CDVariantSHA2th256:
            _CDSHA2Update256(context, bytes, length);
            break;
        case CDVariantSHA2th384:
        case CDVariantSHA2th512:
            _CDSHA2Update512(context, bytes, length);
            break;
        default:
            abort();
            break;
    }
}



static inline void _CDSHA2Final256(CDSHA2Context_s * _Nonnull context) {
    assert(context);
    
    size_t blockSize = CDVariantSHA2th256BlockSize;
    
    size_t size = context->accumulatedSize;
    uint8_t bytes[CDVariantSHA2th256BlockSize * 2] = {};
    
    uint8_t * ptr = bytes;
    if (context->accumulatedSize > 0) {
        memcpy(bytes, context->accumulated, context->accumulatedSize);
        ptr += context->accumulatedSize;
        context->accumulatedSize = 0;
    }
    context->bitCountLow += context->accumulatedSize * 8;
    
    // Step 1. Append Padding Bits
    // append one bit (UInt8 with one bit) to message
    *ptr = 0x80;
    ptr += 1;
    size += 1;
    
    // Step 2. append "0" bit until message length in bits ≡ 448 (mod 512)
    size_t max = blockSize - blockSize / 8;//56
    if (size % blockSize < max) {
        ptr = bytes + max;
        
        CUInt64ToBigEndianBytes(context->bitCountLow, ptr);
        CDSHA2Process32(bytes, context->states.u32Values);
    } else {
        ptr = bytes + blockSize + max;
        CUInt64ToBigEndianBytes(context->bitCountLow, ptr);
        CDSHA2Process32(bytes, context->states.u32Values);
        CDSHA2Process32(bytes + blockSize, context->states.u32Values);
    }
}

static inline void _CDSHA2Final512(CDSHA2Context_s * _Nonnull context) {
    assert(context);
    
    size_t blockSize = CDVariantSHA2th512BlockSize;
    
    size_t size = context->accumulatedSize;
    uint8_t bytes[CDVariantSHA2th512BlockSize * 2] = {};
    
    uint8_t * ptr = bytes;
    if (context->accumulatedSize > 0) {
        memcpy(bytes, context->accumulated, context->accumulatedSize);
        ptr += context->accumulatedSize;
        context->accumulatedSize = 0;
    }
    
    uint64_t bitCountLow = context->bitCountLow + context->accumulatedSize * 8;
    if (bitCountLow < context->bitCountLow) {
        context->bitCountHigh += 1;
    }
    context->bitCountLow = bitCountLow;
    
    // Step 1. Append Padding Bits
    // append one bit (UInt8 with one bit) to message
    *ptr = 0x80;
    ptr += 1;
    size += 1;
    
    // Step 2. append "0" bit until message length in bits ≡ 896 (mod 1024)
    size_t max = blockSize - blockSize / 8;//112
    if (size % blockSize < max) {
        ptr = bytes + max;
        CUInt64ToBigEndianBytes(context->bitCountHigh, ptr);
        ptr += 8;
        CUInt64ToBigEndianBytes(context->bitCountLow, ptr);
        CDSHA2Process64(bytes, context->states.u64Values);
    } else {
        ptr = bytes + blockSize + max;
        CUInt64ToBigEndianBytes(context->bitCountHigh, ptr);
        ptr += 8;
        CUInt64ToBigEndianBytes(context->bitCountLow, ptr);
        CDSHA2Process64(bytes, context->states.u64Values);
        CDSHA2Process64(bytes + blockSize, context->states.u64Values);
    }
}


void CDSHA2Final(CDSHA2Context_s * _Nonnull context) {
    switch (context->digestVariant) {
        case CDVariantSHA2th224:
        case CDVariantSHA2th256:
            _CDSHA2Final256(context);
            break;
        case CDVariantSHA2th384:
        case CDVariantSHA2th512:
            _CDSHA2Final512(context);
            break;
        default:
            abort();
            break;
    }
}

static inline void _CDSHA2ExportHashValue256(CDSHA2Context_s * _Nonnull context, uint8_t * _Nonnull bytes, size_t length) {
    assert(context);
    assert(bytes);
    
    uint8_t * ptr = bytes;
    size_t count = length / 4;
    uint32_t * values = context->states.u32Values;
    for (size_t idx=0; idx<count; idx++) {
        uint32_t v = values[idx];
        CUInt32ToBigEndianBytes(v, ptr);
        ptr += 4;
    }
}

static inline void _CDSHA2ExportHashValue512(CDSHA2Context_s * _Nonnull context, uint8_t * _Nonnull bytes, size_t length) {
    assert(context);
    assert(bytes);
    
    uint8_t * ptr = bytes;
    size_t count = length / 8;
    uint64_t * values = context->states.u64Values;
    for (size_t idx=0; idx<count; idx++) {
        uint64_t v = values[idx];
        CUInt64ToBigEndianBytes(v, ptr);
        ptr += 8;
    }
}
void CDSHA2ExportHashValue(CDSHA2Context_s * _Nonnull context, uint8_t * _Nonnull bytes) {
    switch (context->digestVariant) {
        case CDVariantSHA2th224:
            _CDSHA2ExportHashValue256(context, bytes, 28);
            break;
        case CDVariantSHA2th256:
            _CDSHA2ExportHashValue256(context, bytes, 32);
            break;
        case CDVariantSHA2th384:
            _CDSHA2ExportHashValue512(context, bytes, 48);
            break;
        case CDVariantSHA2th512:
            _CDSHA2ExportHashValue512(context, bytes, 64);
            break;
        default:
            abort();
            break;
    }
}
