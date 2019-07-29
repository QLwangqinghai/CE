//
//  CDSHA3.c
//  Digest
//
//  Created by vector on 2019/7/29.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CoreDigest.h"



#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))

//θ
static inline void CDSHA3ProcessTheta(uint64_t * _Nonnull currentHash) {
    uint64_t c[5] = {};
    uint64_t d[5] = {};
    uint64_t * a = currentHash;
    
    for (uint32_t i=0; i<5; i++) {
        c[i] = a[i] ^ a[i + 5] ^ a[i + 10] ^ a[i + 15] ^ a[i + 20];
    }
    
    d[0] = CUInt64RotateLeft(c[1], 1) ^ c[4];
    d[1] = CUInt64RotateLeft(c[2], 1) ^ c[0];
    d[2] = CUInt64RotateLeft(c[3], 1) ^ c[1];
    d[3] = CUInt64RotateLeft(c[4], 1) ^ c[2];
    d[4] = CUInt64RotateLeft(c[0], 1) ^ c[3];
    
    for (uint32_t i=0; i<5; i++) {
        a[i] ^= d[i];
        a[i + 5] ^= d[i];
        a[i + 10] ^= d[i];
        a[i + 15] ^= d[i];
        a[i + 20] ^= d[i];
    }
}

//π A′[x, y, z]=A[(x &+ 3y) mod 5, x, z]
static inline void CDSHA3ProcessPi(uint64_t * _Nonnull currentHash) {
    uint64_t * a = currentHash;

    uint64_t a1 = a[1];
    a[1] = a[6];
    a[6] = a[9];
    a[9] = a[22];
    a[22] = a[14];
    a[14] = a[20];
    a[20] = a[2];
    a[2] = a[12];
    a[12] = a[13];
    a[13] = a[19];
    a[19] = a[23];
    a[23] = a[15];
    a[15] = a[4];
    a[4] = a[24];
    a[24] = a[21];
    a[21] = a[8];
    a[8] = a[16];
    a[16] = a[5];
    a[5] = a[3];
    a[3] = a[18];
    a[18] = a[17];
    a[17] = a[11];
    a[11] = a[7];
    a[7] = a[10];
    a[10] = a1;
}


//χ
static inline void CDSHA3ProcessChi(uint64_t * _Nonnull currentHash) {
    uint64_t * a = currentHash;
    uint64_t a0;
    uint64_t a1;
    for (int i = 0; i < 24; i+=5) {
        a0 = a[0 + i];
        a1 = a[1 + i];
        a[0 + i] ^= ~a1 & a[2 + i];
        a[1 + i] ^= ~a[2 + i] & a[3 + i];
        a[2 + i] ^= ~a[3 + i] & a[4 + i];
        a[3 + i] ^= ~a[4 + i] & a0;
        a[4 + i] ^= ~a0 & a1;
    }
}

////ι
//static inline void CDSHA3ProcessIota(uint64_t * _Nonnull currentHash, size_t round) {
//    uint64_t * a = currentHash;
//    a[0] ^= r[round]
//}


