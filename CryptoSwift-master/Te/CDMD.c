//
//  CDMD.c
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CoreDigest.h"


typedef struct _CDMD5Context {
    size_t digestVariant;
    uint32_t values[4];
    uint64_t bitCount;
    uint32_t data[16];
    uint8_t accumulated[CDVariantMD5BlockSize];
    uint32_t accumulatedSize;
} CDMD5Context_s;


void CDSHA2th512ContextInit(CDSHA2th512Context_s * _Nonnull context);
void CDSHA2th512Update(CDSHA2th512Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2th512Final(CDSHA2th512Context_s * _Nonnull context);
void CDSHA2th512ExportHashValue(CDSHA2th512Context_s * _Nonnull context, uint8_t bytes[_Nonnull 64]);


static inline void CDMD5Process(uint8_t const block[_Nonnull 64], uint32_t currentHash[_Nonnull 4]) {
    size_t const kCount = 64;
    
    static const uint32_t k[kCount] = {
        0xd76aa478UL, 0xe8c7b756UL, 0x242070dbUL, 0xc1bdceeeUL, 0xf57c0fafUL, 0x4787c62aUL, 0xa8304613UL, 0xfd469501UL,
        0x698098d8UL, 0x8b44f7afUL, 0xffff5bb1UL, 0x895cd7beUL,
                                    0x6b901122UL, 0xfd987193, 0xa679438eUL, 0x49b40821UL,
                                    0xf61e2562UL, 0xc040b340, 0x265e5a51UL, 0xe9b6c7aaUL,
                                    0xd62f105d, 0x2441453, 0xd8a1e681UL, 0xe7d3fbc8UL,
                                    0x21e1cde6, 0xc33707d6, 0xf4d50d87UL, 0x455a14edUL,
                                    0xa9e3e905, 0xfcefa3f8, 0x676f02d9UL, 0x8d2a4c8aUL,
                                    0xfffa3942, 0x8771f681, 0x6d9d6122UL, 0xfde5380cUL,
                                    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60UL, 0xbebfbc70UL,
                                    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x4881d05UL,
                                    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665UL,
                                    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039UL,
                                    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1UL,
                                    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1UL,
                                    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391UL,
                                    };

    
    //    /** specifies the per-round shift amounts */
    //    private let s: Array<UInt32> = [
    //                                    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    //                                    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    //                                    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    //                                    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
    //                                    ]
    //
    //    /** binary integer part of the sines of integers (Radians) */
    
    
    uint32_t M[kCount] = {0};
    uint32_t * hh = currentHash;
    uint8_t const * iter = block;
    size_t idx = 0;
    for (; idx<16; idx++) {
        M[idx] = CDReadUInt32(iter);
        iter += 4;
    }
    
    for (; idx<kCount; idx++) {
        uint32_t s0 = CDUInt32RotateRight(M[idx - 15], 7) ^ CDUInt32RotateRight(M[idx - 15], 18) ^ (M[idx - 15] >> 3);
        uint32_t s1 = CDUInt32RotateRight(M[idx - 2], 17) ^ CDUInt32RotateRight(M[idx - 2], 19) ^ (M[idx - 2] >> 10);
        M[idx] = M[idx - 16] + s0 + M[idx - 7] + s1;
    }
    
    uint32_t A = hh[0];
    uint32_t B = hh[1];
    uint32_t C = hh[2];
    uint32_t D = hh[3];
    uint32_t E = hh[4];
    uint32_t F = hh[5];
    uint32_t G = hh[6];
    uint32_t H = hh[7];
    
    // Main loop
    for (size_t j=0; j<kCount; j++) {
        uint32_t s0 = CDUInt32RotateRight(A, 2) ^ CDUInt32RotateRight(A, 13) ^ CDUInt32RotateRight(A, 22);
        uint32_t maj = (A & B) ^ (A & C) ^ (B & C);
        uint32_t t2 = s0 + maj;
        uint32_t s1 = CDUInt32RotateRight(E, 6) ^ CDUInt32RotateRight(E, 11) ^ CDUInt32RotateRight(E, 25);
        uint32_t ch = (E & F) ^ ((~E) & G);
        uint32_t t1 = H + s1 + ch + k[j] + M[j];
        
        H = G;
        G = F;
        F = E;
        E = D + t1;
        D = C;
        C = B;
        B = A;
        A = t1 + t2;
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

void CDSHA2th224ContextInit(CDSHA2th256Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA2th256Context_s));
    context->digestVariant = CDVariantSHA2th224;
    context->values[0] = 0xc1059ed8UL;
    context->values[1] = 0x367cd507UL;
    context->values[2] = 0x3070dd17UL;
    context->values[3] = 0xf70e5939UL;
    context->values[4] = 0xffc00b31UL;
    context->values[5] = 0x68581511UL;
    context->values[6] = 0x64f98fa7UL;
    context->values[7] = 0xbefa4fa4UL;
}
void CDSHA2th256ContextInit(CDSHA2th256Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA2th256Context_s));
    context->digestVariant = CDVariantSHA2th256;
    context->values[0] = 0x6a09e667UL;
    context->values[1] = 0xbb67ae85UL;
    context->values[2] = 0x3c6ef372UL;
    context->values[3] = 0xa54ff53aUL;
    context->values[4] = 0x510e527fUL;
    context->values[5] = 0x9b05688cUL;
    context->values[6] = 0x1f83d9abUL;
    context->values[7] = 0x5be0cd19UL;
}

