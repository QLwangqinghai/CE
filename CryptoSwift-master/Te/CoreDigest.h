//
//  CoreDigest.h
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CoreDigest_h
#define CoreDigest_h

#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef __APPLE__
#include <libkern/OSByteOrder.h>
#elif __linux__
#include <endian.h>
#elif defined(__FreeBSD__)
#include <sys/endian.h>
#elif defined(_WIN32)
#endif






typedef enum _CDVariant {
    CDVariantNone,
    CDVariantMD5 = 1,
    CDVariantSHA1th160 = 2,
    CDVariantSHA2th224 = 3,
    CDVariantSHA2th256 = 4,
    CDVariantSHA2th384 = 5,
    CDVariantSHA2th512 = 6,
} CDVariant_e;

#define CDVariantMD5BlockSize 64
#define CDVariantSHA1th160BlockSize 64
#define CDVariantSHA2th256BlockSize 64
#define CDVariantSHA2th512BlockSize 128

static inline size_t CDCDVariantDigestLength(CDVariant_e e) {
    switch (e) {
        case CDVariantMD5:
            return 128/8;
        case CDVariantSHA1th160:
            return 160/8;
        case CDVariantSHA2th224:
            return 224/8;//28
        case CDVariantSHA2th256:
            return 256/8;//32
        case CDVariantSHA2th384:
            return 384/8;//48
        case CDVariantSHA2th512:
            return 512/8;//64
        default:
            return 0;
    }
}

static inline size_t CDVariantBlockSize(CDVariant_e e) {
    switch (e) {
        case CDVariantMD5:
            return CDVariantMD5BlockSize;
        case CDVariantSHA1th160:
            return CDVariantSHA1th160BlockSize;
        case CDVariantSHA2th224:
        case CDVariantSHA2th256:
            return CDVariantSHA2th256BlockSize;
        case CDVariantSHA2th384:
        case CDVariantSHA2th512:
            return CDVariantSHA2th512BlockSize;
        default:
            return 0;
    }
}

typedef struct _CDMD5Context {
    size_t digestVariant;
    uint32_t values[4];
    uint64_t bitCount;
    uint8_t accumulated[CDVariantMD5BlockSize];
    uint32_t accumulatedSize;
} CDMD5Context_s;

typedef struct _CDSHA2th256Context {
    size_t digestVariant;
    uint32_t values[8];
    uint64_t bitCount;
    uint8_t accumulated[CDVariantSHA2th256BlockSize];
    uint32_t accumulatedSize;
} CDSHA2th256Context_s;

typedef struct _CDSHA2th512Context {
    size_t digestVariant;
    uint64_t values[8];
    uint64_t bitCountLow;//长度的低位
    uint64_t bitCountHigh;//长度的高位
    uint8_t accumulated[CDVariantSHA2th512BlockSize];
    uint32_t accumulatedSize;
} CDSHA2th512Context_s;

typedef CDSHA2th256Context_s CDSHA2th224Context_s;
typedef CDSHA2th512Context_s CDSHA2th384Context_s;


