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
#include "CInteger.h"


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
#define CCDigestBlockSizeMax 144

//buffer size Buffer
#define CCDigestMd5BufferSize 128
#define CCDigestSha1BufferSize 128
#define CCDigestSha2Variant224BufferSize 128
#define CCDigestSha2Variant256BufferSize 128
#define CCDigestSha2Variant384BufferSize 256
#define CCDigestSha2Variant512BufferSize 256
#define CCDigestSha3Variant224BufferSize 144
#define CCDigestSha3VariantKeccak224BufferSize 144
#define CCDigestSha3Variant256BufferSize 136
#define CCDigestSha3VariantKeccak256BufferSize 136
#define CCDigestSha3Variant384BufferSize 104
#define CCDigestSha3VariantKeccak384BufferSize 104
#define CCDigestSha3Variant512BufferSize 72
#define CCDigestSha3VariantKeccak512BufferSize 72

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

#define CCDigestBufferAndStateSumMax 344


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

typedef struct _CCDigestInfo {
    uint32_t digestType;
    uint16_t outputSize;
    uint16_t blockSize;
    uint16_t bufferSize;
    uint16_t stateSize;
    uint16_t stateWordSize;
    uint16_t paddingLenghSize;
    void * _Nonnull initialStatusValue;
} CCDigestInfo_s;

const CCDigestInfo_s * _Nullable CCDigestInfo(CCDigestType_e type);



struct _CCDigestContext;
typedef struct _CCDigestContext CCDigestContext_s;

typedef void (*CCDigestProcess_f)(void * _Nonnull state, size_t blockCount, const void * _Nonnull in);
typedef void (*CCDigestFinish_f)(CCDigestContext_s * _Nonnull context);
typedef void (*CCDigestExportHashValue_f)(CCDigestContext_s * _Nonnull context, void * _Nonnull bytes);
typedef void (*CCDigestExportStatus_f)(CCDigestContext_s * _Nonnull context, void * _Nonnull bytes);

struct _CCDigestContext {
    uint32_t digestType;
    uint32_t accumulatedSize;
    CUInt128_t count;
    void * _Nonnull states;
    uint8_t * _Nonnull accumulated;
    CCDigestProcess_f _Nonnull process;
    CCDigestFinish_f _Nonnull finish;
    CCDigestExportHashValue_f _Nonnull exportHashValue;
    CCDigestExportHashValue_f _Nonnull exportStatus;
};

static inline void CCDigestContextAddCount(CCDigestContext_s * _Nonnull context, size_t count) {
    assert(context);
    assert(count >= 0);
    context->count = CUInt128AddUInt64(context->count, count);
}



/*
 Big-endian uint32_t, highest 1bit is endian flag, 0 for Big-endian, 1 for Little-endian, version(31bit) |
 following number using endian flag
 
 remain length(uint32_t) | digestType(uint32_t) | processed length (uint128_t) | 
 */


typedef struct _CCDigestContextArchive {
    uint32_t digestType;
    uint32_t accumulatedSize;
    CUInt128_t count;
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
