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
#if defined(__GNUC__)
#include <stdint.h>
#include <stdbool.h>
#endif

typedef enum _CCDigestType {
    CCDigestTypeUnknown = 0,
    
    CCDigestTypeMd5 = 1,
    
    CCDigestTypeSha1 = 2,
    
    CCDigestTypeSha2Variant224 = 3,
    CCDigestTypeSha2Variant256 = 4,
    CCDigestTypeSha2Variant384 = 5,
    CCDigestTypeSha2Variant512 = 6,
    
    CCDigestTypeSha3Variant224 = 7,
    CCDigestTypeSha3Variant256 = 8,
    CCDigestTypeSha3Variant384 = 9,
    CCDigestTypeSha3Variant512 = 10,
    CCDigestTypeSha3VariantKeccak224 = 11,
    CCDigestTypeSha3VariantKeccak256 = 12,
    CCDigestTypeSha3VariantKeccak384 = 13,
    CCDigestTypeSha3VariantKeccak512 = 14,
} CCDigestType_e;

static inline _Bool CCDigestTypeIsValid(CCDigestType_e type) {
    return (0 < type && type <= 14);
}

//block size
#define CCDigestMd5BlockSize 64
#define CCDigestSha1BlockSize 64
#define CCDigestSha2Variant224BlockSize 64
#define CCDigestSha2Variant256BlockSize 64
#define CCDigestSha2Variant384BlockSize 128
#define CCDigestSha2Variant512BlockSize 128
#define CCDigestSha3Variant224BlockSize 144
#define CCDigestSha3VariantKeccak224BlockSize 144
#define CCDigestSha3Variant256BlockSize 136
#define CCDigestSha3VariantKeccak256BlockSize 136
#define CCDigestSha3Variant384BlockSize 104
#define CCDigestSha3VariantKeccak384BlockSize 104
#define CCDigestSha3Variant512BlockSize 72
#define CCDigestSha3VariantKeccak512BlockSize 72

//state size
#define CCDigestMd5StateSize 16
#define CCDigestSha1StateSize 20
#define CCDigestSha2Variant224StateSize 32
#define CCDigestSha2Variant256StateSize 32
#define CCDigestSha2Variant384StateSize 64
#define CCDigestSha2Variant512StateSize 64
#define CCDigestSha3StateSize 200
#define CCDigestSha3Variant224StateSize CCDigestSha3StateSize
#define CCDigestSha3VariantKeccak224StateSize CCDigestSha3StateSize
#define CCDigestSha3Variant256StateSize CCDigestSha3StateSize
#define CCDigestSha3VariantKeccak256StateSize CCDigestSha3StateSize
#define CCDigestSha3Variant384StateSize CCDigestSha3StateSize
#define CCDigestSha3VariantKeccak384StateSize CCDigestSha3StateSize
#define CCDigestSha3Variant512StateSize CCDigestSha3StateSize
#define CCDigestSha3VariantKeccak512StateSize CCDigestSha3StateSize

//out size
#define CCDigestMd5OutputSize 16
#define CCDigestSha1OutputSize 20
#define CCDigestSha224OutputSize 28
#define CCDigestSha256OutputSize 32
#define CCDigestSha384OutputSize 48
#define CCDigestSha512OutputSize 64
#define CCDigestSha2Variant224OutputSize CCDigestSha224OutputSize
#define CCDigestSha2Variant256OutputSize CCDigestSha256OutputSize
#define CCDigestSha2Variant384OutputSize CCDigestSha384OutputSize
#define CCDigestSha2Variant512OutputSize CCDigestSha512OutputSize
#define CCDigestSha3Variant224OutputSize CCDigestSha224OutputSize
#define CCDigestSha3VariantKeccak224OutputSize CCDigestSha224OutputSize
#define CCDigestSha3Variant256OutputSize CCDigestSha256OutputSize
#define CCDigestSha3VariantKeccak256OutputSize CCDigestSha256OutputSize
#define CCDigestSha3Variant384OutputSize CCDigestSha384OutputSize
#define CCDigestSha3VariantKeccak384OutputSize CCDigestSha384OutputSize
#define CCDigestSha3Variant512OutputSize CCDigestSha512OutputSize
#define CCDigestSha3VariantKeccak512OutputSize CCDigestSha512OutputSize


#define CCDigestLengthMax 64


static inline size_t CCDigestOutputSize(CCDigestType_e e) {
    switch (e) {
        case CCDigestTypeMd5:
            return 16;
        case CCDigestTypeSha1:
            return 20;
        case CCDigestTypeSha2Variant224:
        case CCDigestTypeSha3Variant224:
        case CCDigestTypeSha3VariantKeccak224:
            return 28;
        case CCDigestTypeSha2Variant256:
        case CCDigestTypeSha3Variant256:
        case CCDigestTypeSha3VariantKeccak256:
            return 32;
        case CCDigestTypeSha2Variant384:
        case CCDigestTypeSha3Variant384:
        case CCDigestTypeSha3VariantKeccak384:
            return 48;
        case CCDigestTypeSha2Variant512:
        case CCDigestTypeSha3Variant512:
        case CCDigestTypeSha3VariantKeccak512:
            return 64;
        default:
            return 0;
    }
}

