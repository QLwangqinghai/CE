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

typedef enum _CDVariant {
    CDVariantNone,
    CDVariantSHA2th224 = 1,
    CDVariantSHA2th256 = 2,
    CDVariantSHA2th384 = 3,
    CDVariantSHA2th512 = 4,
} CDVariant_e;

#define CDVariantSHA2th256BlockSize 64
#define CDVariantSHA2th512BlockSize 128


typedef struct _CDSHA2th256Context {
    size_t digestVariant;
    uint32_t values[8];
    uint64_t bitCount;
    uint32_t data[16];
    uint8_t accumulated[CDVariantSHA2th256BlockSize];
    unsigned int num,md_len;
    uint32_t accumulatedSize;
} CDSHA2th256Context_s;

typedef struct _CDSHA2th512Context {
    size_t digestVariant;
    uint64_t values[8];
    uint64_t bitCountLow;//长度的低位
    uint64_t bitCountHigh;//长度的高位
    uint64_t data[16];
    uint8_t accumulated[CDVariantSHA2th512BlockSize];
    unsigned int num,md_len;
    uint32_t accumulatedSize;
} CDSHA2th512Context_s;

typedef CDSHA2th256Context_s CDSHA2th224Context_s;
typedef CDSHA2th512Context_s CDSHA2th384Context_s;



void CDSHA2th224ContextInit(CDSHA2th256Context_s * _Nonnull context);
void CDSHA2th224Update(CDSHA2th256Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2th224Final(CDSHA2th256Context_s * _Nonnull context);
void CDSHA2th224ExportHashValue(CDSHA2th256Context_s * _Nonnull context, uint8_t * _Nonnull bytes);


void CDSHA2th256ContextInit(CDSHA2th256Context_s * _Nonnull context);
void CDSHA2th256Update(CDSHA2th256Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2th256Final(CDSHA2th256Context_s * _Nonnull context);
void CDSHA2th256ExportHashValue(CDSHA2th256Context_s * _Nonnull context, uint8_t * _Nonnull bytes);


void CDSHA2th384ContextInit(CDSHA2th512Context_s * _Nonnull context);
void CDSHA2th384Update(CDSHA2th512Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2th384Final(CDSHA2th512Context_s * _Nonnull context);
void CDSHA2th384ExportHashValue(CDSHA2th512Context_s * _Nonnull context, uint8_t * _Nonnull bytes);


void CDSHA2th512ContextInit(CDSHA2th512Context_s * _Nonnull context);
void CDSHA2th512Update(CDSHA2th512Context_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length);
void CDSHA2th512Final(CDSHA2th512Context_s * _Nonnull context);
void CDSHA2th512ExportHashValue(CDSHA2th512Context_s * _Nonnull context, uint8_t * _Nonnull bytes);



#endif /* CoreDigest_h */
