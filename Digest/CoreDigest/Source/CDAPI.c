//
//  CoreDigest.c
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CDAPI.h"

static const uint32_t CCDigestMd5InitialStatusValue[4] = {
    0x67452301UL, 0xefcdab89UL, 0x98badcfeUL, 0x10325476UL,
};
static const uint32_t CCDigestSha1InitialStatusValue[5] = {
    0x67452301UL, 0xefcdab89UL, 0x98badcfeUL, 0x10325476UL, 0xc3d2e1f0UL,
};
static const uint32_t CCDigestSha2Variant224InitialStatusValue[8] = {
    0xc1059ed8UL, 0x367cd507UL, 0x3070dd17UL, 0xf70e5939UL, 0xffc00b31UL, 0x68581511UL, 0x64f98fa7UL, 0xbefa4fa4UL,
};
static const uint32_t CCDigestSha2Variant256InitialStatusValue[8] = {
    0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL, 0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL,
};
static const uint64_t CCDigestSha2Variant384InitialStatusValue[8] = {
    0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL, 0x9159015a3070dd17ULL, 0x152fecd8f70e5939ULL, 0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL, 0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL,
};
static const uint64_t CCDigestSha2Variant512InitialStatusValue[8] = {
    0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL, 0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL,
};
static const uint8_t CCDigestSha3InitialStatusValue[CCDigestSha3StateSize] = {};