#define CCDigestBlockSizeMax 144

static inline size_t CCDigestBlockSize(CCDigestType_e e) {
    switch (e) {
        case CCDigestTypeMd5:
            return CCDigestMd5BlockSize;
        case CCDigestTypeSha1:
            return CCDigestSha1BlockSize;
        case CCDigestTypeSha2Variant224:
        case CCDigestTypeSha2Variant256:
            return CCDigestSha2Variant256BlockSize;
        case CCDigestTypeSha2Variant384:
        case CCDigestTypeSha2Variant512:
            return CCDigestSha2Variant512BlockSize;
        case CCDigestTypeSha3Variant224:
            return CCDigestSha3Variant224BlockSize;
        case CCDigestTypeSha3VariantKeccak224:
            return CCDigestSha3VariantKeccak224BlockSize;
        case CCDigestTypeSha3Variant256:
            return CCDigestSha3Variant256BlockSize;
        case CCDigestTypeSha3VariantKeccak256:
            return CCDigestSha3VariantKeccak256BlockSize;
        case CCDigestTypeSha3Variant384:
            return CCDigestSha3Variant384BlockSize;
        case CCDigestTypeSha3VariantKeccak384:
            return CCDigestSha3VariantKeccak384BlockSize;
        case CCDigestTypeSha3Variant512:
            return CCDigestSha3Variant512BlockSize;
        case CCDigestTypeSha3VariantKeccak512:
            return CCDigestSha3VariantKeccak512BlockSize;
        default:
            return 0;
    }
}

static inline size_t CCDigestStatusSize(CCDigestType_e e) {
    switch (e) {
        case CCDigestTypeMd5:
            return CCDigestMd5StateSize;
        case CCDigestTypeSha1:
            return CCDigestSha1StateSize;
        case CCDigestTypeSha2Variant224:
            return CCDigestSha2Variant224StateSize;
        case CCDigestTypeSha2Variant256:
            return CCDigestSha2Variant256StateSize;
        case CCDigestTypeSha2Variant384:
            return CCDigestSha2Variant384StateSize;
        case CCDigestTypeSha2Variant512:
            return CCDigestSha2Variant512StateSize;
        case CCDigestTypeSha3Variant224:
        case CCDigestTypeSha3VariantKeccak224:
        case CCDigestTypeSha3Variant256:
        case CCDigestTypeSha3VariantKeccak256:
        case CCDigestTypeSha3Variant384:
        case CCDigestTypeSha3VariantKeccak384:
        case CCDigestTypeSha3Variant512:
        case CCDigestTypeSha3VariantKeccak512:
            return CCDigestSha3StateSize;
        default:
            return 0;
    }
}

static inline size_t CCDigestHashWordSize(CCDigestType_e e) {
    switch (e) {
        case CCDigestTypeMd5:
        case CCDigestTypeSha1:
        case CCDigestTypeSha2Variant224:
        case CCDigestTypeSha2Variant256:
            return 4;
        case CCDigestTypeSha2Variant384:
        case CCDigestTypeSha2Variant512:
        case CCDigestTypeSha3Variant224:
        case CCDigestTypeSha3VariantKeccak224:
        case CCDigestTypeSha3Variant256:
        case CCDigestTypeSha3VariantKeccak256:
        case CCDigestTypeSha3Variant384:
        case CCDigestTypeSha3VariantKeccak384:
        case CCDigestTypeSha3Variant512:
        case CCDigestTypeSha3VariantKeccak512:
            return 8;
        default:
            return 0;
    }
}


static inline size_t CCDigestPaddingBitLengthByteCount(CCDigestType_e e) {
    switch (e) {
        case CCDigestTypeMd5:
            return 8;
        case CCDigestTypeSha1:
            return 8;
        case CCDigestTypeSha2Variant224:
            return 8;
        case CCDigestTypeSha2Variant256:
            return 8;
        case CCDigestTypeSha2Variant384:
            return 16;
        case CCDigestTypeSha2Variant512:
            return 16;
        case CCDigestTypeSha3Variant224:
        case CCDigestTypeSha3VariantKeccak224:
        case CCDigestTypeSha3Variant256:
        case CCDigestTypeSha3VariantKeccak256:
        case CCDigestTypeSha3Variant384:
        case CCDigestTypeSha3VariantKeccak384:
        case CCDigestTypeSha3Variant512:
        case CCDigestTypeSha3VariantKeccak512:
            return 0;
        default:
            return 0;
    }
}

