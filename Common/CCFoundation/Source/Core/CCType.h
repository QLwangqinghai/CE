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

#define CCUIntFast64 uint_fast64_t
#define CCUIntFast32 uint_fast32_t
#define CCUIntFast64Max UINT_FAST64_MAX
#define CCUIntFast32Max UINT_FAST32_MAX


#if CCBuild64Bit
typedef CCSInt64 CCInt;
typedef CCUInt64 CCUInt;
#define CCUIntFast CCUIntFast64
#define CCUIntFastMax CCUIntFast64Max
#else
typedef CCSInt32 CCInt;
typedef CCUInt32 CCUInt;
#define CCUIntFast CCUIntFast32
#define CCUIntFastMax CCUIntFast32Max
#endif

#if CCBuild64Bit
#define CCUInt_MAX UINT64_MAX
#define CCSInt_MAX INT64_MAX
#else
#define CCUInt_MAX UINT32_MAX
#define CCSInt_MAX INT32_MAX
#endif

#pragma mark - pointer

typedef void * CCPtr;

#pragma mark - CCHashCode

typedef CCUInt CCHashCode;


#pragma mark - CCMicrosecondTime

typedef CCSInt64 CCMicrosecondTime;


#pragma mark - CCIndex

#define CCConstantIndex16NotFound UINT16_MAX
#define CCConstantIndex32NotFound UINT32_MAX
#define CCConstantIndex64NotFound UINT64_MAX


typedef CCUInt CCIndex;
#define CCConstantIndexNotFound CCUInt_MAX

typedef CCUInt16 CCIndex16;
typedef CCUInt32 CCIndex32;
typedef CCUInt64 CCIndex64;

extern const CCIndex CCIndexNotFound;
extern const CCIndex16 CCIndex16NotFound;
extern const CCIndex32 CCIndex32NotFound;
extern const CCIndex64 CCIndex64NotFound;

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


#pragma mark - CCRect
typedef struct {
    CCPoint32 origin;
    CCSize32 size;
} CCRect32;
static inline CCRect32 CCRect32Make(CCSInt32 x, CCSInt32 y, CCUInt32 width, CCUInt32 height) {
    CCRect32 rect = {
        .origin = CCPoint32Make(x, y),
        .size = CCSize32Make(width, height),
    };
    return rect;
}
static inline CCBool CCRect32Equal(CCRect32 lhs, CCRect32 rhs) {
    return CCPoint32Equal(lhs.origin, rhs.origin) && CCSize32Equal(lhs.size, rhs.size);
}
static inline CCBool CCRect32IsValid(CCRect32 rect) {
    return 0;
}



typedef struct {
    CCPoint64 origin;
    CCSize64 size;
} CCRect64;
static inline CCRect64 CCRect64Make(CCSInt64 x, CCSInt64 y, CCUInt64 width, CCUInt64 height) {
    CCRect64 rect = {
        .origin = CCPoint64Make(x, y),
        .size = CCSize64Make(width, height),
    };
    return rect;
}
static inline CCBool CCRect64Equal(CCRect64 lhs, CCRect64 rhs) {
    return CCPoint64Equal(lhs.origin, rhs.origin) && CCSize64Equal(lhs.size, rhs.size);
}
static inline CCBool CCRect64IsValid(CCRect32 rect) {
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


#pragma mark - net

typedef CCByte16 CCIpAddress;

#pragma mark - other

typedef struct {
    CCSInt64 seconds;
    CCSInt32 nanosecond;
    CCSInt32 _xx;//must be zero
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