void CDSHA2th384ContextInit(CDSHA2th512Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA2th512Context_s));
    context->digestVariant = CDVariantSHA2th384;
    context->values[0] = 0xcbbb9d5dc1059ed8ULL;
    context->values[1] = 0x629a292a367cd507ULL;
    context->values[2] = 0x9159015a3070dd17ULL;
    context->values[3] = 0x152fecd8f70e5939ULL;
    context->values[4] = 0x67332667ffc00b31ULL;
    context->values[5] = 0x8eb44a8768581511ULL;
    context->values[6] = 0xdb0c2e0d64f98fa7ULL;
    context->values[7] = 0x47b5481dbefa4fa4ULL;
}
void CDSHA2th512ContextInit(CDSHA2th512Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA2th512Context_s));
    context->digestVariant = CDVariantSHA2th512;
    context->values[0] = 0x6a09e667f3bcc908ULL;
    context->values[1] = 0xbb67ae8584caa73bULL;
    context->values[2] = 0x3c6ef372fe94f82bULL;
    context->values[3] = 0xa54ff53a5f1d36f1ULL;
    context->values[4] = 0x510e527fade682d1ULL;
    context->values[5] = 0x9b05688c2b3e6c1fULL;
    context->values[6] = 0x1f83d9abfb41bd6bULL;
    context->values[7] = 0x5be0cd19137e2179ULL;
}

static inline void _CDSHA2th256Final(CDSHA2th256Context_s * _Nonnull context) {
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
        ptr[0] = (uint8_t)(context->bitCount >> 56);
        ptr[1] = (uint8_t)(context->bitCount >> 48);
        ptr[2] = (uint8_t)(context->bitCount >> 40);
        ptr[3] = (uint8_t)(context->bitCount >> 32);
        ptr[4] = (uint8_t)(context->bitCount >> 24);
        ptr[5] = (uint8_t)(context->bitCount >> 16);
        ptr[6] = (uint8_t)(context->bitCount >> 8);
        ptr[7] = (uint8_t)(context->bitCount);
        
        CDSHA2Process32(bytes, context->values);
    } else {
        ptr = bytes + blockSize + max;
        ptr[0] = (uint8_t)(context->bitCount >> 56);
        ptr[1] = (uint8_t)(context->bitCount >> 48);
        ptr[2] = (uint8_t)(context->bitCount >> 40);
        ptr[3] = (uint8_t)(context->bitCount >> 32);
        ptr[4] = (uint8_t)(context->bitCount >> 24);
        ptr[5] = (uint8_t)(context->bitCount >> 16);
        ptr[6] = (uint8_t)(context->bitCount >> 8);
        ptr[7] = (uint8_t)(context->bitCount);
        CDSHA2Process32(bytes, context->values);
        CDSHA2Process32(bytes + blockSize, context->values);
    }
}

