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

typedef enum SHA2Variant {
    SHA2VariantNone,
    SHA2Variant224 = 1,
    SHA2Variant256 = 2,
    SHA2Variant384 = 3,
    SHA2Variant512 = 4,
} SHA2Variant_e;

#define SHA2Variant256BlockSize 64
#define SHA2Variant512BlockSize 128

static inline size_t SHA2VariantDigestLength(SHA2Variant_e e) {
    switch (e) {
        case SHA2Variant224:
            return 224/8;//28
        case SHA2Variant256:
            return 256/8;//32
        case SHA2Variant384:
            return 384/8;//4
        case SHA2Variant512:
            return 512/8;
        default:
            return 0;
    }
}

static inline size_t SHA2VariantBlockSize(SHA2Variant_e e) {
    switch (e) {
        case SHA2Variant224:
        case SHA2Variant256:
            return SHA2Variant256BlockSize;
        case SHA2Variant384:
        case SHA2Variant512:
            return SHA2Variant512BlockSize;
        default:
            return 0;
    }
}

static inline uint64_t SHAReadUInt64(uint8_t block[_Nonnull 8]) {
    return (((uint64_t)block[0]) << 56) | (((uint64_t)block[1]) << 48) | (((uint64_t)block[2]) << 40) | (((uint64_t)block[3]) << 32) | (((uint64_t)block[4]) << 24) | (((uint64_t)block[5]) << 16) | (((uint64_t)block[6]) << 8) | ((uint64_t)block[7]);
}

static inline uint32_t SHAReadUInt32(uint8_t block[_Nonnull 4]) {
    return (((uint32_t)block[0]) << 24) | (((uint32_t)block[1]) << 16) | (((uint32_t)block[2]) << 8) | ((uint32_t)block[3]);
}

static inline uint64_t SHAUInt64RotateRight(uint64_t value, uint64_t by) {
    return (value >> by) | (value << (64 - by));
}
static inline uint32_t SHAUInt32RotateRight(uint32_t value, uint32_t by) {
    return (value >> by) | (value << (32 - by));
}

static inline void SHA2Process64(uint8_t block[_Nonnull 128], uint64_t currentHash[_Nonnull 8]) {
    // break chunk into sixteen 64-bit words M[j], 0 ≤ j ≤ 15, big-endian
    // Extend the sixteen 64-bit words into eighty 64-bit words:
    size_t const kCount = 80;

    static const uint64_t k[kCount] = {
        0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
         0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
         0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
         0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
         0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
         0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
         0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
         0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
         0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
         0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
         0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
         0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
         0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
         0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
         0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
         0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL,
    };
    
    uint64_t M[kCount] = {0};
    uint64_t * hh = currentHash;
    uint8_t * iter = block;
    size_t idx = 0;
    for (; idx<16; idx++) {
        M[idx] = SHAReadUInt64(iter);
        iter += 8;
    }

    for (; idx<kCount; idx++) {
        uint64_t s0 = SHAUInt64RotateRight(M[idx - 15], 1) ^ SHAUInt64RotateRight(M[idx - 15], 8) ^ (M[idx - 15] >> 7);
        uint64_t s1 = SHAUInt64RotateRight(M[idx - 2], 19) ^ SHAUInt64RotateRight(M[idx - 2], 61) ^ (M[idx - 2] >> 6);
        M[idx] = M[idx - 16] + s0 + M[idx - 7] + s1;
    }
    
    uint64_t A = hh[0];
    uint64_t B = hh[1];
    uint64_t C = hh[2];
    uint64_t D = hh[3];
    uint64_t E = hh[4];
    uint64_t F = hh[5];
    uint64_t G = hh[6];
    uint64_t H = hh[7];
    
    // Main loop
    for (size_t j=0; j<kCount; j++) {
        uint64_t s0 = SHAUInt64RotateRight(A, 28) ^ SHAUInt64RotateRight(A, 34) ^ SHAUInt64RotateRight(A, 39);
        uint64_t maj = (A & B) ^ (A & C) ^ (B & C);
        uint64_t t2 = s0 + maj;
        uint64_t s1 = SHAUInt64RotateRight(E, 14) ^ SHAUInt64RotateRight(E, 18) ^ SHAUInt64RotateRight(E, 41);
        uint64_t ch = (E & F) ^ ((~E) & G);
        uint64_t t1 = H + s1 + ch + k[j] + M[j];
        
        H = G;
        G = F;
        F = E;
        E = D + t1;
        D = C;
        C = B;
        B = A;
        A = t1 + t2;
    }
    
    hh[0] = (hh[0] + A);
    hh[1] = (hh[1] + B);
    hh[2] = (hh[2] + C);
    hh[3] = (hh[3] + D);
    hh[4] = (hh[4] + E);
    hh[5] = (hh[5] + F);
    hh[6] = (hh[6] + G);
    hh[7] = (hh[7] + H);
}