void CDMD5ContextInit(CDMD5Context_s * _Nonnull context);
void CDMD5Final(CDMD5Context_s * _Nonnull context);
void CDMD5Update(CDMD5Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDMD5ExportHashValue(CDMD5Context_s * _Nonnull context, uint8_t bytes[_Nonnull 16]);
void CDMD5Update2(CDMD5Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);

void CDSHA2th224ContextInit(CDSHA2th224Context_s * _Nonnull context);
void CDSHA2th224Update(CDSHA2th224Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2th224Final(CDSHA2th224Context_s * _Nonnull context);
void CDSHA2th224ExportHashValue(CDSHA2th224Context_s * _Nonnull context, uint8_t bytes[_Nonnull 28]);


void CDSHA2th256ContextInit(CDSHA2th256Context_s * _Nonnull context);
void CDSHA2th256Update(CDSHA2th256Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2th256Final(CDSHA2th256Context_s * _Nonnull context);
void CDSHA2th256ExportHashValue(CDSHA2th256Context_s * _Nonnull context, uint8_t bytes[_Nonnull 32]);


void CDSHA2th384ContextInit(CDSHA2th384Context_s * _Nonnull context);
void CDSHA2th384Update(CDSHA2th384Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2th384Final(CDSHA2th384Context_s * _Nonnull context);
void CDSHA2th384ExportHashValue(CDSHA2th384Context_s * _Nonnull context, uint8_t bytes[_Nonnull 48]);


void CDSHA2th512ContextInit(CDSHA2th512Context_s * _Nonnull context);
void CDSHA2th512Update(CDSHA2th512Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2th512Final(CDSHA2th512Context_s * _Nonnull context);
void CDSHA2th512ExportHashValue(CDSHA2th512Context_s * _Nonnull context, uint8_t bytes[_Nonnull 64]);



#pragma mark - helper

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static inline uint64_t CUInt64MakeWithBigEndianBytes(uint8_t const block[_Nonnull 8]) {
    return (((uint64_t)block[0]) << 56) | (((uint64_t)block[1]) << 48) | (((uint64_t)block[2]) << 40) | (((uint64_t)block[3]) << 32) | (((uint64_t)block[4]) << 24) | (((uint64_t)block[5]) << 16) | (((uint64_t)block[6]) << 8) | ((uint64_t)block[7]);
}
static inline uint32_t CUInt32MakeWithBigEndianBytes(uint8_t const block[_Nonnull 4]) {
    return (((uint32_t)block[0]) << 24) | (((uint32_t)block[1]) << 16) | (((uint32_t)block[2]) << 8) | ((uint32_t)block[3]);
}
static inline uint64_t CUInt64MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 8]) {
    return *((uint64_t *)block);
}
static inline uint32_t CUInt32MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 4]) {
    return *((uint32_t *)block);
}


static inline void CUInt64ToBigEndianBytes(uint64_t value, uint8_t bytes[_Nonnull 8]) {
    bytes[0] = (uint8_t)(value >> 56);
    bytes[1] = (uint8_t)(value >> 48);
    bytes[2] = (uint8_t)(value >> 40);
    bytes[3] = (uint8_t)(value >> 32);
    bytes[4] = (uint8_t)(value >> 24);
    bytes[5] = (uint8_t)(value >> 16);
    bytes[6] = (uint8_t)(value >> 8);
    bytes[7] = (uint8_t)(value);
}
static inline void CUInt32ToBigEndianBytes(uint32_t value, uint8_t bytes[_Nonnull 4]) {
    bytes[0] = (uint8_t)(value >> 24);
    bytes[1] = (uint8_t)(value >> 16);
    bytes[2] = (uint8_t)(value >> 8);
    bytes[3] = (uint8_t)(value);
}
static inline void CUInt64ToLittleEndianBytes(uint64_t value, uint8_t block[_Nonnull 8]) {
    *((uint64_t *)block) = value;
}
static inline void CUInt32ToLittleEndianBytes(uint32_t value, uint8_t block[_Nonnull 4]) {
    *((uint32_t *)block) = value;
}


#elif __BYTE_ORDER__== __ORDER_BIG_ENDIAN__
static inline uint64_t CUInt64MakeWithBigEndianBytes(uint8_t const block[_Nonnull 8]) {
    return *((uint64_t *)block);
}
static inline uint32_t CUInt32MakeWithBigEndianBytes(uint8_t const block[_Nonnull 4]) {
    return *((uint32_t *)block);
}
static inline uint64_t CUInt64MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 8]) {
    return (((uint64_t)block[0])) | (((uint64_t)block[1]) << 8) | (((uint64_t)block[2]) << 16) | (((uint64_t)block[3]) << 24) | (((uint64_t)block[4]) << 32) | (((uint64_t)block[5]) << 40) | (((uint64_t)block[6]) << 48) | ((uint64_t)block[7] << 56);
}
static inline uint32_t CUInt32MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 4]) {
    return (((uint32_t)block[0])) | (((uint32_t)block[1]) << 8) | (((uint32_t)block[2]) << 16) | (((uint32_t)block[3]) << 24);
}


static inline void CUInt64ToBigEndianBytes(uint64_t value, uint8_t bytes[_Nonnull 8]) {
    *((uint64_t *)block) = value;
}
static inline void CUInt32ToBigEndianBytes(uint32_t value, uint8_t bytes[_Nonnull 4]) {
    *((uint32_t *)block) = value;
}
static inline void CUInt64ToLittleEndianBytes(uint64_t value, uint8_t block[_Nonnull 8]) {
    bytes[7] = (uint8_t)(value >> 56);
    bytes[6] = (uint8_t)(value >> 48);
    bytes[5] = (uint8_t)(value >> 40);
    bytes[4] = (uint8_t)(value >> 32);
    bytes[3] = (uint8_t)(value >> 24);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[1] = (uint8_t)(value >> 8);
    bytes[0] = (uint8_t)(value);
}
static inline void CUInt32ToLittleEndianBytes(uint32_t value, uint8_t block[_Nonnull 4]) {
    *((uint32_t *)block) = value;
    
    bytes[3] = (uint8_t)(value >> 24);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[1] = (uint8_t)(value >> 8);
    bytes[0] = (uint8_t)(value);
}

#else
#error "DEFINE BIG_ENDIAN OR LITTLE_ENDIAN"
#endif





static inline uint64_t CDUInt64RotateRight(uint64_t value, uint64_t by) {
    return (value >> by) | (value << (64 - by));
}
static inline uint32_t CDUInt32RotateRight(uint32_t value, uint32_t by) {
    return (value >> by) | (value << (32 - by));
}

static inline uint32_t CDUInt32RotateLeft(uint32_t value, uint32_t by) {
    return ((value << by) & 0xffffffff) | (value >> (32 - by));
}

#endif /* CoreDigest_h */
