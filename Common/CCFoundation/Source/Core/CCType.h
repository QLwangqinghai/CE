//
//  CCType.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CCType_h
#define CCType_h

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
#include "CCConfig.h"

#pragma mark - log

#define CCLogError(format, ...) do {\
fprintf(stderr, format, ##__VA_ARGS__);                                               \
fflush(stderr);\
} while(0)


#pragma mark - base

typedef _Bool CBool;

#if BUILD_TARGET_RT_64_BIT

    typedef int64_t SInteger;
    typedef uint64_t UInteger;

    #define SIntegerFast int_fast64_t
    #define SIntegerFastMax INT_FAST64_MAX
    #define SIntegerMax INT64_MAX
    #define SIntegerFastMin INT_FAST64_MIN
    #define SIntegerMin INT64_MIN

    #define UIntegerFast uint_fast64_t
    #define UIntegerFastMax UINT_FAST64_MAX
    #define UIntegerMax UINT64_MAX

#elif BUILD_TARGET_RT_32_BIT

    typedef int32_t SInteger;
    typedef uint32_t UInteger;

    #define SIntegerFast int_fast32_t
    #define SIntegerFastMax INT_FAST32_MAX
    #define SIntegerMax INT32_MAX
    #define SIntegerFastMin INT_FAST32_MIN
    #define SIntegerMin INT32_MIN

    #define UIntegerFast uint_fast32_t
    #define UIntegerFastMax UINT_FAST32_MAX
    #define UIntegerMax UINT32_MAX

#else
    #error unknown rt
#endif


typedef unsigned char U8Char;
typedef unsigned short U16Char;

typedef _Bool CBool;

typedef uint8_t CCChar;
typedef uint16_t CCU16Char;

#pragma mark - pointer

typedef void * CCPtr;

#pragma mark - CCHashCode

typedef UInteger CCHashCode;

#pragma mark - CCMicrosecondTime

typedef int64_t CCMicrosecondTime;
typedef uint64_t CCBootMicrosecondTime;


#pragma mark - CCIndex

#define CCConstantIndex16NotFound UINT16_MAX
#define CCConstantIndex32NotFound UINT32_MAX
#define CCConstantIndex64NotFound UINT64_MAX


typedef UInteger CCIndex;
#define CCConstantIndexNotFound UIntegerMax

typedef uint16_t CCIndex16;
typedef uint32_t CCIndex32;
typedef uint64_t CCIndex64;

extern const CCIndex CCIndexNotFound;
extern const CCIndex16 CCIndex16NotFound;
extern const CCIndex32 CCIndex32NotFound;
extern const CCIndex64 CCIndex64NotFound;

#pragma mark - CCPoint
typedef struct {
    int32_t x;
    int32_t y;
} CCPoint32;
static inline CCPoint32 CCPoint32Make(int32_t x, int32_t y) {
    CCPoint32 point = {
        .x = x,
        .y = y,
    };
    return point;
}
static inline CBool CCPoint32Equal(CCPoint32 lhs, CCPoint32 rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

typedef struct {
    int64_t x;
    int64_t y;
} CCPoint64;
static inline CCPoint64 CCPoint64Make(int64_t x, int64_t y) {
    CCPoint64 point = {
        .x = x,
        .y = y,
    };
    return point;
}
static inline CBool CCPoint64Equal(CCPoint64 lhs, CCPoint64 rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

#pragma mark - CCSize
typedef struct {
    int32_t width;
    int32_t height;
} CCSize32;
static inline CCSize32 CCSize32Make(int32_t width, int32_t height) {
    CCSize32 size = {
        .width = width,
        .height = height,
    };
    return size;
}
static inline CBool CCSize32Equal(CCSize32 lhs, CCSize32 rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

typedef struct {
    int64_t width;
    int64_t height;
} CCSize64;
static inline CCSize64 CCSize64Make(int64_t width, int64_t height) {
    CCSize64 size = {
        .width = width,
        .height = height,
    };
    return size;
}
static inline CBool CCSize64Equal(CCSize64 lhs, CCSize64 rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}


#pragma mark - CCRect
typedef struct {
    CCPoint32 origin;
    CCSize32 size;
} CCRect32;
static inline CCRect32 CCRect32Make(int32_t x, int32_t y, int32_t width, int32_t height) {
    CCRect32 rect = {
        .origin = CCPoint32Make(x, y),
        .size = CCSize32Make(width, height),
    };
    return rect;
}
static inline CBool CCRect32Equal(CCRect32 lhs, CCRect32 rhs) {
    return CCPoint32Equal(lhs.origin, rhs.origin) && CCSize32Equal(lhs.size, rhs.size);
}
static inline CBool CCRect32IsValid(CCRect32 rect) {
    return 0;
}



typedef struct {
    CCPoint64 origin;
    CCSize64 size;
} CCRect64;
static inline CCRect64 CCRect64Make(int64_t x, int64_t y, int64_t width, int64_t height) {
    CCRect64 rect = {
        .origin = CCPoint64Make(x, y),
        .size = CCSize64Make(width, height),
    };
    return rect;
}
static inline CBool CCRect64Equal(CCRect64 lhs, CCRect64 rhs) {
    return CCPoint64Equal(lhs.origin, rhs.origin) && CCSize64Equal(lhs.size, rhs.size);
}
static inline CBool CCRect64IsValid(CCRect32 rect) {
    return 0;
}

////并集
//static inline _Bool CCRect32Union(CCRect32 rect, CCRect32 rect1, CCRect32 * _Nonnull result) {
//    CCRect32 tmp = {};
//    tmp.origin.x = MIN(rect.origin.x, rect1.origin.x);
//    tmp.origin.y = MIN(rect.origin.y, rect1.origin.y);
//    tmp.size.width = MAX(rect.origin.x + rect.size.width, rect1.origin.x + rect1.size.width) - tmp.origin.x;
//    tmp.size.height = MAX(rect.origin.y + rect.size.height, rect1.origin.y + rect1.size.height) - tmp.origin.y;
//    if (tmp.size.width > 0 && tmp.size.height > 0) {
//        *result = tmp;
//        return true;
//    }
//    return false;
//}
//
////交集
//static inline _Bool C2DRectIntersect(C2DRect rect, C2DRect rect1, C2DRect * _Nonnull result) {
//    rect = C2DRectStandardize(rect);
//    rect1 = C2DRectStandardize(rect1);
//    C2DRect tmp = {};
//    tmp.origin.x = MAX(rect.origin.x, rect1.origin.x);
//    tmp.origin.y = MAX(rect.origin.y, rect1.origin.y);
//    tmp.size.width = MIN(rect.origin.x + rect.size.width, rect1.origin.x + rect1.size.width) - tmp.origin.x;
//    tmp.size.height = MIN(rect.origin.y + rect.size.height, rect1.origin.y + rect1.size.height) - tmp.origin.y;
//    if (tmp.size.width > 0 && tmp.size.height > 0) {
//        *result = tmp;
//        return true;
//    }
//    return false;
//}






#pragma mark - CCRange

typedef struct {
    CCIndex16 location;
    CCIndex16 length;
} CCRange16;
static inline CCRange16 CCRange16Make(CCIndex16 location, CCIndex16 length) {
    CCRange16 range = {
        .location = location,
        .length = length,
    };
    return range;
}

typedef struct {
    CCIndex32 location;
    CCIndex32 length;
} CCRange32;
static inline CCRange32 CCRange32Make(CCIndex32 location, CCIndex32 length) {
    CCRange32 range = {
        .location = location,
        .length = length,
    };
    return range;
}

typedef struct {
    CCIndex64 location;
    CCIndex64 length;
} CCRange64;
static inline CCRange64 CCRange64Make(CCIndex64 location, CCIndex64 length) {
    CCRange64 range = {
        .location = location,
        .length = length,
    };
    return range;
}

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
//used for uuid、md5、ipAddress
typedef struct {
    uint8_t content[16];
} CCByte16;

static inline CBool CCByte16Equal(const CCByte16 * _Nonnull lhs, const CCByte16 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte16)) == 0;
}
static inline void CCByte16ToBytes(const CCByte16 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 16);
}
static inline void CCByte16Reset(CCByte16 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 16);
}
static inline CCByte16 CCByteMake(const uint8_t * _Nonnull buffer) {
    CCByte16 result = {};
    memcpy(result.content, buffer, 16);
    return result;
}