typedef struct _CCDigestInfo {
    uint32_t digestType;
    uint16_t blockSize;
    uint16_t outputSize;
    uint16_t stateSize;
    uint8_t stateWordSize;
    uint8_t paddingLenghSize;
    void * _Nullable initialHashValue;
} CCDigestInfo_s;


struct _CCDigestContext;
typedef struct _CCDigestContext CCDigestContext_s;

typedef void (*CCDigestProcess_f)(void * _Nonnull state, size_t blockCount, const void * _Nonnull in);
typedef void (*CCDigestFinish_f)(CCDigestContext_s * _Nonnull context);
typedef void (*CCDigestExportHashValue_f)(CCDigestContext_s * _Nonnull context, void * _Nonnull bytes);

struct _CCDigestContext {
    uint32_t digestType;
    uint32_t accumulatedSize;
    uint64_t countLow;//长度的低位
    uint64_t countHigh;//长度的高位
    void * _Nonnull states;
    uint8_t * _Nonnull accumulated;
    CCDigestProcess_f _Nonnull process;
    CCDigestFinish_f _Nonnull finish;
    CCDigestExportHashValue_f _Nonnull exportHashValue;
};

static inline void CCDigestContextAddCount(CCDigestContext_s * _Nonnull context, size_t count) {
    assert(context);
    assert(count >= 0);
    uint64_t countLow = context->countLow + count;
    if (countLow < context->countLow) {
        context->countHigh += 1;
    }
    context->countLow = countLow;
}



/*
 Big-endian uint32_t, highest 1bit is endian flag, 0 for Big-endian, 1 for Little-endian, version(31bit) |
 following number using endian flag
 
 remain length(uint32_t) | digestType(uint32_t) | processed length (uint128_t) | 
 */


typedef struct _CCDigestContextArchive {
    uint32_t digestType;
    uint32_t accumulatedSize;
    uint64_t countLow;//长度的低位
    uint64_t countHigh;//长度的高位
    void * _Nonnull states;
    uint8_t * _Nonnull accumulated;
} CCDigestContextArchive_s;


//accumulatedSize < blockSize
void CCDigestContextInit(CCDigestContext_s * _Nonnull context, CCDigestType_e type, void * _Nonnull states, uint8_t * _Nonnull accumulated);
void CCDigestContextInitMd5(CCDigestContext_s * _Nonnull context, void * _Nonnull states, uint8_t * _Nonnull accumulated);
void CCDigestContextInitSha1(CCDigestContext_s * _Nonnull context, void * _Nonnull states, uint8_t * _Nonnull accumulated);
void CCDigestContextInitSha2(CCDigestContext_s * _Nonnull context, CCDigestType_e type, void * _Nonnull states, uint8_t * _Nonnull accumulated);
void CCDigestContextInitSha3(CCDigestContext_s * _Nonnull context, CCDigestType_e type, void * _Nonnull states, uint8_t * _Nonnull accumulated);

void CCDigestUpdate(CCDigestContext_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CCDigestFinal(CCDigestContext_s * _Nonnull context);
void CCDigestExportHashValue(CCDigestContext_s * _Nonnull context, uint8_t * _Nonnull bytes);







typedef struct _CDMD5Context {
    size_t digestVariant;
    size_t accumulatedSize;
    uint32_t values[4];
    uint64_t bitCount;
    uint8_t accumulated[CCDigestMd5BlockSize];
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
    uint8_t accumulated[CCDigestSha1BlockSize];
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

void CDSHA2ContextInit(CDSHA2Context_s * _Nonnull context, CCDigestType_e e);
void CDSHA2Update(CDSHA2Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2Final(CDSHA2Context_s * _Nonnull context);
void CDSHA2ExportHashValue(CDSHA2Context_s * _Nonnull context, uint8_t * _Nonnull bytes);


typedef struct _CDSHA3Context {
    size_t digestVariant;
    size_t accumulatedSize;
    uint64_t states[25];
    uint8_t accumulated[144];
} CDSHA3Context_s;

void CDSHA3ContextInit(CDSHA3Context_s * _Nonnull context, CCDigestType_e e);
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


void CDContextInit(CDAllRoundContext_u * _Nonnull context, CCDigestType_e e);
void CDUpdate(CDAllRoundContext_u * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDFinal(CDAllRoundContext_u * _Nonnull context);
void CDExportHashValue(CDAllRoundContext_u * _Nonnull context, uint8_t * _Nonnull bytes);


int32_t CDHmac(CCDigestType_e e, uint8_t const * _Nullable key, size_t keyLength, uint8_t const * _Nullable input, size_t inputLength, uint8_t * _Nullable output, size_t outputLength);


#endif /* CDAPI_h */
