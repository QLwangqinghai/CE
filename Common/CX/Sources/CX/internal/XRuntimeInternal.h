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
 /* flagBegin:3 type:6 (rcInfo54(flag:4 count:18+32)) flagEnd:1  */

//32bit rc info
/* flag:4 count:18+32 */

#if BUILD_TARGET_RT_64_BIT

typedef struct {
    _Atomic(uintptr_t) typeInfo;
} _XObjectCompressedBase;

typedef struct {
    _Atomic(uintptr_t) typeInfo;
    _Atomic(XFastUInt) rcInfo;
} _XObjectBase;

#define X_BUILD_TypeInfoCompressedMask (uintptr_t)(0xE000000000000001ULL)


/*
  flagBegin:3 type:6 (rcInfo54(flag:4 count:18+32)) flagEnd:1
 
 
 */

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

typedef XBool (*XRefEqualTo_f)(XRef _Nonnull lhs, XRef _Nonnull rhs);



struct _XTypeBase;
typedef struct _XTypeBase XTypeBase_s;

struct _XTypeBase {
    _XTypeIdentifierPtr _Nonnull identifier;

#if BUILD_TARGET_RT_64_BIT
    XUInt64 kind: 4;
    //是否是压缩的
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
    _XAllocatorPtr _Nullable allocator;

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


/*
 typedef XObject XNumber;
 typedef XObject XString;
 typedef XObject XArray;
 typedef XObject XMap;
 
 typedef XObject XData;
 typedef XObject XDate;
 typedef XObject XValue;
 typedef XObject XSet;
 
 _XObjectBase base;
 _XObjectCompressedBase base;
 */
typedef struct {
    XUInt __;
} _XNullContent_t;

typedef struct {
    _XObjectBase _runtime;
    _XNullContent_t content;
} _XNull;


typedef struct {
    XBool value;
} _XBooleanContent_t;

typedef struct {
    _XObjectBase _runtime;
    _XBooleanContent_t content;
} _XBoolean;



typedef struct {
    XUInt64 type: 8;
    XUInt64 sign: 1;

    XSize size;
    XUInt8 content[0];
} _XNumberContent11_t;

typedef struct {
    XUInt64 type: 8;
    XUInt64 sign: 1;

    XSize size;
    XUInt8 content[0];
} _XNumberContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XNumberContent_t content;
} _XNumber;


typedef struct {
    XSize size;
    XUInt8 content[0];
} _XValueContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XValueContent_t content;
} _XValue;


typedef void (*XStorageClear_f)(XUInt8 * _Nullable content, XSize size);

typedef struct {
    XSize contentSize;
    XStorageClear_f _Nullable clear;
    XUInt8 content[0];
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

typedef struct {
    XUInt32 offset;
    XUInt32 length;
    _XBuffer * _Nonnull buffer;
} _XStringBufferContent_t;

typedef struct {
    XUInt layout: 4;
    XUInt hasHashCode: 1;
    XUInt __unuse: 3;
    XUInt mask: 8;
    XUInt inlineLength: 16;
    XUInt hashCode: 32;
    XUInt8 extended[0];
} _XStringContent_t;


typedef XUInt _XStringLayout;
typedef XUInt _XStringMask;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XStringContent_t content;
} _XString;


typedef struct {
    XUInt layout: 4;
    XUInt hasHashCode: 1;
    XUInt __unuse: 3;
    XUInt mask: 8;
    XUInt inlineLength: 16;
    XUInt hashCode: 32;
    XUInt8 extended[sizeof(XUInt)];
} _XStringInlineEmptyContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XStringInlineEmptyContent_t content;
} _XInlineEmptyString;


/*
32:
base: 4
length: 4
hashCode: 4
content: 4, 20, 52
 
64:
base: 8
length: 4
hashCode: 4
content: 16, 48, 112

 32: =24
 base: 4
 length: 4
 hashCode: 4
 xxx: 4
 offset: 4
 buffer: 4
 
 64: = 32
 base: 8
 length: 4
 hashCode: 4
 xxx: 4
 offset: 4
 buffer: 8
 */

typedef struct {
    XUInt length: 32;
    XUInt hashCode: 32;
    XUInt8 extended[0];
} _XDataContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XDataContent_t content;
} _XData;


typedef struct {
    XTimeInterval time;
} _XDateContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XDateContent_t content;
} _XDate;

#pragma pack(pop)



extern uintptr_t _XRefGetTypeInfo(XRef _Nonnull ref);
extern XClass _Nullable _XRefGetCompressedType(XUInt id);


#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XRuntime_h */