typedef struct {
    uint8_t content[20];
} CCByte20;
static inline CBool CCByte20Equal(const CCByte20 * _Nonnull lhs, const CCByte20 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte20)) == 0;
}
static inline void CCByte20ToBytes(const CCByte20 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 20);
}
static inline void CCByte20Reset(CCByte20 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 20);
}
static inline CCByte20 CCByte20Make(const uint8_t * _Nonnull buffer) {
    CCByte20 result = {};
    memcpy(result.content, buffer, 20);
    return result;
}

typedef struct {
    uint8_t content[32];
} CCByte32;
static inline CBool CCByte32Equal(const CCByte32 * _Nonnull lhs, const CCByte32 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte32)) == 0;
}
static inline void CCByte32ToBytes(const CCByte32 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 32);
}
static inline void CCByte32Reset(CCByte32 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 32);
}
static inline CCByte32 CCByte32Make(const uint8_t * _Nonnull buffer) {
    CCByte32 result = {};
    memcpy(result.content, buffer, 32);
    return result;
}

typedef struct {
    uint8_t content[48];
} CCByte48;
static inline CBool CCByte48Equal(const CCByte48 * _Nonnull lhs, const CCByte48 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte48)) == 0;
}
static inline void CCByte48ToBytes(const CCByte48 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 48);
}
static inline void CCByte48Reset(CCByte48 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 48);
}
static inline CCByte48 CCByte48Make(const uint8_t * _Nonnull buffer) {
    CCByte48 result = {};
    memcpy(result.content, buffer, 48);
    return result;
}

typedef struct {
    uint8_t content[64];
} CCByte64;
static inline CBool CCByte64Equal(const CCByte64 * _Nonnull lhs, const CCByte64 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte64)) == 0;
}
static inline void CCByte64ToBytes(const CCByte64 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 64);
}
static inline void CCByte64Reset(CCByte64 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 64);
}
static inline CCByte64 CCByte64Make(const uint8_t * _Nonnull buffer) {
    CCByte64 result = {};
    memcpy(result.content, buffer, 64);
    return result;
}

typedef struct {
    uint8_t content[128];
} CCByte128;
static inline CBool CCByte128Equal(const CCByte128 * _Nonnull lhs, const CCByte128 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(CCByte128)) == 0;
}
static inline void CCByte128ToBytes(const CCByte128 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 128);
}
static inline void CCByte128Reset(CCByte128 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 128);
}
static inline CCByte128 CCByte128Make(const uint8_t * _Nonnull buffer) {
    CCByte128 result = {};
    memcpy(result.content, buffer, 128);
    return result;
}


#pragma mark - net

typedef CCByte16 CCIpAddress;

#pragma mark - other

typedef struct {
    int64_t seconds;
    int32_t nanosecond;
    int32_t _xx;//must be zero
} CCTime;


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


#if defined(__cplusplus)
}  // extern C
#endif


#endif /* CCType_h */
