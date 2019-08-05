//
//  CoreDigest.h
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CDAPI_h
#define CDAPI_h

#include <stdio.h>
#include <stdlib.h>
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
    CDVariantNone = 0,
    
    CDVariantMD5 = 1,
    
    CDVariantSHA1th160 = 2,
    
    CDVariantSHA2th224 = 3,
    CDVariantSHA2th256 = 4,
    CDVariantSHA2th384 = 5,
    CDVariantSHA2th512 = 6,
    
    CDVariantSHA3th224 = 7,
    CDVariantSHA3th256 = 8,
    CDVariantSHA3th384 = 9,
    CDVariantSHA3th512 = 10,
    CDVariantSHA3thKeccak224 = 11,
    CDVariantSHA3thKeccak256 = 12,
    CDVariantSHA3thKeccak384 = 13,
    CDVariantSHA3thKeccak512 = 14,
} CDVariant_e;

#define CDVariantMD5BlockSize 64
#define CDVariantSHA1th160BlockSize 64
#define CDVariantSHA2th256BlockSize 64
#define CDVariantSHA2th512BlockSize 128

#define CDVariantDigestLengthMax 64


static inline size_t CDVariantDigestLength(CDVariant_e e) {
    switch (e) {
        case CDVariantMD5:
            return 16;
        case CDVariantSHA1th160:
            return 20;
        case CDVariantSHA2th224:
        case CDVariantSHA3th224:
        case CDVariantSHA3thKeccak224:
            return 28;
        case CDVariantSHA2th256:
        case CDVariantSHA3th256:
        case CDVariantSHA3thKeccak256:
            return 32;
        case CDVariantSHA2th384:
        case CDVariantSHA3th384:
        case CDVariantSHA3thKeccak384:
            return 48;
        case CDVariantSHA2th512:
        case CDVariantSHA3th512:
        case CDVariantSHA3thKeccak512:
            return 64;
        default:
            return 0;
    }
}

#define CDVariantBlockSizeMax 144

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
        case CDVariantSHA3th224:
        case CDVariantSHA3thKeccak224:
            return 144;
        case CDVariantSHA3th256:
        case CDVariantSHA3thKeccak256:
            return 136;
        case CDVariantSHA3th384:
        case CDVariantSHA3thKeccak384:
            return 104;
        case CDVariantSHA3th512:
        case CDVariantSHA3thKeccak512:
            return 72;
        default:
            return 0;
    }
}

static inline size_t CDVarianStatusSize(CDVariant_e e) {
    switch (e) {
        case CDVariantMD5:
            return 16;
        case CDVariantSHA1th160:
            return 20;
        case CDVariantSHA2th224:
        case CDVariantSHA2th256:
            return 32;
        case CDVariantSHA2th384:
        case CDVariantSHA2th512:
            return 64;
        case CDVariantSHA3th224:
        case CDVariantSHA3thKeccak224:
        case CDVariantSHA3th256:
        case CDVariantSHA3thKeccak256:
        case CDVariantSHA3th384:
        case CDVariantSHA3thKeccak384:
        case CDVariantSHA3th512:
        case CDVariantSHA3thKeccak512:
            return 200;
        default:
            return 0;
    }
}

typedef struct _CDMD5Context {
    size_t digestVariant;
    size_t accumulatedSize;
    uint32_t values[4];
    uint64_t bitCount;
    uint8_t accumulated[CDVariantMD5BlockSize];
} CDMD5Context_s;