//blockSize in [144, 136, 104, 72]
static inline void CDSHA3Process(const uint8_t * _Nonnull block, uint64_t * _Nonnull currentHash, size_t blockSize) {
    // break chunk into sixteen 64-bit words M[j], 0 ≤ j ≤ 15, big-endian
    // Extend the sixteen 64-bit words into eighty 64-bit words:
    size_t const kCount = 80;
    
    static const uint64_t r[kCount] = {
        0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL,
        0x000000000000808bULL, 0x0000000080000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL,
        0x000000000000008aULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
        0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL,
        0x8000000000008002ULL, 0x8000000000000080ULL, 0x000000000000800aULL, 0x800000008000000aULL,
        0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL,
    };
    
    uint64_t * hh = currentHash;
    uint8_t const * iter = block;
    
    size_t nCount = blockSize / 8;
    if (nCount > 25) {
        nCount = 25;
    }
    size_t idx = 0;
    for (; idx<nCount; idx++) {
        hh[idx] = hh[idx] ^ CUInt64MakeWithLittleEndianBytes(iter);
        iter += 8;
    }
    
    for (int i = 0; i < 24; i++) {
        CDSHA3ProcessTheta(currentHash);

        hh[1] = CUInt64RotateLeft(hh[1], 1);
        hh[2] = CUInt64RotateLeft(hh[2], 62);
        hh[3] = CUInt64RotateLeft(hh[3], 28);
        hh[4] = CUInt64RotateLeft(hh[4], 27);
        hh[5] = CUInt64RotateLeft(hh[5], 36);
        hh[6] = CUInt64RotateLeft(hh[6], 44);
        hh[7] = CUInt64RotateLeft(hh[7], 6);
        hh[8] = CUInt64RotateLeft(hh[8], 55);
        hh[9] = CUInt64RotateLeft(hh[9], 20);
        hh[10] = CUInt64RotateLeft(hh[10], 3);
        hh[11] = CUInt64RotateLeft(hh[11], 10);
        hh[12] = CUInt64RotateLeft(hh[12], 43);
        hh[13] = CUInt64RotateLeft(hh[13], 25);
        hh[14] = CUInt64RotateLeft(hh[14], 39);
        hh[15] = CUInt64RotateLeft(hh[15], 41);
        hh[16] = CUInt64RotateLeft(hh[16], 45);
        hh[17] = CUInt64RotateLeft(hh[17], 15);
        hh[18] = CUInt64RotateLeft(hh[18], 21);
        hh[19] = CUInt64RotateLeft(hh[19], 8);
        hh[20] = CUInt64RotateLeft(hh[20], 18);
        hh[21] = CUInt64RotateLeft(hh[21], 2);
        hh[22] = CUInt64RotateLeft(hh[22], 61);
        hh[23] = CUInt64RotateLeft(hh[23], 56);
        hh[24] = CUInt64RotateLeft(hh[24], 14);

        CDSHA3ProcessPi(hh);
        CDSHA3ProcessChi(hh);
        
        hh[0] ^= r[i];
    }
}

void CDSHA3th224ContextInit(CDSHA3th224Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA3th224Context_s));
    context->digestVariant = CDVariantSHA3th224;
}
void CDSHA3th256ContextInit(CDSHA3th256Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA3th256Context_s));
    context->digestVariant = CDVariantSHA3th256;
}

void CDSHA3th384ContextInit(CDSHA3th384Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA3th384Context_s));
    context->digestVariant = CDVariantSHA3th384;
}
void CDSHA3th512ContextInit(CDSHA3th512Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA3th512Context_s));
    context->digestVariant = CDVariantSHA3th512;
}

void CDSHA3thKeccak224ContextInit(CDSHA3thKeccak224Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA3thKeccak224Context_s));
    context->digestVariant = CDVariantSHA3thKeccak224;
}
void CDSHA3thKeccak256ContextInit(CDSHA3thKeccak256Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA3thKeccak256Context_s));
    context->digestVariant = CDVariantSHA3thKeccak256;
}

void CDSHA3thKeccak384ContextInit(CDSHA3thKeccak384Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA3thKeccak384Context_s));
    context->digestVariant = CDVariantSHA3thKeccak384;
}
void CDSHA3thKeccak512ContextInit(CDSHA3thKeccak512Context_s * _Nonnull context) {
    assert(context);
    memset(context, 0, sizeof(CDSHA3thKeccak512Context_s));
    context->digestVariant = CDVariantSHA3thKeccak512;
}
static inline void __CDSHA3thFinal(uint8_t * _Nonnull accumulated, size_t * _Nonnull accumulatedSizePtr, size_t blockSize, uint64_t * _Nonnull currentHash, uint8_t markByte) {
    assert(accumulated);
    assert(accumulatedSizePtr);
    assert(currentHash);
    assert(blockSize == 144 || blockSize == 136 || blockSize == 104 || blockSize == 72);
    assert(markByte == 0x06 || blockSize == 0x01);
    
    // Add padding
    size_t markByteIndex = *accumulatedSizePtr;
    
    size_t r = blockSize * 8;
    size_t q = (r / 8) - (markByteIndex % (r / 8));
    memset(accumulated + markByteIndex, 0, q);
    accumulated[markByteIndex] |= markByte;
    accumulated[blockSize - 1] |= 0x80;
    CDSHA3Process(accumulated, currentHash, blockSize);
}

