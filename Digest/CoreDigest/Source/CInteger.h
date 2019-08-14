//
//  CInteger.h
//  CoreDigest
//
//  Created by vector on 2019/8/11.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CInteger_h
#define CInteger_h

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

typedef struct _CUInt128 {
    uint8_t content[16];
} CUInt128_t;

extern const CUInt128_t CUInt128Zero;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define CUInt128High64Offset 8
#define CUInt128Low64Offset 0
#elif __BYTE_ORDER__== __ORDER_BIG_ENDIAN__
#define CUInt128High64Offset 0
#define CUInt128Low64Offset 8
#else
#error "DEFINE BIG_ENDIAN OR LITTLE_ENDIAN"
#endif

#define _kCUInt128GetLow64Ptr(a) ((uint64_t *)(&((a).content[CUInt128Low64Offset])))
#define _kCUInt128GetHigh64Ptr(a) ((uint64_t *)(&((a).content[CUInt128High64Offset])))

static inline uint64_t * _Nonnull _CUInt128GetLow64Ptr(CUInt128_t * _Nonnull a) {
    return ((uint64_t *)(&(a->content[CUInt128Low64Offset])));
}
static inline uint64_t * _Nonnull _CUInt128GetHigh64Ptr(CUInt128_t * _Nonnull a) {
    return ((uint64_t *)(&(a->content[CUInt128High64Offset])));
}

//~a
static inline CUInt128_t CUInt128Not(CUInt128_t a) {
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    *l = ~(*l);
    *h = ~(*h);
    return a;
}

//a | b
static inline CUInt128_t CUInt128Or(CUInt128_t a, CUInt128_t b) {
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    *l = (*l) | (*_kCUInt128GetLow64Ptr(b));
    *h = (*h) | (*_kCUInt128GetHigh64Ptr(b));
    return a;
}

//a & b
static inline CUInt128_t CUInt128And(CUInt128_t a, CUInt128_t b) {
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    *l = (*l) & (*_kCUInt128GetLow64Ptr(b));
    *h = (*h) & (*_kCUInt128GetHigh64Ptr(b));
    return a;
}

//a ^ b
static inline CUInt128_t CUInt128Xor(CUInt128_t a, CUInt128_t b) {
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    *l = (*l) ^ (*_kCUInt128GetLow64Ptr(b));
    *h = (*h) ^ (*_kCUInt128GetHigh64Ptr(b));
    return a;
}

static inline CUInt128_t CUInt128MakeWithUInt64(uint64_t high, uint64_t low) {
    CUInt128_t v = CUInt128Zero;
    *(_kCUInt128GetLow64Ptr(v)) = low;
    *(_kCUInt128GetHigh64Ptr(v)) = high;
    return v;
}

static inline CUInt128_t CUInt128Shl(CUInt128_t word, unsigned int offset) {
    uint64_t * l = _kCUInt128GetLow64Ptr(word);
    uint64_t * h = _kCUInt128GetHigh64Ptr(word);
    if (offset < 64) {
        uint64_t t = (*l) >> (64 - offset);
        *h = ((*h) << offset) | t;
        *l = (*l) << offset;
    } else if (offset < 128) {
        *h = (*l) << (offset - 64);
        *l = 0;
    }
    return word;
}

static inline CUInt128_t CUInt128Shr(CUInt128_t word, unsigned int offset) {
    uint64_t * l = _kCUInt128GetLow64Ptr(word);
    uint64_t * h = _kCUInt128GetHigh64Ptr(word);
    if (offset < 64) {
        uint64_t t = (*h) << (64 - offset);
        *h = (*h) >> offset;
        *l = ((*l) >> offset) | t;
    } else if (offset < 128) {
        *h = 0;
        *l = (*h) >> (offset - 64);
    }
    return word;
}