static inline void _CDSHA2th512Final(CDSHA2th512Context_s * _Nonnull context) {
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
        ptr[0] = (uint8_t)(context->bitCountHigh >> 56);
        ptr[1] = (uint8_t)(context->bitCountHigh >> 48);
        ptr[2] = (uint8_t)(context->bitCountHigh >> 40);
        ptr[3] = (uint8_t)(context->bitCountHigh >> 32);
        ptr[4] = (uint8_t)(context->bitCountHigh >> 24);
        ptr[5] = (uint8_t)(context->bitCountHigh >> 16);
        ptr[6] = (uint8_t)(context->bitCountHigh >> 8);
        ptr[7] = (uint8_t)(context->bitCountHigh);
        
        ptr += 8;
        ptr[0] = (uint8_t)(context->bitCountLow >> 56);
        ptr[1] = (uint8_t)(context->bitCountLow >> 48);
        ptr[2] = (uint8_t)(context->bitCountLow >> 40);
        ptr[3] = (uint8_t)(context->bitCountLow >> 32);
        ptr[4] = (uint8_t)(context->bitCountLow >> 24);
        ptr[5] = (uint8_t)(context->bitCountLow >> 16);
        ptr[6] = (uint8_t)(context->bitCountLow >> 8);
        ptr[7] = (uint8_t)(context->bitCountLow);
        
        CDSHA2Process64(bytes, context->values);
    } else {
        ptr = bytes + blockSize + max;
        
        ptr[0] = (uint8_t)(context->bitCountHigh >> 56);
        ptr[1] = (uint8_t)(context->bitCountHigh >> 48);
        ptr[2] = (uint8_t)(context->bitCountHigh >> 40);
        ptr[3] = (uint8_t)(context->bitCountHigh >> 32);
        ptr[4] = (uint8_t)(context->bitCountHigh >> 24);
        ptr[5] = (uint8_t)(context->bitCountHigh >> 16);
        ptr[6] = (uint8_t)(context->bitCountHigh >> 8);
        ptr[7] = (uint8_t)(context->bitCountHigh);
        
        ptr += 8;
        ptr[0] = (uint8_t)(context->bitCountLow >> 56);
        ptr[1] = (uint8_t)(context->bitCountLow >> 48);
        ptr[2] = (uint8_t)(context->bitCountLow >> 40);
        ptr[3] = (uint8_t)(context->bitCountLow >> 32);
        ptr[4] = (uint8_t)(context->bitCountLow >> 24);
        ptr[5] = (uint8_t)(context->bitCountLow >> 16);
        ptr[6] = (uint8_t)(context->bitCountLow >> 8);
        ptr[7] = (uint8_t)(context->bitCountLow);
        
        CDSHA2Process64(bytes, context->values);
        CDSHA2Process64(bytes + blockSize, context->values);
    }
}


void CDSHA2th224Final(CDSHA2th256Context_s * _Nonnull context) {
    _CDSHA2th256Final(context);
}
void CDSHA2th256Final(CDSHA2th256Context_s * _Nonnull context) {
    _CDSHA2th256Final(context);
}

void CDSHA2th384Final(CDSHA2th512Context_s * _Nonnull context) {
    _CDSHA2th512Final(context);
}
void CDSHA2th512Final(CDSHA2th512Context_s * _Nonnull context) {
    _CDSHA2th512Final(context);
}


static inline void _CDSHA2th256Update(CDSHA2th256Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
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
            CDSHA2Process32(context->accumulated, context->values);
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
        CDSHA2Process32(ptr, context->values);
        ptr += blockSize;
    }
    
    if (length > 0) {
        memcpy(context->accumulated, ptr, length);
        context->accumulatedSize += length;
    }
}

static inline void _CDSHA2th512Update(CDSHA2th512Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
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
            CDSHA2Process64(context->accumulated, context->values);
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
        CDSHA2Process64(ptr, context->values);
        ptr += blockSize;
    }
    
    if (length > 0) {
        memcpy(context->accumulated, ptr, length);
        context->accumulatedSize += length;
    }
}