void CDMD5ContextInit(CDMD5Context_s * _Nonnull context);
void CDMD5Final(CDMD5Context_s * _Nonnull context);
void CDMD5Update(CDMD5Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDMD5ExportHashValue(CDMD5Context_s * _Nonnull context, uint8_t bytes[_Nonnull 16]);



typedef struct _CDSHA1Context {
    size_t digestVariant;
    size_t accumulatedSize;
    uint64_t bitCount;
    uint32_t values[5];
    uint8_t accumulated[CDVariantSHA1th160BlockSize];
} CDSHA1Context_s;

void CDSHA1ContextInit(CDSHA1Context_s * _Nonnull context);
void CDSHA1Update(CDSHA1Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA1Final(CDSHA1Context_s * _Nonnull context);
void CDSHA1ExportHashValue(CDSHA1Context_s * _Nonnull context, uint8_t bytes[_Nonnull 20]);


typedef union _CDSHA2State {
    uint32_t u32Values[8];
    uint64_t u64Values[8];
} CDSHA2State_u;

typedef struct _CDSHA2Context {
    size_t digestVariant;
    size_t accumulatedSize;
    CDSHA2State_u states;
    uint64_t bitCountLow;//长度的低位
    uint64_t bitCountHigh;//长度的高位
    uint8_t accumulated[128];
} CDSHA2Context_s;

void CDSHA2ContextInit(CDSHA2Context_s * _Nonnull context, CDVariant_e e);
void CDSHA2Update(CDSHA2Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2Final(CDSHA2Context_s * _Nonnull context);
void CDSHA2ExportHashValue(CDSHA2Context_s * _Nonnull context, uint8_t * _Nonnull bytes);


typedef struct _CDSHA3Context {
    size_t digestVariant;
    size_t accumulatedSize;
    uint64_t states[25];
    uint8_t accumulated[144];
} CDSHA3Context_s;

void CDSHA3ContextInit(CDSHA3Context_s * _Nonnull context, CDVariant_e e);
void CDSHA3Update(CDSHA3Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA3Final(CDSHA3Context_s * _Nonnull context);
void CDSHA3ExportHashValue(CDSHA3Context_s * _Nonnull context, uint8_t * _Nonnull bytes);


typedef union _CDAllRoundContext {
    size_t digestVariant;
    CDMD5Context_s mdContext;
    CDSHA1Context_s sha1Context;
    CDSHA2Context_s sha2Context;
    CDSHA3Context_s sha3Context;
} CDAllRoundContext_u;


void CDContextInit(CDAllRoundContext_u * _Nonnull context, CDVariant_e e);
void CDUpdate(CDAllRoundContext_u * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDFinal(CDAllRoundContext_u * _Nonnull context);
void CDExportHashValue(CDAllRoundContext_u * _Nonnull context, uint8_t * _Nonnull bytes);


int32_t CDHmac(CDVariant_e e, uint8_t const * _Nullable key, size_t keyLength, uint8_t const * _Nullable input, size_t inputLength, uint8_t * _Nullable output, size_t outputLength);



#pragma mark - helper


////arm uint32_t
//static inline uint32_t CUInt32ByteSwap(uint32_t val) {
//    __asm__("bswap %0" : "=r" (val) : "0" (val));
//    return val;
//}
//
//
////arm uint64_t
//static inline uint64_t CUInt64ByteSwap64(uint64_t val) {
//#ifdef __X64
//    __asm__("bswapq %0" : "=r" (val) : "0" (val));
//#else
//
//    uint64_t lower = val & 0xffffffffU;
//    uint32_t higher = (val >> 32) & 0xffffffffU;
//
//    lower = CUInt32ByteSwap(lower);
//    higher = CUInt32ByteSwap(higher);
//
//    return (lower << 32) + higher;
//
//#endif
//    return val;
//}

static inline uint16_t CUInt16ByteSwap(uint16_t data) {
    return (uint16_t)(data << 8 | data >> 8);
}

static inline uint32_t CUInt32ByteSwap(uint32_t data) {
#if defined(__llvm__)
    data = __builtin_bswap32(data);
#else
    data = (((data ^ (data >> 16 | (data << 16))) & 0xFF00FFFF) >> 8) ^ (data >> 8 | data << 24);
#endif
    return data;
}

static inline uint64_t CUInt64ByteSwap(uint64_t data) {
#if defined(__llvm__)
    return __builtin_bswap64(data);
#else
    union {
        uint64_t ull;
        uint32_t ul[2];
    } u;
    u.ull = data;
    uint32_t t = u.ul[0];
    u.ul[0] = CUInt32ByteSwap(u.ul[1]);
    u.ul[1] = CUInt32ByteSwap(t);
    return u.ull;
#endif
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static inline uint64_t CUInt64MakeWithBigEndianBytes(uint8_t const block[_Nonnull 8]) {
    uint64_t value = *((uint64_t *)block);
    return CUInt64ByteSwap(value);
}
static inline uint32_t CUInt32MakeWithBigEndianBytes(uint8_t const block[_Nonnull 4]) {
    uint32_t value = *((uint32_t *)block);
    return CUInt32ByteSwap(value);
}
static inline uint64_t CUInt64MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 8]) {
    return *((uint64_t *)block);
}
static inline uint32_t CUInt32MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 4]) {
    return *((uint32_t *)block);
}


static inline void CUInt64ToBigEndianBytes(uint64_t value, uint8_t bytes[_Nonnull 8]) {
    uint64_t v = CUInt64ByteSwap(value);
    *((uint64_t *)bytes) = v;
}
static inline void CUInt32ToBigEndianBytes(uint32_t value, uint8_t bytes[_Nonnull 4]) {
    uint32_t v = CUInt32ByteSwap(value);
    *((uint32_t *)bytes) = v;
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
    uint64_t value = *((uint64_t *)block);
    return CUInt64ByteSwap(value);
}
static inline uint32_t CUInt32MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 4]) {
    uint32_t value = *((uint32_t *)block);
    return CUInt32ByteSwap(value);
}