static inline CUInt128_t CUInt128RotateLeft(CUInt128_t word, unsigned int offset) {
    return CUInt128Or(CUInt128Shl(word, offset&127), CUInt128Shr(word, 128-(offset&127)));
}

static inline CUInt128_t CUInt128RotateRight(CUInt128_t word, unsigned int offset) {
    return CUInt128Or(CUInt128Shr(word, offset&127), CUInt128Shl(word, 128-(offset&127)));
}

static inline CUInt128_t CUInt128Add(CUInt128_t a, CUInt128_t b) {
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t bh = *_kCUInt128GetHigh64Ptr(b);
    uint64_t bl = *_kCUInt128GetLow64Ptr(b);
    uint64_t carry = 0;
    *l = *l + bl;
    if (*l < bl) {
        carry = 1;
    }
    *h = *h + bh + carry;
    return a;
}

//a-b
static inline CUInt128_t CUInt128Sub(CUInt128_t a, CUInt128_t b) {
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t bh = *_kCUInt128GetHigh64Ptr(b);
    uint64_t bl = *_kCUInt128GetLow64Ptr(b);

    uint64_t carry = 0;
    if (*l < bl) {
        carry = 1;
    }
    *l = *l - bl;
    *h = *h - bh - carry;
    return a;
}


static inline CUInt128_t CUInt128Opposite(CUInt128_t word) {
    return CUInt128Sub(CUInt128Zero, word);
}


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

static inline CUInt128_t CUInt128ByteSwap(CUInt128_t data) {
    CUInt128_t r = {};
    *((uint64_t *)(&(r.content[0]))) = CUInt64ByteSwap(*((uint64_t *)(&(data.content[8]))));
    *((uint64_t *)(&(r.content[8]))) = CUInt64ByteSwap(*((uint64_t *)(&(data.content[0]))));
    return r;
}


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static inline CUInt128_t CUInt128MakeWithBigEndianBytes(uint8_t const block[_Nonnull 16]) {
    CUInt128_t value = {};
    memcpy(&(value.content[0]), block, 16);
    return CUInt128ByteSwap(value);
}
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
static inline CUInt128_t CUInt128MakeWithBigEndianBytes(uint8_t const block[_Nonnull 16]) {
    CUInt128_t value = {};
    memcpy(&(value.content[0]), block, 16);
    return value;
}
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

static inline uint64_t CUInt64RotateLeft(uint64_t word, unsigned int offset) {
    __asm__("rolq %%cl,%0"
            :"=r" (word)
            :"0" (word),"c" (offset));
    return word;
}

static inline uint64_t CUInt64RotateRight(uint64_t word, unsigned int offset) {
    __asm__("rorq %%cl,%0"
            :"=r" (word)
            :"0" (word),"c" (offset));
    return word;
}

#else /* Not x86_64  */

// MARK: -- default C version

static inline uint64_t CUInt64RotateLeft(uint64_t word, unsigned int offset) {
    return ( (word<<(i&63)) | (word>>(64-(i&63))) );
}

static inline uint64_t CUInt64RotateRight(uint64_t word, unsigned int offset) {
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

static inline uint32_t CUInt32RotateLeft(uint32_t word, unsigned int offset) {
    __asm__ ("roll %%cl,%0"
             :"=r" (word)
             :"0" (word),"c" (offset));
    return word;
}

static inline uint32_t CUInt32RotateRight(uint32_t word, unsigned int offset) {
    __asm__ ("rorl %%cl,%0"
             :"=r" (word)
             :"0" (word),"c" (offset));
    return word;
}

#else

static inline uint32_t CUInt32RotateLeft(uint32_t word, unsigned int offset) {
    return ( (word<<(offset&31)) | (word>>(32-(offset&31))) );
}

static inline uint32_t CUInt32RotateRight(uint32_t word, unsigned int offset) {
    return ( (word>>(offset&31)) | (word<<(32-(offset&31))) );
}

#endif


#endif /* CInteger_h */
