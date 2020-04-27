//
//  XRuntimeInternal.h
//  
//
//  Created by vector on 2020/4/4.
//

#ifndef XRuntimeInternal_h
#define XRuntimeInternal_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XRuntime.h"
#include "XAllocator.h"
#include "XObjectInternal.h"

typedef XPtr _XDescriptionBuffer;

typedef void (*XObjectDeinit_f)(XObject _Nonnull obj);
typedef void (*XObjectDescribe_f)(XObject _Nonnull obj, _XDescriptionBuffer _Nullable buffer);
typedef XObject _Nonnull (*XObjectCopy_f)(XObject _Nonnull obj);


#if BUILD_TARGET_RT_64_BIT
    #pragma pack(push, 8)
#elif BUILD_TARGET_RT_32_BIT
    #pragma pack(push, 4)
#else
    #error unknown rt
#endif


/*
 
 XObjectFlagClearOnDealloc
 XObjectFlagNoRc
 */


//64bit 压缩的头
 /* flagBegin:2 type:6 rcInfo:55 flagEnd:1  */

//32bit rc info
/*
 count://d高位
 
 flags: 低4位
 flag0:clear on dealloc
 flag1:hasWeak
 
 */

#if BUILD_TARGET_RT_64_BIT

typedef struct {
    _Atomic(uintptr_t) typeInfo;
} _XObjectCompressedBase;

typedef struct {
    _Atomic(uintptr_t) typeInfo;
    _Atomic(XFastUInt) rcInfo;
} _XObjectBase;

#define X_BUILD_TypeInfoCompressedMask (uintptr_t)(0xC000000000000001ULL)

#define X_BUILD_TypeInfoConstantCompressed (uintptr_t)(0xE00FFFFFFFFFFFFFULL)


#define _XConstantObjectCompressedBaseMake(type) {\
    .typeInfo = ATOMIC_VAR_INIT((uintptr_t)(X_BUILD_TypeInfoConstantCompressed | (((type) & 0x3f) << 55))),\
}

#elif BUILD_TARGET_RT_32_BIT

typedef struct {
    _Atomic(uintptr_t) typeInfo;
    _Atomic(XFastUInt) rcInfo;
} _XObjectCompressedBase;

typedef _XObjectCompressedBase _XObjectBase;

#define _XConstantObjectCompressedBaseMake(type) {\
    .typeInfo = ATOMIC_VAR_INIT((uintptr_t)(type)),\
    .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectFlagReadOnly)),\
}

#else
    #error unknown rt
#endif

#define _XConstantObjectBaseMake(type) {\
    .typeInfo = ATOMIC_VAR_INIT((uintptr_t)(type)),\
    .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectFlagReadOnly)),\
}



struct _XTypeIdentifier;
typedef struct _XTypeIdentifier _XTypeIdentifier_s;
typedef const struct _XTypeIdentifier * _XTypeIdentifierPtr;

struct _XTypeIdentifier {
    _XTypeIdentifierPtr _Nullable super;
    const char * _Nonnull name;
    XRefHashCode_f _Nullable hashCode;
    XRefEqualTo_f _Nonnull equalTo;
};



struct _XTypeBase;
typedef struct _XTypeBase XTypeBase_s;

struct _XTypeBase {
    _XTypeIdentifierPtr _Nonnull identifier;

#if BUILD_TARGET_RT_64_BIT
    XUInt64 kind: 4;
    XUInt64 xx: 4;
    XUInt64 domain: 8;
    XUInt64 tableSize: 48;
#elif BUILD_TARGET_RT_32_BIT
    XUInt32 kind: 4;
    XUInt32 xx: 4;
    XUInt32 domain: 8;
    XUInt32 tableSize: 16;
#else
    #error unknown rt
#endif
    uintptr_t /* _XType_s * _Nullable */ super;
    _XAllocatorPtr _Nonnull allocator;

    //对象方法
    XObjectDeinit_f _Nullable deinit;
    XObjectDescribe_f _Nonnull describe;
};


typedef struct {
    _XObjectBase _runtime;
    XTypeBase_s base;
    
    //动态方法列表
    uintptr_t table[0];
} _XType_s;


#if BUILD_TARGET_RT_64_BIT

#define X_BUILD_ObjectRcMax (0xFFFFFFFFFFFFFFF0ULL)
//#define X_BUILD_ObjectCompressedRcMax (0xFFFFFFFFFFFFFFF0ULL)

#elif BUILD_TARGET_RT_32_BIT

#define X_BUILD_ObjectRcMax (0xFFFFFFF0UL)
//#define X_BUILD_ObjectCompressedRcMax (0xFFFFFFF0UL)
#else
    #error unknown rt