static inline void _CDSHA3thFinal(uint8_t * _Nonnull accumulated, size_t * _Nonnull accumulatedSizePtr, size_t blockSize, uint64_t * _Nonnull currentHash) {
    __CDSHA3thFinal(accumulated, accumulatedSizePtr, blockSize, currentHash, 0x06);
}

static inline void _CDSHA3thKeccakFinal(uint8_t * _Nonnull accumulated, size_t * _Nonnull accumulatedSizePtr, size_t blockSize, uint64_t * _Nonnull currentHash) {
    __CDSHA3thFinal(accumulated, accumulatedSizePtr, blockSize, currentHash, 0x01);
}

void CDSHA3th224Final(CDSHA3th224Context_s * _Nonnull context) {
    _CDSHA3thFinal(context->accumulated, &(context->accumulatedSize), 144, context->values);
}
void CDSHA3th256Final(CDSHA3th256Context_s * _Nonnull context) {
    _CDSHA3thFinal(context->accumulated, &(context->accumulatedSize), 136, context->values);
}
void CDSHA3th384Final(CDSHA3th384Context_s * _Nonnull context) {
    _CDSHA3thFinal(context->accumulated, &(context->accumulatedSize), 104, context->values);
}
void CDSHA3th512Final(CDSHA3th512Context_s * _Nonnull context) {
    _CDSHA3thFinal(context->accumulated, &(context->accumulatedSize), 72, context->values);
}

void CDSHA3thKeccak224Final(CDSHA3thKeccak224Context_s * _Nonnull context) {
    _CDSHA3thKeccakFinal(context->accumulated, &(context->accumulatedSize), 144, context->values);
}
void CDSHA3thKeccak256Final(CDSHA3thKeccak256Context_s * _Nonnull context) {
    _CDSHA3thKeccakFinal(context->accumulated, &(context->accumulatedSize), 136, context->values);
}
void CDSHA3thKeccak384Final(CDSHA3thKeccak384Context_s * _Nonnull context) {
    _CDSHA3thKeccakFinal(context->accumulated, &(context->accumulatedSize), 104, context->values);
}
void CDSHA3thKeccak512Final(CDSHA3thKeccak512Context_s * _Nonnull context) {
    _CDSHA3thKeccakFinal(context->accumulated, &(context->accumulatedSize), 72, context->values);
}

//static inline void CDSHA3Process(uint8_t * _Nonnull accumulated, size_t * _Nonnull accumulatedSize, size_t blockSize, const uint8_t * _Nonnull block, uint64_t * _Nonnull currentHash) {

static inline void _CDSHA3Update(uint8_t * _Nonnull accumulated, size_t * _Nonnull accumulatedSizePtr, size_t blockSize, uint64_t * _Nonnull currentHash, uint8_t const * _Nonnull bytes, size_t length) {
    assert(accumulated);
    assert(accumulatedSizePtr);
    assert(currentHash);
    assert(blockSize == 144 || blockSize == 136 || blockSize == 104 || blockSize == 72);
    assert(length >= 0);
    
    if (0 == length) {
        return;
    }
    assert(bytes);
    size_t accumulatedSize = *accumulatedSizePtr;
    uint8_t const * ptr = bytes;
    if (accumulatedSize > 0) {
        size_t missingLength = blockSize - accumulatedSize;
        if (length < missingLength) {
            memcpy(accumulated, ptr, length);
            *accumulatedSizePtr += length;
            return;
        } else {
            memcpy(accumulated, ptr, missingLength);
            CDSHA3Process(accumulated, currentHash, blockSize);
            *accumulatedSizePtr = 0;
            ptr += missingLength;
            length -= missingLength;
        }
    }
    for (; length >= blockSize; length -= blockSize) {
        CDSHA3Process(ptr, currentHash, blockSize);
        ptr += blockSize;
    }
    
    if (length > 0) {
        memcpy(accumulated, ptr, length);
        *accumulatedSizePtr += length;
    }
}

