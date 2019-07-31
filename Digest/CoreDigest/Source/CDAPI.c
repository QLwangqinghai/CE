//
//  CoreDigest.c
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CDAPI.h"


int32_t CDHmac(CDVariant_e e, uint8_t const * _Nullable key, size_t keyLength, uint8_t const * _Nullable input, size_t inputLength, uint8_t * _Nullable output, size_t outputLength) {
    assert(keyLength >= 0);
    assert(inputLength >= 0);
    assert(outputLength >= 0);
    size_t digestSize = CDVariantDigestLength(e);
    size_t blockSize = CDVariantBlockSize(e);
    CDAllRoundContext_u context = {};

    if (digestSize <= 0 || blockSize <= 0) {
        return -1;
    }
    if (outputLength < digestSize) {
        return -2;
    }
    
    uint8_t opad[CDVariantBlockSizeMax] = {};
    uint8_t ipad[CDVariantBlockSizeMax] = {};
    uint8_t k[CDVariantBlockSizeMax] = {};
    uint8_t messageHash[CDVariantDigestLengthMax] = {};
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
    
    
    //        let ipadAndMessageHash = variant.calculateHash(ipad + bytes)
    //        let result = variant.calculateHash(opad + ipadAndMessageHash)
    //
    //        // return Array(result[0..<10]) // 80 bits
    //        return result
    
    //        self.variant = variant
    //        self.key = key
    //



    
    
    size_t size = CDVariantDigestLength(e);
    if (size > 0) {
        if (outputLength < size) {
            return -2;
        }
        switch (e) {
            case CDVariantMD5: {
                CDContextInit(&context, e);
                
                return CDVariantMD5BlockSize;
            }
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
        
    } else {
        return -1;
    }

    
//    CDVariantMD5 = 1,
//    CDVariantSHA1th160 = 2,
//    CDVariantSHA2th224 = 3,
//    CDVariantSHA2th256 = 4,
//    CDVariantSHA2th384 = 5,
//    CDVariantSHA2th512 = 6,
//    
//    CDVariantSHA3th224 = 7,
//    CDVariantSHA3th256 = 8,
//    CDVariantSHA3th384 = 9,
//    CDVariantSHA3th512 = 10,
//    CDVariantSHA3thKeccak224 = 11,
//    CDVariantSHA3thKeccak256 = 12,
//    CDVariantSHA3thKeccak384 = 13,
//    CDVariantSHA3thKeccak512 = 14,
//    
//    
//    if (0 < keyLength) {
//        assert(key);
//    }
//    size_t blockSize = CDVariantSHA2th256BlockSize;
//
//    uint8_t bytes[CDVariantSHA2th256BlockSize] = {};
//    if (keyLength > blockSize) {
//        CDSHA2th224ContextInit(context);
//        CDSHA2th224Update(context, key, keyLength);
//        CDSHA2th224Final(context);
//        CDSHA2th224ExportHashValue(context, bytes);
//    } else {
//        if (keyLength > 0) {
//            memcpy(bytes, key, keyLength);
//        }
//    }
}




//
//void CDSHA2th256ContextInitHmac(CDSHA2th256Context_s * _Nonnull context, uint8_t * _Nullable key, size_t keyLength);
//
//void CDSHA2th384ContextInitHmac(CDSHA2th384Context_s * _Nonnull context, uint8_t * _Nullable key, size_t keyLength);
//void CDSHA2th512ContextInitHmac(CDSHA2th512Context_s * _Nonnull context);
//
//
//public final class HMAC: Authenticator {
//    public enum Error: Swift.Error {
//    case authenticateError
//    case invalidInput
//    }
//
//    public enum Variant {
//    case sha1, sha256, sha384, sha512, md5
//
//        var digestLength: Int {
//            switch self {
//            case .sha1:
//                return SHA1.digestLength
//            case .sha256:
//                return SHA2.Variant.sha256.digestLength
//            case .sha384:
//                return SHA2.Variant.sha384.digestLength
//            case .sha512:
//                return SHA2.Variant.sha512.digestLength
//            case .md5:
//                return MD5.digestLength
//            }
//        }
//
//        func calculateHash(_ bytes: Array<UInt8>) -> Array<UInt8> {
//            switch self {
//            case .sha1:
//                return Digest.sha1(bytes)
//            case .sha256:
//                return Digest.sha256(bytes)
//            case .sha384:
//                return Digest.sha384(bytes)
//            case .sha512:
//                return Digest.sha512(bytes)
//            case .md5:
//                return Digest.md5(bytes)
//            }
//        }
//
//        func blockSize() -> Int {
//            switch self {
//            case .md5:
//                return MD5.blockSize
//            case .sha1, .sha256:
//                return 64
//            case .sha384, .sha512:
//                return 128
//            }
//        }
//    }
//
//    var key: Array<UInt8>
//    let variant: Variant
//
//    public init(key: Array<UInt8>, variant: HMAC.Variant = .md5) {
//        self.variant = variant
//        self.key = key
//
//        if key.count > variant.blockSize() {
//            let hash = variant.calculateHash(key)
//            self.key = hash
//        }
//
//        if key.count < variant.blockSize() {
//            self.key = ZeroPadding().add(to: key, blockSize: variant.blockSize())
//        }
//    }
//
//    // MARK: Authenticator
//
//    public func authenticate(_ bytes: Array<UInt8>) throws -> Array<UInt8> {
//        var opad = Array<UInt8>(repeating: 0x5c, count: variant.blockSize())
//        for idx in key.indices {
//            opad[idx] = key[idx] ^ opad[idx]
//        }
//        var ipad = Array<UInt8>(repeating: 0x36, count: variant.blockSize())
//        for idx in key.indices {
//            ipad[idx] = key[idx] ^ ipad[idx]
//        }
//
//        let ipadAndMessageHash = variant.calculateHash(ipad + bytes)
//        let result = variant.calculateHash(opad + ipadAndMessageHash)
//
//        // return Array(result[0..<10]) // 80 bits
//        return result
//    }
//}



void CDContextInit(CDAllRoundContext_u * _Nonnull context, CDVariant_e e) {
    switch (e) {
        case CDVariantMD5:
            CDMD5ContextInit(&(context->mdContext));
            break;
        case CDVariantSHA1th160:
            CDSHA1ContextInit(&(context->sha1Context));
            break;
        case CDVariantSHA2th224:
        case CDVariantSHA2th256:
        case CDVariantSHA2th384:
        case CDVariantSHA2th512:
            CDSHA2ContextInit(&(context->sha2Context), e);
            break;
        case CDVariantSHA3th224:
        case CDVariantSHA3thKeccak224:
        case CDVariantSHA3th256:
        case CDVariantSHA3thKeccak256:
        case CDVariantSHA3th384:
        case CDVariantSHA3thKeccak384:
        case CDVariantSHA3th512:
        case CDVariantSHA3thKeccak512:
            CDSHA3ContextInit(&(context->sha3Context), e);
            break;
        default:
            abort();
            break;
    }
}
void CDUpdate(CDAllRoundContext_u * _Nonnull context, uint8_t const * _Nonnull bytes, size_t length) {
    switch (context->digestVariant) {
        case CDVariantMD5:
            CDMD5Update(&(context->mdContext), bytes, length);
            break;
        case CDVariantSHA1th160:
            CDSHA1Update(&(context->sha1Context), bytes, length);
            break;
        case CDVariantSHA2th224:
        case CDVariantSHA2th256:
        case CDVariantSHA2th384:
        case CDVariantSHA2th512:
            CDSHA2Update(&(context->sha2Context), bytes, length);
            break;
        case CDVariantSHA3th224:
        case CDVariantSHA3thKeccak224:
        case CDVariantSHA3th256:
        case CDVariantSHA3thKeccak256:
        case CDVariantSHA3th384:
        case CDVariantSHA3thKeccak384:
        case CDVariantSHA3th512:
        case CDVariantSHA3thKeccak512:
            CDSHA3Update(&(context->sha3Context), bytes, length);
            break;
        default:
            abort();
            break;
    }
}
void CDFinal(CDAllRoundContext_u * _Nonnull context) {
    switch (context->digestVariant) {
        case CDVariantMD5:
            CDMD5Final(&(context->mdContext));
            break;
        case CDVariantSHA1th160:
            CDSHA1Final(&(context->sha1Context));
            break;
        case CDVariantSHA2th224:
        case CDVariantSHA2th256:
        case CDVariantSHA2th384:
        case CDVariantSHA2th512:
            CDSHA2Final(&(context->sha2Context));
            break;
        case CDVariantSHA3th224:
        case CDVariantSHA3thKeccak224:
        case CDVariantSHA3th256:
        case CDVariantSHA3thKeccak256:
        case CDVariantSHA3th384:
        case CDVariantSHA3thKeccak384:
        case CDVariantSHA3th512:
        case CDVariantSHA3thKeccak512:
            CDSHA3Final(&(context->sha3Context));
            break;
        default:
            abort();
            break;
    }
}
void CDExportHashValue(CDAllRoundContext_u * _Nonnull context, uint8_t * _Nonnull bytes) {
    switch (context->digestVariant) {
        case CDVariantMD5:
            CDMD5ExportHashValue(&(context->mdContext), bytes);
            break;
        case CDVariantSHA1th160:
            CDSHA1ExportHashValue(&(context->sha1Context), bytes);
            break;
        case CDVariantSHA2th224:
        case CDVariantSHA2th256:
        case CDVariantSHA2th384:
        case CDVariantSHA2th512:
            CDSHA2ExportHashValue(&(context->sha2Context), bytes);
            break;
        case CDVariantSHA3th224:
        case CDVariantSHA3thKeccak224:
        case CDVariantSHA3th256:
        case CDVariantSHA3thKeccak256:
        case CDVariantSHA3th384:
        case CDVariantSHA3thKeccak384:
        case CDVariantSHA3th512:
        case CDVariantSHA3thKeccak512:
            CDSHA3ExportHashValue(&(context->sha3Context), bytes);
            break;
        default:
            abort();
            break;
    }
}
