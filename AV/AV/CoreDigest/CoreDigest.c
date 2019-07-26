//
//  CoreDigest.c
//  AV
//
//  Created by vector on 2019/7/19.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CoreDigest.h"



//public init(key: Array<UInt8>, variant: HMAC.Variant = .md5) {
//    self.variant = variant
//    self.key = key
//
//    if key.count > variant.blockSize() {
//        let hash = variant.calculateHash(key)
//        self.key = hash
//    }
//
//    if key.count < variant.blockSize() {
//        self.key = ZeroPadding().add(to: key, blockSize: variant.blockSize())
//    }
//}


//#define CDVariantSHA2th256BlockSize 64
//#define CDVariantSHA2th512BlockSize 128
void CDHmacSHA2th224Authenticate(CDSHA2th224Context_s * _Nonnull context, uint8_t const * _Nullable key, size_t keyLength) {
    assert(keyLength >= 0);

    if (0 < keyLength) {
        assert(key);
    }
    size_t blockSize = CDVariantSHA2th256BlockSize;

    uint8_t bytes[CDVariantSHA2th256BlockSize] = {};
    if (keyLength > blockSize) {
        CDSHA2th224ContextInit(context);
        CDSHA2th224Update(context, key, keyLength);
        CDSHA2th224Final(context);
        CDSHA2th224ExportHashValue(context, bytes);
    } else {
        if (keyLength > 0) {
            memcpy(bytes, key, keyLength);
        }
    }
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
