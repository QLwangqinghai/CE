//
//  XType.h
//  X
//
//  Created by vector on 2020/4/3.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XType_h
#define XType_h


#if defined(__cplusplus)
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <wchar.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>

#if defined(__GNUC__)
#include <stdbool.h>
#endif
#include "XConfig.h"


typedef _Bool XBool;

typedef uint8_t XUInt8;
typedef uint16_t XUInt16;
typedef uint32_t XUInt32;
typedef uint64_t XUInt64;

typedef int8_t XSInt8;
typedef int16_t XSInt16;
typedef int32_t XSInt32;
typedef int64_t XSInt64;


typedef uint8_t XByte;


#if BUILD_TARGET_RT_64_BIT
    typedef int64_t XSInt;
    typedef uint64_t XUInt;
#elif BUILD_TARGET_RT_32_BIT
    typedef int32_t XSInt;
    typedef uint32_t XUInt;
#else
    #error unknown rt
#endif

typedef unsigned char XU8Char;
typedef uint16_t XU16Char;
typedef uint32_t XU32Char;

#pragma mark - pointer

typedef void * XPtr;

#pragma mark - CCHashCode

typedef XUInt XHashCode;

#pragma mark - XTimeInterval

typedef int64_t XTimeInterval;



#define XUInt16Max UINT16_MAX
#define XUInt32Max UINT32_MAX
#define XUInt64Max UINT64_MAX

#pragma mark - CCByte
//used for uuid、md5、ipAddress
typedef struct {
    uint8_t content[16];
} XByte16;

static inline XBool XByte16Equal(const XByte16 * _Nonnull lhs, const XByte16 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte16)) == 0;
}
static inline void XByte16ToBytes(const XByte16 * _Nonnull bytes, XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 16);
}
static inline void XByte16Reset(XByte16 * _Nonnull bytes, const XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 16);
}
static inline XByte16 CCByteMake(const XByte * _Nonnull buffer) {
    XByte16 result = {};
    memcpy(result.content, buffer, 16);
    return result;
}

typedef struct {
    uint8_t content[20];
} XByte20;
static inline XBool XByte20Equal(const XByte20 * _Nonnull lhs, const XByte20 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte20)) == 0;
}
static inline void XByte20ToBytes(const XByte20 * _Nonnull bytes, XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 20);
}
static inline void XByte20Reset(XByte20 * _Nonnull bytes, const XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 20);
}
static inline XByte20 XByte20Make(const uint8_t * _Nonnull buffer) {
    XByte20 result = {};
    memcpy(result.content, buffer, 20);
    return result;
}

typedef struct {
    uint8_t content[32];
} XByte32;
static inline XBool XByte32Equal(const XByte32 * _Nonnull lhs, const XByte32 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte32)) == 0;
}
static inline void XByte32ToBytes(const XByte32 * _Nonnull bytes, XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 32);
}
static inline void XByte32Reset(XByte32 * _Nonnull bytes, const XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 32);
}
static inline XByte32 XByte32Make(const uint8_t * _Nonnull buffer) {
    XByte32 result = {};
    memcpy(result.content, buffer, 32);
    return result;
}

typedef struct {
    uint8_t content[48];
} CCByte48;
static inline XBool CCByte48Equal(const CCByte48 * _Nonnull lhs, const CCByte48 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte48)) == 0;
}
static inline void CCByte48ToBytes(const CCByte48 * _Nonnull bytes, XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 48);
}
static inline void CCByte48Reset(CCByte48 * _Nonnull bytes, const XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 48);
}
static inline CCByte48 CCByte48Make(const XByte * _Nonnull buffer) {
    CCByte48 result = {};
    memcpy(result.content, buffer, 48);
    return result;
}

typedef struct {
    uint8_t content[64];
} XByte64;
static inline XBool XByte64Equal(const XByte64 * _Nonnull lhs, const XByte64 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte64)) == 0;
}
static inline void XByte64ToBytes(const XByte64 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 64);
}
static inline void XByte64Reset(XByte64 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 64);
}
static inline XByte64 XByte64Make(const uint8_t * _Nonnull buffer) {
    XByte64 result = {};
    memcpy(result.content, buffer, 64);
    return result;
}

typedef struct {
    uint8_t content[128];
} XByte128;
static inline XBool XByte128Equal(const XByte128 * _Nonnull lhs, const XByte128 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte128)) == 0;
}
static inline void XByte128ToBytes(const XByte128 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 128);
}
static inline void XByte128Reset(XByte128 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 128);
}
static inline XByte128 XByte128Make(const uint8_t * _Nonnull buffer) {
    XByte128 result = {};
    memcpy(result.content, buffer, 128);
    return result;
}

typedef enum {
    XTypeNone = 0,
    
    XTypeBool = 1,
    XTypeSInt8 = 2,
    XTypeUInt8 = 3,
    XTypeSInt16 = 4,
    XTypeUInt16 = 5,
    XTypeSInt32 = 6,
    XTypeUInt32 = 7,
    XTypeFloat = 8,
    XTypePtr = 9,
    XTypeSInt64 = 10,
    XTypeUInt64 = 11,
    XTypeDouble = 12,
    XTypeBuffer = 13,
} XType_e;

static inline _Bool XTypeIsValid(XType_e t) {
    return (t >= XTypeBool && t <= XTypeBuffer);
}


#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XType_h */