void CDSHA2th224Update(CDSHA2th256Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA2th256Update(context, bytes, length);
}
void CDSHA2th256Update(CDSHA2th256Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA2th256Update(context, bytes, length);
}

void CDSHA2th384Update(CDSHA2th512Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA2th512Update(context, bytes, length);
}
void CDSHA2th512Update(CDSHA2th512Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA2th512Update(context, bytes, length);
}



static inline void _CDSHA2th256ExportHashValue(CDSHA2th256Context_s * _Nonnull context, uint8_t * _Nonnull bytes, size_t length) {
    assert(context);
    assert(bytes);
    assert(length == 32 || length == 28);
    
    uint8_t * ptr = bytes;
    size_t count = length / 4;
    for (size_t idx=0; idx<count; idx++) {
        uint32_t v = context->values[idx];
        ptr[0] = (uint8_t)(v >> 24);
        ptr[1] = (uint8_t)(v >> 16);
        ptr[2] = (uint8_t)(v >> 8);
        ptr[3] = (uint8_t)(v);
        ptr += 4;
    }
}

static inline void _CDSHA2th512ExportHashValue(CDSHA2th512Context_s * _Nonnull context, uint8_t * _Nonnull bytes, size_t length) {
    assert(context);
    assert(bytes);
    assert(length == 48 || length == 64);
    
    uint8_t * ptr = bytes;
    size_t count = length / 8;
    for (size_t idx=0; idx<count; idx++) {
        uint64_t v = context->values[idx];
        ptr[0] = (uint8_t)(v >> 56);
        ptr[1] = (uint8_t)(v >> 48);
        ptr[2] = (uint8_t)(v >> 40);
        ptr[3] = (uint8_t)(v >> 32);
        ptr[4] = (uint8_t)(v >> 24);
        ptr[5] = (uint8_t)(v >> 16);
        ptr[6] = (uint8_t)(v >> 8);
        ptr[7] = (uint8_t)(v);
        ptr += 8;
    }
}

void CDSHA2th224ExportHashValue(CDSHA2th256Context_s * _Nonnull context, uint8_t bytes[_Nonnull 28]) {
    _CDSHA2th256ExportHashValue(context, bytes, 28);
}
void CDSHA2th256ExportHashValue(CDSHA2th256Context_s * _Nonnull context, uint8_t bytes[_Nonnull 32]) {
    _CDSHA2th256ExportHashValue(context, bytes, 32);
}

void CDSHA2th384ExportHashValue(CDSHA2th512Context_s * _Nonnull context, uint8_t bytes[_Nonnull 48]) {
    _CDSHA2th512ExportHashValue(context, bytes, 48);
}
void CDSHA2th512ExportHashValue(CDSHA2th512Context_s * _Nonnull context, uint8_t bytes[_Nonnull 64]) {
    _CDSHA2th512ExportHashValue(context, bytes, 64);
}


