//
//  CDMD.c
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CoreDigest.h"


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

        B = B + CDUInt32RotateLeft(A + F + k[j] + Mg, s[j]);
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
        
        B = B + CDUInt32RotateLeft(A + F + k[j] + Mg, s[j]);
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

        B = B + CDUInt32RotateLeft(A + F + k[j] + Mg, s[j]);
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

        B = B + CDUInt32RotateLeft(A + F + k[j] + Mg, s[j]);
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