static inline void CUInt64ToBigEndianBytes(uint64_t value, uint8_t bytes[_Nonnull 8]) {
    *((uint64_t *)block) = value;
}
static inline void CUInt32ToBigEndianBytes(uint32_t value, uint8_t bytes[_Nonnull 4]) {
    *((uint32_t *)block) = value;
}
static inline void CUInt64ToLittleEndianBytes(uint64_t value, uint8_t block[_Nonnull 8]) {
    uint64_t v = CUInt64ByteSwap(value);
    *((uint64_t *)bytes) = v;
}
static inline void CUInt32ToLittleEndianBytes(uint32_t value, uint8_t block[_Nonnull 4]) {
    uint32_t v = CUInt32ByteSwap(value);
    *((uint32_t *)bytes) = v;
}

#else
#error "DEFINE BIG_ENDIAN OR LITTLE_ENDIAN"
#endif



#if defined(__x86_64__) && !defined(_MSC_VER) //clang _MSVC doesn't support GNU-style inline assembly
// MARK: -- intel 64 asm version

static inline uint64_t CUInt64RotateLeft(uint64_t word, int i) {
    __asm__("rolq %%cl,%0"
            :"=r" (word)
            :"0" (word),"c" (i));
    return word;
}

static inline uint64_t CUInt64RotateRight(uint64_t word, int i) {
    __asm__("rorq %%cl,%0"
            :"=r" (word)
            :"0" (word),"c" (i));
    return word;
}

#else /* Not x86_64  */

// MARK: -- default C version

static inline uint64_t CUInt64RotateLeft(uint64_t word, int i) {
    return ( (word<<(i&63)) | (word>>(64-(i&63))) );
}

static inline uint64_t CUInt64RotateRight(uint64_t word, int i) {
    return ( (word>>(i&63)) | (word<<(64-(i&63))) );
}

#endif

// MARK: - 32-bit Rotates

#if defined(_MSC_VER)
// MARK: -- MSVC version

#include <stdlib.h>
#if !defined(__clang__)
#pragma intrinsic(_lrotr,_lrotl)
#endif
#define    CUInt32RotateRight(x,n) _lrotr(x,n)
#define    CUInt32RotateLeft(x,n) _lrotl(x,n)

#elif (defined(__i386__) || defined(__x86_64__))
// MARK: -- intel asm version

static inline uint32_t CUInt32RotateLeft(uint32_t word, int i) {
    __asm__ ("roll %%cl,%0"
             :"=r" (word)
             :"0" (word),"c" (i));
    return word;
}

static inline uint32_t CUInt32RotateRight(uint32_t word, int i) {
    __asm__ ("rorl %%cl,%0"
             :"=r" (word)
             :"0" (word),"c" (i));
    return word;
}

#else

static inline uint32_t CUInt32RotateLeft(uint32_t word, int i) {
    return ( (word<<(i&31)) | (word>>(32-(i&31))) );
}

static inline uint32_t CUInt32RotateRight(uint32_t word, int i) {
    return ( (word>>(i&31)) | (word<<(32-(i&31))) );
}

#endif

#endif /* CDAPI_h */
