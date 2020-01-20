//
//  CType.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CType_h
#define CType_h

#include <sys/types.h>
#include <stdint.h>
#include <assert.h>

#include <string.h>
#if defined(__GNUC__)
#include <stdbool.h>
#endif
#include "CCConfig.h"

#pragma mark - base

typedef uint64_t CCUInt64;
typedef uint32_t CCUInt32;
typedef uint16_t CCUInt16;
typedef uint8_t CCUInt8;
typedef int64_t CCSInt64;
typedef int32_t CCSInt32;
typedef int16_t CCSInt16;
typedef int8_t CCSInt8;
typedef _Bool CCBool;

typedef CCUInt8 CCChar;
typedef CCUInt16 CCU16Char;

#if CCBuild64Bit
typedef CCSInt64 CCInt;
typedef CCUInt64 CCUInt;
#else
typedef CCSInt32 CCInt;
typedef CCUInt32 CCUInt;
#endif


#pragma mark - CCHashCode

typedef CCUInt CCHashCode;


#pragma mark - CCMicrosecondTime

typedef CCSInt64 CCMicrosecondTime;


#pragma mark - CCIndex

#if CCBuild64Bit
    typedef uint64_t CCIndex;
#define CCConstantIndexNotFound UINT64_MAX
#else
    typedef uint32_t CCIndex;
#define CCConstantIndexNotFound UINT32_MAX
#endif

extern const CCIndex CCIndexNotFound;


#pragma mark - CCPoint
typedef struct {
    CCSInt32 x;
    CCSInt32 y;
} CCPoint32;
static inline CCPoint32 CCPoint32Make(CCSInt32 x, CCSInt32 y) {
    CCPoint32 point = {
        .x = x,
        .y = y,
    };
    return point;
}
static inline CCBool CCPoint32Equal(CCPoint32 lhs, CCPoint32 rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

typedef struct {
    CCSInt64 x;
    CCSInt64 y;
} CCPoint64;
static inline CCPoint64 CCPoint64Make(CCSInt64 x, CCSInt64 y) {
    CCPoint64 point = {
        .x = x,
        .y = y,
    };
    return point;
}
static inline CCBool CCPoint64Equal(CCPoint64 lhs, CCPoint64 rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

#pragma mark - CCSize
typedef struct {
    CCUInt32 width;
    CCUInt32 height;
} CCSize32;
static inline CCSize32 CCSize32Make(CCUInt32 width, CCUInt32 height) {
    CCSize32 size = {
        .width = width,
        .height = height,
    };
    return size;
}
static inline CCBool CCSize32Equal(CCSize32 lhs, CCSize32 rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

typedef struct {
    CCUInt64 width;
    CCUInt64 height;
} CCSize64;
static inline CCSize64 CCSize64Make(CCUInt64 width, CCUInt64 height) {
    CCSize64 size = {
        .width = width,
        .height = height,
    };
    return size;
}
static inline CCBool CCSize64Equal(CCSize64 lhs, CCSize64 rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

#pragma mark - CCRange

typedef struct {
    CCIndex location;
    CCIndex length;
} CCRange;

static inline CCRange CCRangeMake(CCIndex location, CCIndex length) {
    CCRange range = {
        .location = location,
        .length = length,
    };
    return range;
}


#pragma mark - CCByte

typedef struct {
    CCUInt8 content[16];
} CCByte16;

static inline CCBool CCByte16Equal(const CCByte16 * _Nonnull lhs, const CCByte16 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte16)) == 0;
}
static inline void CCByte16ToBytes(const CCByte16 * _Nonnull bytes, CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 16);
}
static inline void CCByte16Reset(CCByte16 * _Nonnull bytes, const CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 16);
}
static inline CCByte16 CCByteMake(const CCUInt8 * _Nonnull buffer) {
    CCByte16 result = {};
    memcpy(result.content, buffer, 16);
    return result;
}