#endif


#pragma mark - XClass

extern const XClassIdentifier _Nonnull XMateClassIdentifier;
extern const _XType_s _XClassTable[];
extern const _XType_s _XClassNull;
extern const _XType_s _XClassBoolean;

static inline XClass _Nullable _XRefGetCompressedType(XUInt id) {
    if (id <= 0 || id > 9) {
        return NULL;
    } else {
        return (XClass)&(_XClassTable[id - 1]);
    }
}

#pragma mark - XNull

typedef struct {
    XUInt __;
} _XNullContent_t;

typedef struct {
    _XObjectBase _runtime;
    _XNullContent_t content;
} _XNull;

#pragma mark - XBoolean

typedef struct {
    XBool value;
} _XBooleanContent_t;

typedef struct {
    _XObjectBase _runtime;
    _XBooleanContent_t content;
} _XBoolean;

#pragma mark - XNumber

typedef union {
    XSInt32 s;
    XUInt32 u;
    XFloat32 f;
} _XNumberBits32_u;

typedef union {
    XSInt64 s;
    XUInt64 u;
    XFloat64 f;
} _XNumberBits64_u;

typedef struct {
    XUInt32 type;
    _XNumberBits32_u bits32;
    XUInt8 extended[0];/* 可能有0、8、16 3种可能 */
} _XNumberContent_t;


typedef struct {
    _XObjectCompressedBase _runtime;
    _XNumberContent_t content;
} _XNumber;

#pragma mark - XString
/*
 
 */

//inline buffer
typedef XUInt32 _XExtendedLayout;


/*
32: (8 + 4 + 4 + opt12 = 16 or 28)
16 48 (128-16)
64: (8 + 4 + 4 + opt16 = 16 or 32)
16 48 (128-16) (256-16)
*/
typedef struct {
    _XExtendedLayout layout: 4;
    XUInt32 hasHashCode: 1;
    XUInt32 clearOnDealloc: 1;
    XUInt32 __unuse: 10;
    XUInt32 inlineLength: 16;//layout == _XStringLayoutInline 时有效
    XUInt32 hashCode;
    XUInt8 extended[0];/* 可能有0、16、48、112、 240 5种可能 */
} _XByteStorageContent_t;

typedef struct {
    XUInt offset: 32;
    XUInt length: 32;
    _XBuffer * _Nonnull buffer;
} _XByteBufferContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XByteStorageContent_t content;
} _XByteStorage;



//typedef void (*XCustomRelease_f)(XPtr _Nullable context, XPtr _Nonnull content, XUInt length);
//typedef struct {
//    XPtr _Nullable context;
//    XUInt length;
//    XCustomRelease_f _Nullable customRelease;
//    XPtr _Nonnull content;
//} _XByteCustomContent_t;


typedef struct _XByteStorageContent_t _XStringContent_t;
typedef struct _XByteBufferContent_t _XStringBufferContent_t;


typedef _XByteStorage _XString;


typedef struct {
    _XString _base;
    XUInt8 extended[sizeof(XUInt)];
} _XInlineEmptyString;

#pragma mark - XData

typedef _XByteStorage _XData;

#pragma mark - XDate

typedef struct {
    XTimeInterval time;
} _XDateContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XDateContent_t content;
} _XDate;

#pragma mark - XValue

/*
32: (8 + 4 + 4 = 16)
16 48
64: (8 + 4 + 4 = 16)
16 48
*/

typedef struct {
    XUInt32 clearOnDealloc: 1;
    XUInt32 hasHashCode: 1;
    XUInt32 extendedSize: 30;
    XUInt32 hashCode;
    XUInt8 extended[0];
} _XValueContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XValueContent_t content;
} _XValue;

typedef void (*XStorageClear_f)(XUInt8 * _Nullable content, XSize size);

typedef struct {
    XSize extendedSize;
    XStorageClear_f _Nullable clear;
    XUInt8 extended[0];
} _XStorageContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XStorageContent_t content;
} _XStorage;

#if BUILD_TARGET_RT_64_BIT
#elif BUILD_TARGET_RT_32_BIT
#else
    #error unknown rt
#endif


#pragma pack(pop)

extern XCompressedType _XRefInlineTypeToCompressedType(XUInt id);


extern uintptr_t _XRefGetTypeInfo(XRef _Nonnull ref);
extern XClass _Nullable _XRefGetCompressedType(XUInt id);

extern XHashCode _XHashUInt64(XUInt64 i);
extern XHashCode _XHashFloat64(XFloat64 d);

#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XRuntime_h */