//
//public final class MD5: DigestType {
//    static let blockSize: Int = 64
//    static let digestLength: Int = 16 // 128 / 8
//    fileprivate static let hashInitialValue: Array<UInt32> = [0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476]
//    
//    fileprivate var accumulated = Array<UInt8>()
//    fileprivate var processedBytesTotalCount: Int = 0
//    fileprivate var accumulatedHash: Array<UInt32> = MD5.hashInitialValue
//    
//    /** specifies the per-round shift amounts */
//    private let s: Array<UInt32> = [
//                                    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
//                                    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
//                                    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
//                                    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
//                                    ]
//    
//    /** binary integer part of the sines of integers (Radians) */
//    private let k: Array<UInt32> = [
//                                    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
//                                    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
//                                    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
//                                    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
//                                    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
//                                    0xd62f105d, 0x2441453, 0xd8a1e681, 0xe7d3fbc8,
//                                    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
//                                    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
//                                    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
//                                    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
//                                    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x4881d05,
//                                    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
//                                    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
//                                    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
//                                    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
//                                    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
//                                    ]
//    
//    public init() {
//    }
//    
//    public func calculate(for bytes: Array<UInt8>) -> Array<UInt8> {
//        do {
//            return try update(withBytes: bytes.slice, isLast: true)
//        } catch {
//            fatalError()
//        }
//    }
//    
//    // mutating currentHash in place is way faster than returning new result
//    fileprivate func process(block chunk: ArraySlice<UInt8>, currentHash: inout Array<UInt32>) {
//        assert(chunk.count == 16 * 4)
//        
//        // Initialize hash value for this chunk:
//        var A: UInt32 = currentHash[0]
//        var B: UInt32 = currentHash[1]
//        var C: UInt32 = currentHash[2]
//        var D: UInt32 = currentHash[3]
//        
//        var dTemp: UInt32 = 0
//        
//        // Main loop
//        for j in 0..<k.count {
//            var g = 0
//            var F: UInt32 = 0
//            
//            switch j {
//            case 0...15:
//                F = (B & C) | ((~B) & D)
//                g = j
//                break
//            case 16...31:
//                F = (D & B) | (~D & C)
//                g = (5 * j + 1) % 16
//                break
//            case 32...47:
//                F = B ^ C ^ D
//                g = (3 * j + 5) % 16
//                break
//            case 48...63:
//                F = C ^ (B | (~D))
//                g = (7 * j) % 16
//                break
//            default:
//                break
//            }
//            dTemp = D
//            D = C
//            C = B
//            
//            // break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15 and get M[g] value
//            let gAdvanced = g << 2
//            
//            var Mg = UInt32(chunk[chunk.startIndex &+ gAdvanced])
//            Mg |= UInt32(chunk[chunk.startIndex &+ gAdvanced &+ 1]) << 8
//            Mg |= UInt32(chunk[chunk.startIndex &+ gAdvanced &+ 2]) << 16
//            Mg |= UInt32(chunk[chunk.startIndex &+ gAdvanced &+ 3]) << 24
//            
//            B = B &+ rotateLeft(A &+ F &+ k[j] &+ Mg, by: s[j])
//            A = dTemp
//        }
//        
//        currentHash[0] = currentHash[0] &+ A
//        currentHash[1] = currentHash[1] &+ B
//        currentHash[2] = currentHash[2] &+ C
//        currentHash[3] = currentHash[3] &+ D
//    }
//    }
//    
//    extension MD5: Updatable {
//        public func update(withBytes bytes: ArraySlice<UInt8>, isLast: Bool = false) throws -> Array<UInt8> {
//            accumulated += bytes
//            
//            if isLast {
//                let lengthInBits = (processedBytesTotalCount + accumulated.count) * 8
//                let lengthBytes = lengthInBits.bytes(totalBytes: 64 / 8) // A 64-bit representation of b
//                
//                // Step 1. Append padding
//                bitPadding(to: &accumulated, blockSize: MD5.blockSize, allowance: 64 / 8)
//                
//                // Step 2. Append Length a 64-bit representation of lengthInBits
//                accumulated += lengthBytes.reversed()
//            }
//            
//            var processedBytes = 0
//            for chunk in accumulated.batched(by: MD5.blockSize) {
//                if isLast || (accumulated.count - processedBytes) >= MD5.blockSize {
//                    process(block: chunk, currentHash: &accumulatedHash)
//                    processedBytes += chunk.count
//                }
//            }
//            accumulated.removeFirst(processedBytes)
//            processedBytesTotalCount += processedBytes
//            
//            // output current hash
//            var result = Array<UInt8>()
//            result.reserveCapacity(MD5.digestLength)
//            
//            for hElement in accumulatedHash {
//                let hLE = hElement.littleEndian
//                result += Array<UInt8>(arrayLiteral: UInt8(hLE & 0xff), UInt8((hLE >> 8) & 0xff), UInt8((hLE >> 16) & 0xff), UInt8((hLE >> 24) & 0xff))
//            }
//            
//            // reset hash value for instance
//            if isLast {
//                accumulatedHash = MD5.hashInitialValue
//            }
//            
//            return result
//        }
//    }