typedef struct {
    CCUInt8 content[20];
} CCByte20;
static inline CCBool CCByte20Equal(const CCByte20 * _Nonnull lhs, const CCByte20 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte20)) == 0;
}
static inline void CCByte20ToBytes(const CCByte20 * _Nonnull bytes, CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 20);
}
static inline void CCByte20Reset(CCByte20 * _Nonnull bytes, const CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 20);
}
static inline CCByte20 CCByte20Make(const CCUInt8 * _Nonnull buffer) {
    CCByte20 result = {};
    memcpy(result.content, buffer, 20);
    return result;
}

typedef struct {
    CCUInt8 content[32];
} CCByte32;
static inline CCBool CCByte32Equal(const CCByte32 * _Nonnull lhs, const CCByte32 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte32)) == 0;
}
static inline void CCByte32ToBytes(const CCByte32 * _Nonnull bytes, CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 32);
}
static inline void CCByte32Reset(CCByte32 * _Nonnull bytes, const CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 32);
}
static inline CCByte32 CCByte32Make(const CCUInt8 * _Nonnull buffer) {
    CCByte32 result = {};
    memcpy(result.content, buffer, 32);
    return result;
}

typedef struct {
    CCUInt8 content[48];
} CCByte48;
static inline CCBool CCByte48Equal(const CCByte48 * _Nonnull lhs, const CCByte48 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte48)) == 0;
}
static inline void CCByte48ToBytes(const CCByte48 * _Nonnull bytes, CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 48);
}
static inline void CCByte48Reset(CCByte48 * _Nonnull bytes, const CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 48);
}
static inline CCByte48 CCByte48Make(const CCUInt8 * _Nonnull buffer) {
    CCByte48 result = {};
    memcpy(result.content, buffer, 48);
    return result;
}

typedef struct {
    CCUInt8 content[64];
} CCByte64;
static inline CCBool CCByte64Equal(const CCByte64 * _Nonnull lhs, const CCByte64 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte64)) == 0;
}
static inline void CCByte64ToBytes(const CCByte64 * _Nonnull bytes, CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 64);
}
static inline void CCByte64Reset(CCByte64 * _Nonnull bytes, const CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 64);
}
static inline CCByte64 CCByte64Make(const CCUInt8 * _Nonnull buffer) {
    CCByte64 result = {};
    memcpy(result.content, buffer, 64);
    return result;
}

typedef struct {
    CCUInt8 content[128];
} CCByte128;
static inline CCBool CCByte128Equal(const CCByte128 * _Nonnull lhs, const CCByte128 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte128)) == 0;
}
static inline void CCByte128ToBytes(const CCByte128 * _Nonnull bytes, CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 128);
}
static inline void CCByte128Reset(CCByte128 * _Nonnull bytes, const CCUInt8 * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 128);
}
static inline CCByte128 CCByte128Make(const CCUInt8 * _Nonnull buffer) {
    CCByte128 result = {};
    memcpy(result.content, buffer, 128);
    return result;
}

typedef struct {
    CCSInt64 seconds;
    CCSInt32 nanosecond;
    CCSInt32 _xx;//must be zero
} CCTime;

typedef void * CCPtr;



typedef enum {
    CTypeNone = 0,
    
    CTypeBool = 1,
    CTypeSInt8 = 2,
    CTypeUInt8 = 3,
    CTypeSInt16 = 4,
    CTypeUInt16 = 5,
    CTypeSInt32 = 6,
    CTypeUInt32 = 7,
    CTypeFloat = 8,
    CTypePtr = 9,
    CTypeSInt64 = 10,
    CTypeUInt64 = 11,
    CTypeDouble = 12,
    CTypeBuffer = 13,
} CType_e;

static inline _Bool CTypeIsValid(CType_e t) {
    return (t >= CTypeBool && t <= CTypeBuffer);
}

#endif /* CEParamType_h */