// mutating currentHash in place is way faster than returning new result
static inline void SHA2Process32(uint8_t block[_Nonnull 64], uint32_t currentHash[_Nonnull 8]) {
    // break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15, big-endian
    // Extend the sixteen 32-bit words into sixty-four 32-bit words:
    size_t const kCount = 64;
    
    static const uint32_t k[kCount] = {
         0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
         0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
         0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
         0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
         0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
         0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
         0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
         0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL,
    };
    uint32_t M[kCount] = {0};
    uint32_t * hh = currentHash;
    uint8_t * iter = block;
    size_t idx = 0;
    for (; idx<16; idx++) {
        M[idx] = SHAReadUInt32(iter);
        iter += 4;
    }
    
    for (; idx<kCount; idx++) {
        uint32_t s0 = SHAUInt32RotateRight(M[idx - 15], 7) ^ SHAUInt32RotateRight(M[idx - 15], 18) ^ (M[idx - 15] >> 3);
        uint32_t s1 = SHAUInt32RotateRight(M[idx - 2], 17) ^ SHAUInt32RotateRight(M[idx - 2], 19) ^ (M[idx - 2] >> 10);
        M[idx] = M[idx - 16] + s0 + M[idx - 7] + s1;
    }

    uint32_t A = hh[0];
    uint32_t B = hh[1];
    uint32_t C = hh[2];
    uint32_t D = hh[3];
    uint32_t E = hh[4];
    uint32_t F = hh[5];
    uint32_t G = hh[6];
    uint32_t H = hh[7];
    
    // Main loop
    for (size_t j=0; j<kCount; j++) {
        uint32_t s0 = SHAUInt32RotateRight(A, 2) ^ SHAUInt32RotateRight(A, 13) ^ SHAUInt32RotateRight(A, 22);
        uint32_t maj = (A & B) ^ (A & C) ^ (B & C);
        uint32_t t2 = s0 + maj;
        uint32_t s1 = SHAUInt32RotateRight(E, 6) ^ SHAUInt32RotateRight(E, 11) ^ SHAUInt32RotateRight(E, 25);
        uint32_t ch = (E & F) ^ ((~E) & G);
        uint32_t t1 = H + s1 + ch + k[j] + M[j];
        
        H = G;
        G = F;
        F = E;
        E = D + t1;
        D = C;
        C = B;
        B = A;
        A = t1 + t2;
    }
    
    hh[0] = (hh[0] + A);
    hh[1] = (hh[1] + B);
    hh[2] = (hh[2] + C);
    hh[3] = (hh[3] + D);
    hh[4] = (hh[4] + E);
    hh[5] = (hh[5] + F);
    hh[6] = (hh[6] + G);
    hh[7] = (hh[7] + H);
}





