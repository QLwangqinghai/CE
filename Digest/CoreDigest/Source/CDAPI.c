//
//  CoreDigest.c
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CDAPI.h"


int32_t CDHmac(CCDigestType_e e, uint8_t const * _Nullable key, size_t keyLength, uint8_t const * _Nullable input, size_t inputLength, uint8_t * _Nullable output, size_t outputLength) {
    assert(keyLength >= 0);
    assert(inputLength >= 0);
    assert(outputLength >= 0);
    size_t digestSize = CCDigestOutputSize(e);
    size_t blockSize = CCDigestBlockSize(e);
    CDAllRoundContext_u context = {};

    if (digestSize <= 0 || blockSize <= 0) {
        return -1;
    }
    if (outputLength < digestSize) {
        return -2;
    }
    
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
    
    size_t size = CCDigestOutputSize(e);
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

    size_t blockSize = CCDigestBlockSize(context->digestType);
    assert(blockSize > 0);
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