static const CCDigestInfo_s _CCDigestInfos[14] = {
    {
        .digestType = CCDigestTypeMd5,
        .outputSize = CCDigestMd5OutputSize,
        .blockSize = CCDigestMd5BlockSize,
        .bufferSize = CCDigestMd5BufferSize,
        .stateSize = CCDigestMd5StateSize,
        .stateWordSize = 4,
        .paddingLenghSize = 8,
        .initialStatusValue = (void *)(&CCDigestMd5InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha1,
        .outputSize = CCDigestSha1OutputSize,
        .blockSize = CCDigestSha1BlockSize,
        .bufferSize = CCDigestSha1BufferSize,
        .stateSize = CCDigestSha1StateSize,
        .stateWordSize = 4,
        .paddingLenghSize = 8,
        .initialStatusValue = (void *)(&CCDigestSha1InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha2Variant224,
        .outputSize = CCDigestSha2Variant224OutputSize,
        .blockSize = CCDigestSha2Variant224BlockSize,
        .bufferSize = CCDigestSha2Variant224BufferSize,
        .stateSize = CCDigestSha2Variant224StateSize,
        .stateWordSize = 4,
        .paddingLenghSize = 8,
        .initialStatusValue = (void *)(&CCDigestSha2Variant224InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha2Variant256,
        .outputSize = CCDigestSha2Variant256OutputSize,
        .blockSize = CCDigestSha2Variant256BlockSize,
        .bufferSize = CCDigestSha2Variant256BufferSize,
        .stateSize = CCDigestSha2Variant256StateSize,
        .stateWordSize = 4,
        .paddingLenghSize = 8,
        .initialStatusValue = (void *)(&CCDigestSha2Variant256InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha2Variant384,
        .outputSize = CCDigestSha2Variant384OutputSize,
        .blockSize = CCDigestSha2Variant384BlockSize,
        .bufferSize = CCDigestSha2Variant384BufferSize,
        .stateSize = CCDigestSha2Variant384StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 16,
        .initialStatusValue = (void *)(&CCDigestSha2Variant384InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha2Variant512,
        .outputSize = CCDigestSha2Variant512OutputSize,
        .blockSize = CCDigestSha2Variant512BlockSize,
        .bufferSize = CCDigestSha2Variant512BufferSize,
        .stateSize = CCDigestSha2Variant512StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 16,
        .initialStatusValue = (void *)(&CCDigestSha2Variant512InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha3Variant224,
        .outputSize = CCDigestSha3Variant224OutputSize,
        .blockSize = CCDigestSha3Variant224BlockSize,
        .bufferSize = CCDigestSha3Variant224BufferSize,
        .stateSize = CCDigestSha3Variant224StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 0,
        .initialStatusValue = (void *)(&CCDigestSha3InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha3Variant256,
        .outputSize = CCDigestSha3Variant256OutputSize,
        .blockSize = CCDigestSha3Variant256BlockSize,
        .bufferSize = CCDigestSha3Variant256BufferSize,
        .stateSize = CCDigestSha3Variant256StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 0,
        .initialStatusValue = (void *)(&CCDigestSha3InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha3Variant384,
        .outputSize = CCDigestSha3Variant384OutputSize,
        .blockSize = CCDigestSha3Variant384BlockSize,
        .bufferSize = CCDigestSha3Variant384BufferSize,
        .stateSize = CCDigestSha3Variant384StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 0,
        .initialStatusValue = (void *)(&CCDigestSha3InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha3Variant512,
        .outputSize = CCDigestSha3Variant512OutputSize,
        .blockSize = CCDigestSha3Variant512BlockSize,
        .bufferSize = CCDigestSha3Variant512BufferSize,
        .stateSize = CCDigestSha3Variant512StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 0,
        .initialStatusValue = (void *)(&CCDigestSha3InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha3VariantKeccak224,
        .outputSize = CCDigestSha3VariantKeccak224OutputSize,
        .blockSize = CCDigestSha3VariantKeccak224BlockSize,
        .bufferSize = CCDigestSha3VariantKeccak224BufferSize,
        .stateSize = CCDigestSha3VariantKeccak224StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 0,
        .initialStatusValue = (void *)(&CCDigestSha3InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha3VariantKeccak256,
        .outputSize = CCDigestSha3VariantKeccak256OutputSize,
        .blockSize = CCDigestSha3VariantKeccak256BlockSize,
        .bufferSize = CCDigestSha3VariantKeccak256BufferSize,
        .stateSize = CCDigestSha3VariantKeccak256StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 0,
        .initialStatusValue = (void *)(&CCDigestSha3InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha3VariantKeccak384,
        .outputSize = CCDigestSha3VariantKeccak384OutputSize,
        .blockSize = CCDigestSha3VariantKeccak384BlockSize,
        .bufferSize = CCDigestSha3VariantKeccak384BufferSize,
        .stateSize = CCDigestSha3VariantKeccak384StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 0,
        .initialStatusValue = (void *)(&CCDigestSha3InitialStatusValue[0]),
    },
    {
        .digestType = CCDigestTypeSha3VariantKeccak512,
        .outputSize = CCDigestSha3VariantKeccak512OutputSize,
        .blockSize = CCDigestSha3VariantKeccak512BlockSize,
        .bufferSize = CCDigestSha3VariantKeccak512BufferSize,
        .stateSize = CCDigestSha3VariantKeccak512StateSize,
        .stateWordSize = 8,
        .paddingLenghSize = 0,
        .initialStatusValue = (void *)(&CCDigestSha3InitialStatusValue[0]),
    }
};


const CCDigestInfo_s * _Nullable CCDigestInfo(CCDigestType_e type) {
    if (type > 0 && type <= 14) {
        return &(_CCDigestInfos[type - 1]);
    } else {
        return NULL;
    }
}

int32_t CDHmac(CCDigestType_e e, uint8_t const * _Nullable key, size_t keyLength, uint8_t const * _Nullable input, size_t inputLength, uint8_t * _Nullable output, size_t outputLength) {
    assert(keyLength >= 0);
    assert(inputLength >= 0);
    assert(outputLength >= 0);
    const CCDigestInfo_s * digestInfo = CCDigestInfo(e);
    if (NULL == digestInfo) {
        return -1;
    }
    size_t digestSize = digestInfo->outputSize;
    size_t blockSize = digestInfo->blockSize;
    CDAllRoundContext_u context = {};

    uint8_t opad[CCDigestBlockSizeMax] = {};
    uint8_t ipad[CCDigestBlockSizeMax] = {};
    uint8_t k[CCDigestBlockSizeMax] = {};
    uint8_t messageHash[CCDigestLengthMax] = {};
    if (keyLength > blockSize) {
        CDContextInit(&context, e);
        CDUpdate(&context, key, keyLength);
        CDFinal(&context);
        CDExportHashValue(&context, k);
    }
    
    for (size_t idx=0; idx<blockSize; idx++) {
        opad[idx] = 0x5c;
        ipad[idx] = 0x36;
    }
    for (size_t idx=0; idx<keyLength; idx++) {
        opad[idx] = key[idx] ^ opad[idx];
        ipad[idx] = key[idx] ^ ipad[idx];
    }

    CDContextInit(&context, e);
    CDUpdate(&context, ipad, blockSize);
    CDUpdate(&context, input, inputLength);
    CDFinal(&context);
    CDExportHashValue(&context, messageHash);

    CDContextInit(&context, e);
    CDUpdate(&context, opad, blockSize);
    CDUpdate(&context, messageHash, digestSize);
    CDFinal(&context);
    CDExportHashValue(&context, output);
    
    size_t size = digestSize;
    if (size > 0) {
        if (outputLength < size) {
            return -2;
        }
        switch (e) {
            case CCDigestTypeMd5: {
                CDContextInit(&context, e);
                
                return CCDigestMd5BlockSize;
            }
            case CCDigestTypeSha1:
                return CCDigestSha1BlockSize;
            case CCDigestTypeSha2Variant224:
            case CCDigestTypeSha2Variant256:
                return CCDigestSha2Variant256BlockSize;
            case CCDigestTypeSha2Variant384:
            case CCDigestTypeSha2Variant512:
                return CCDigestSha2Variant512BlockSize;
            case CCDigestTypeSha3Variant224:
            case CCDigestTypeSha3VariantKeccak224:
                return 144;
            case CCDigestTypeSha3Variant256:
            case CCDigestTypeSha3VariantKeccak256:
                return 136;
            case CCDigestTypeSha3Variant384:
            case CCDigestTypeSha3VariantKeccak384:
                return 104;
            case CCDigestTypeSha3Variant512:
            case CCDigestTypeSha3VariantKeccak512:
                return 72;
            default:
                return 0;
        }
        
    } else {
        return -1;
    }
}

//accumulatedSize < blockSize
void CCDigestContextInit(CCDigestContext_s * _Nonnull context, CCDigestType_e type, void * _Nonnull states, uint8_t * _Nonnull accumulated) {
    switch (type) {
        case CCDigestTypeMd5: {
            CCDigestContextInitMd5(context, states, accumulated);
        }
            break;
        case CCDigestTypeSha1: {
            CCDigestContextInitSha1(context, states, accumulated);
        }
            break;
        case CCDigestTypeSha2Variant224:
        case CCDigestTypeSha2Variant256:
        case CCDigestTypeSha2Variant384:
        case CCDigestTypeSha2Variant512: {
            CCDigestContextInitSha2(context, type, states, accumulated);
        }
            break;
        case CCDigestTypeSha3Variant224:
        case CCDigestTypeSha3VariantKeccak224:
        case CCDigestTypeSha3Variant256:
        case CCDigestTypeSha3VariantKeccak256:
        case CCDigestTypeSha3Variant384:
        case CCDigestTypeSha3VariantKeccak384:
        case CCDigestTypeSha3Variant512:
        case CCDigestTypeSha3VariantKeccak512: {
            CCDigestContextInitSha3(context, type, states, accumulated);
        }
            break;
        default:
            abort();
    }
    
}
void CCDigestUpdate(CCDigestContext_s * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    assert(context);
    assert(length >= 0);
    
    if (0 == length) {
        return;
    }
    assert(bytes);
    assert(context->process);

    const CCDigestInfo_s * digestInfo = CCDigestInfo(context->digestType);
    assert(digestInfo);
    
    size_t blockSize = digestInfo->blockSize;
    uint8_t const * ptr = bytes;
    if (context->accumulatedSize > 0) {
        size_t missingLength = blockSize - context->accumulatedSize;
        if (length < missingLength) {
            memcpy(context->accumulated, ptr, length);
            context->accumulatedSize += length;
            return;
        } else {
            memcpy(context->accumulated, ptr, missingLength);
            context->process(context->states, 1, context->accumulated);
            context->accumulatedSize = 0;
            ptr += missingLength;
            length -= missingLength;
            CCDigestContextAddCount(context, blockSize);
        }
    }
    uint64_t tmp = length / blockSize;
    context->process(context->states, tmp, context->accumulated);
    tmp = tmp * blockSize;
    CCDigestContextAddCount(context, tmp);
    ptr += tmp;
    tmp = length % blockSize;

    if (tmp > 0) {
        memcpy(context->accumulated, ptr, tmp);
        context->accumulatedSize += tmp;
    }
}
void CCDigestFinal(CCDigestContext_s * _Nonnull context) {
    assert(context);
    assert(context->finish);
    context->finish(context);
}
void CCDigestExportHashValue(CCDigestContext_s * _Nonnull context, uint8_t * _Nonnull bytes) {
    assert(context);
    assert(context->exportHashValue);
    context->exportHashValue(context, bytes);
}



void CDContextInit(CDAllRoundContext_u * _Nonnull context, CCDigestType_e e) {
    switch (e) {
        case CCDigestTypeMd5:
            CDMD5ContextInit(&(context->mdContext));
            break;
        case CCDigestTypeSha1:
            CDSHA1ContextInit(&(context->sha1Context));
            break;
        case CCDigestTypeSha2Variant224:
        case CCDigestTypeSha2Variant256:
        case CCDigestTypeSha2Variant384:
        case CCDigestTypeSha2Variant512:
            CDSHA2ContextInit(&(context->sha2Context), e);
            break;
        case CCDigestTypeSha3Variant224:
        case CCDigestTypeSha3VariantKeccak224:
        case CCDigestTypeSha3Variant256:
        case CCDigestTypeSha3VariantKeccak256:
        case CCDigestTypeSha3Variant384:
        case CCDigestTypeSha3VariantKeccak384:
        case CCDigestTypeSha3Variant512:
        case CCDigestTypeSha3VariantKeccak512:
            CDSHA3ContextInit(&(context->sha3Context), e);
            break;
        default:
            abort();
            break;
    }
}
void CDUpdate(CDAllRoundContext_u * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    switch (context->digestVariant) {
        case CCDigestTypeMd5:
            CDMD5Update(&(context->mdContext), bytes, length);
            break;
        case CCDigestTypeSha1:
            CDSHA1Update(&(context->sha1Context), bytes, length);
            break;
        case CCDigestTypeSha2Variant224:
        case CCDigestTypeSha2Variant256:
        case CCDigestTypeSha2Variant384:
        case CCDigestTypeSha2Variant512:
            CDSHA2Update(&(context->sha2Context), bytes, length);
            break;
        case CCDigestTypeSha3Variant224:
        case CCDigestTypeSha3VariantKeccak224:
        case CCDigestTypeSha3Variant256:
        case CCDigestTypeSha3VariantKeccak256:
        case CCDigestTypeSha3Variant384:
        case CCDigestTypeSha3VariantKeccak384:
        case CCDigestTypeSha3Variant512:
        case CCDigestTypeSha3VariantKeccak512:
            CDSHA3Update(&(context->sha3Context), bytes, length);
            break;
        default:
            abort();
            break;
    }
}
void CDFinal(CDAllRoundContext_u * _Nonnull context) {
    switch (context->digestVariant) {
        case CCDigestTypeMd5:
            CDMD5Final(&(context->mdContext));
            break;
        case CCDigestTypeSha1:
            CDSHA1Final(&(context->sha1Context));
            break;
        case CCDigestTypeSha2Variant224:
        case CCDigestTypeSha2Variant256:
        case CCDigestTypeSha2Variant384:
        case CCDigestTypeSha2Variant512:
            CDSHA2Final(&(context->sha2Context));
            break;
        case CCDigestTypeSha3Variant224:
        case CCDigestTypeSha3VariantKeccak224:
        case CCDigestTypeSha3Variant256:
        case CCDigestTypeSha3VariantKeccak256:
        case CCDigestTypeSha3Variant384:
        case CCDigestTypeSha3VariantKeccak384:
        case CCDigestTypeSha3Variant512:
        case CCDigestTypeSha3VariantKeccak512:
            CDSHA3Final(&(context->sha3Context));
            break;
        default:
            abort();
            break;
    }
}
void CDExportHashValue(CDAllRoundContext_u * _Nonnull context, uint8_t * _Nonnull bytes) {
    switch (context->digestVariant) {
        case CCDigestTypeMd5:
            CDMD5ExportHashValue(&(context->mdContext), bytes);
            break;
        case CCDigestTypeSha1:
            CDSHA1ExportHashValue(&(context->sha1Context), bytes);
            break;
        case CCDigestTypeSha2Variant224:
        case CCDigestTypeSha2Variant256:
        case CCDigestTypeSha2Variant384:
        case CCDigestTypeSha2Variant512:
            CDSHA2ExportHashValue(&(context->sha2Context), bytes);
            break;
        case CCDigestTypeSha3Variant224:
        case CCDigestTypeSha3VariantKeccak224:
        case CCDigestTypeSha3Variant256:
        case CCDigestTypeSha3VariantKeccak256:
        case CCDigestTypeSha3Variant384:
        case CCDigestTypeSha3VariantKeccak384:
        case CCDigestTypeSha3Variant512:
        case CCDigestTypeSha3VariantKeccak512:
            CDSHA3ExportHashValue(&(context->sha3Context), bytes);
            break;
        default:
            abort();
            break;
    }
}


void CCDigestContextInitSha1(CCDigestContext_s * _Nonnull context, void * _Nonnull states, uint8_t * _Nonnull accumulated) {}
void CCDigestContextInitSha2(CCDigestContext_s * _Nonnull context, CCDigestType_e type, void * _Nonnull states, uint8_t * _Nonnull accumulated) {}
void CCDigestContextInitSha3(CCDigestContext_s * _Nonnull context, CCDigestType_e type, void * _Nonnull states, uint8_t * _Nonnull accumulated) {}