//    fileprivate var h: Array<UInt64> {
//        switch self {
//        case .sha224:
//            return [0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4]
//        case .sha256:
//            return [0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19]
//        case .sha384:
//            return [0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939, 0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4]
//        case .sha512:
//            return [0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179]
//        }
//    }
//
//    fileprivate var finalLength: Int {
//        switch self {
//        case .sha224:
//            return 7
//        case .sha384:
//            return 6
//        default:
//            return Int.max
//        }
//    }
}

extension SHA2: Updatable {
    public func update(withBytes bytes: ArraySlice<UInt8>, isLast: Bool = false) throws -> Array<UInt8> {
        accumulated += bytes
        
        if isLast {
            let lengthInBits = (processedBytesTotalCount + accumulated.count) * 8
            let lengthBytes = lengthInBits.bytes(totalBytes: blockSize / 8) // A 64-bit/128-bit representation of b. blockSize fit by accident.
            
            // Step 1. Append padding
            bitPadding(to: &accumulated, blockSize: blockSize, allowance: blockSize / 8)
            
            // Step 2. Append Length a 64-bit representation of lengthInBits
            accumulated += lengthBytes
        }
        
        var processedBytes = 0
        for chunk in accumulated.batched(by: blockSize) {
            if isLast || (accumulated.count - processedBytes) >= blockSize {
                switch variant {
                case .sha224, .sha256:
                    process32(block: chunk, currentHash: &accumulatedHash32)
                case .sha384, .sha512:
                    process64(block: chunk, currentHash: &accumulatedHash64)
                }
                processedBytes += chunk.count
            }
        }
        accumulated.removeFirst(processedBytes)
        processedBytesTotalCount += processedBytes
        
        // output current hash
        var result = Array<UInt8>(repeating: 0, count: variant.digestLength)
        switch variant {
        case .sha224, .sha256:
            var pos = 0
            for idx in 0..<accumulatedHash32.count where idx < variant.finalLength {
                let h = accumulatedHash32[idx]
                result[pos + 0] = UInt8((h >> 24) & 0xff)
                result[pos + 1] = UInt8((h >> 16) & 0xff)
                result[pos + 2] = UInt8((h >> 8) & 0xff)
                result[pos + 3] = UInt8(h & 0xff)
                pos += 4
            }
        case .sha384, .sha512:
            var pos = 0
            for idx in 0..<accumulatedHash64.count where idx < variant.finalLength {
                let h = accumulatedHash64[idx]
                result[pos + 0] = UInt8((h >> 56) & 0xff)
                result[pos + 1] = UInt8((h >> 48) & 0xff)
                result[pos + 2] = UInt8((h >> 40) & 0xff)
                result[pos + 3] = UInt8((h >> 32) & 0xff)
                result[pos + 4] = UInt8((h >> 24) & 0xff)
                result[pos + 5] = UInt8((h >> 16) & 0xff)
                result[pos + 6] = UInt8((h >> 8) & 0xff)
                result[pos + 7] = UInt8(h & 0xff)
                pos += 8
            }
        }
        
        // reset hash value for instance
        if isLast {
            switch variant {
            case .sha224, .sha256:
                accumulatedHash32 = variant.h.lazy.map { UInt32($0) } // FIXME: UInt64 for process64
            case .sha384, .sha512:
                accumulatedHash64 = variant.h
            }
        }
        
        return result
        }
}




typedef struct _SHA2State256 {
    uint32_t h[8];
    uint64_t length;
    uint32_t data[16];
    uint8_t accumulated[SHA2Variant256BlockSize];
    unsigned int num,md_len;
    uint32_t accumulatedSize;
} SHA2State256_s;

typedef struct _SHA2State512 {
    uint64_t h[8];
    uint64_t lengthLow;//长度的低位
    uint64_t lengthHigh;//长度的高位
    union {
        uint64_t    d[16];
        unsigned char    p[SHA512_CBLOCK];
    } u;
    uint8_t accumulated[SHA2Variant512BlockSize];
    unsigned int num,md_len;
    uint32_t accumulatedSize;
} SHA2State512_s;



#endif /* CoreDigest_h */