void CDSHA3th224Update(CDSHA3th224Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA3Update(context->accumulated, &(context->accumulatedSize), 144, context->values, bytes, length);
}
void CDSHA3th256Update(CDSHA3th256Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA3Update(context->accumulated, &(context->accumulatedSize), 136, context->values, bytes, length);
}
void CDSHA3th384Update(CDSHA3th384Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA3Update(context->accumulated, &(context->accumulatedSize), 104, context->values, bytes, length);
}
void CDSHA3th512Update(CDSHA3th512Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA3Update(context->accumulated, &(context->accumulatedSize), 72, context->values, bytes, length);
}

void CDSHA3thKeccak224Update(CDSHA3thKeccak224Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA3Update(context->accumulated, &(context->accumulatedSize), 144, context->values, bytes, length);
}
void CDSHA3thKeccak256Update(CDSHA3thKeccak256Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA3Update(context->accumulated, &(context->accumulatedSize), 136, context->values, bytes, length);
}

void CDSHA3thKeccak384Update(CDSHA3thKeccak384Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA3Update(context->accumulated, &(context->accumulatedSize), 104, context->values, bytes, length);
}
void CDSHA3thKeccak512Update(CDSHA3thKeccak512Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    _CDSHA3Update(context->accumulated, &(context->accumulatedSize), 72, context->values, bytes, length);
}

static inline void _CDSHA3thExportHashValue(uint64_t * _Nonnull currentHash, uint8_t * _Nonnull bytes, size_t length) {
    assert(currentHash);
    assert(bytes);
    assert(length == 64 || length == 48 || length == 32 || length == 28);
    uint8_t tmp[8] = {};
    uint8_t * ptr = bytes;
    size_t count = length / 8;
    for (size_t idx=0; idx<count; idx++) {
        uint64_t v = currentHash[idx];
        CUInt64ToBigEndianBytes(v, ptr);
        ptr += 8;
    }
    
    size_t rcount = length % 8;
    if (rcount > 0) {
        uint64_t v = currentHash[count];
        CUInt64ToBigEndianBytes(v, tmp);
        memcpy(ptr, tmp, rcount);
    }
}

void CDSHA3th224ExportHashValue(CDSHA3th224Context_s * _Nonnull context, uint8_t bytes[_Nonnull 28]) {
    _CDSHA3thExportHashValue(context->values, bytes, 28);
}
void CDSHA3th256ExportHashValue(CDSHA3th256Context_s * _Nonnull context, uint8_t bytes[_Nonnull 32]) {
    _CDSHA3thExportHashValue(context->values, bytes, 32);
}
void CDSHA3th384ExportHashValue(CDSHA3th384Context_s * _Nonnull context, uint8_t bytes[_Nonnull 48]) {
    _CDSHA3thExportHashValue(context->values, bytes, 48);
}
void CDSHA3th512ExportHashValue(CDSHA3th512Context_s * _Nonnull context, uint8_t bytes[_Nonnull 64]) {
    _CDSHA3thExportHashValue(context->values, bytes, 64);
}
void CDSHA3thKeccak224ExportHashValue(CDSHA3thKeccak224Context_s * _Nonnull context, uint8_t bytes[_Nonnull 28]) {
    _CDSHA3thExportHashValue(context->values, bytes, 28);
}
void CDSHA3thKeccak256ExportHashValue(CDSHA3thKeccak256Context_s * _Nonnull context, uint8_t bytes[_Nonnull 32]) {
    _CDSHA3thExportHashValue(context->values, bytes, 32);
}
void CDSHA3thKeccak384ExportHashValue(CDSHA3thKeccak384Context_s * _Nonnull context, uint8_t bytes[_Nonnull 48]) {
    _CDSHA3thExportHashValue(context->values, bytes, 48);
}
void CDSHA3thKeccak512ExportHashValue(CDSHA3thKeccak512Context_s * _Nonnull context, uint8_t bytes[_Nonnull 64]) {
    _CDSHA3thExportHashValue(context->values, bytes, 64);
}
