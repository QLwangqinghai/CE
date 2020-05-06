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
#include "XAtomic.h"
#include "XRef.h"

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
 Tagged32
 refType: 1
 taggedContent: 30
 flag: 1, value = 1
 */

/*
Tagged64
refType: 2
taggedContent: 61
flag: 1, value = 1
*/


/*
 TaggedObject32

 refType: 1, value = 1
 taggedContent: {
    class: 2
    objectContent: 28
 }
 flag: 1, value = 1
*/

/*
 TaggedObject64

 refType: 2, value = 1
 taggedContent: 61 {
    class: 2
    objectContent: 59
 }
 flag: 1, value = 1
*/


/*
 TaggedIsa64
 
 refType: 2, value = 2
 taggedContent: 61 {
    isa: 6
    counter: 51
    counterFlag: 4
 }
 flag: 1, value = 1
 */


/*
 
 XObjectFlagclearWhenDealloc
 XObjectFlagNoRc
 */


//64bit 压缩的ObjectHeader flagBegin = 2
 /* flagBegin:2 type:6 rcInfo:55 flagEnd:1  */

//64bit 压缩的StackObject flagBegin = 1
/* flagBegin:2 type:6 rcInfo:55 flagEnd:1  */

//32bit StackObject flagBegin = 1
/* flagBegin:1 type:6 rcInfo:55 flagEnd:1  */


// rc info
/*
 count://其余高位
 flags: 低4位
 flag1:hasWeak
 */

#if BUILD_TARGET_RT_64_BIT

#define X_BUILD_ObjectRcMax 0xFFFFFFFFFFFFFFF0ULL

#define X_BUILD_TaggedMask 0xC000000000000001ULL
#define X_BUILD_TaggedObjectFlag 0x8000000000000001ULL

#define X_BUILD_TaggedObjectClassMask    0x3000000000000000ULL
#define X_BUILD_TaggedObjectClassShift    60ULL
#define X_BUILD_TaggedObjectClassNumber   0x0ULL
#define X_BUILD_TaggedObjectClassString  0x1000000000000000ULL
#define X_BUILD_TaggedObjectClassData    0x2000000000000000ULL
#define X_BUILD_TaggedObjectClassDate    0x3000000000000000ULL

#define X_BUILD_TaggedObjectHeaderFlag  0x4000000000000001ULL
#define X_BUILD_TaggedObjectHeaderClassMask    0x3F00000000000000ULL
#define X_BUILD_TaggedObjectHeaderClassShift   56ULL


#elif BUILD_TARGET_RT_32_BIT

#define X_BUILD_ObjectRcMax 0xFFFFFFF0UL

#define X_BUILD_TaggedMask 0x80000001UL
#define X_BUILD_TaggedObjectFlag 0x80000001UL

#define X_BUILD_TaggedObjectClassMask    0x60000000UL
#define X_BUILD_TaggedObjectClassShift   29UL
#define X_BUILD_TaggedObjectClassNumber  0X0UL
#define X_BUILD_TaggedObjectClassString  0x20000000UL
#define X_BUILD_TaggedObjectClassData    0x40000000UL
#define X_BUILD_TaggedObjectClassDate    0x60000000UL

#else
    #error unknown rt
#endif

#define X_BUILD_TaggedObjectClassMax    X_BUILD_TaggedObjectClassDate

#if BUILD_TARGET_RT_64_BIT

typedef struct {
    _Atomic(uintptr_t) typeInfo;
} _XObjectCompressedBase;

typedef struct {
    _Atomic(uintptr_t) typeInfo;
    _Atomic(XFastUInt) rcInfo;
} _XObjectBase;

//
#define X_BUILD_TypeInfoConstantCompressed (uintptr_t)(0xC0FFFFFFFFFFFFE1ULL)

/* flagBegin:2 type:6 rcInfo:55 flagEnd:1  */
#define _XConstantObjectCompressedBaseMake(type) {\
    .typeInfo = ATOMIC_VAR_INIT((uintptr_t)(X_BUILD_TypeInfoConstantCompressed | (((type) & 0x3f) << 56))),\
}

#elif BUILD_TARGET_RT_32_BIT

typedef struct {
    _Atomic(uintptr_t) typeInfo;
    _Atomic(XFastUInt) rcInfo;
} _XObjectCompressedBase;

typedef _XObjectCompressedBase _XObjectBase;

#define _XConstantObjectCompressedBaseMake(type) {\
    .typeInfo = ATOMIC_VAR_INIT((uintptr_t)(type)),\
    .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectRcFlagReadOnly)),\
}

#else
    #error unknown rt
#endif

#define _XConstantObjectBaseMake(type) {\
    .typeInfo = ATOMIC_VAR_INIT((uintptr_t)(type)),\
    .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectRcFlagReadOnly)),\
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

extern const _XType_s * _Nonnull _XClassType;
extern const _XType_s * _Nonnull _XClassNull;
extern const _XType_s * _Nonnull _XClassBoolean;
extern const _XType_s * _Nonnull _XClassNumber;
extern const _XType_s * _Nonnull _XClassString;
extern const _XType_s * _Nonnull _XClassData;
extern const _XType_s * _Nonnull _XClassDate;
extern const _XType_s * _Nonnull _XClassValue;
extern const _XType_s * _Nonnull _XClassStorage;
extern const _XType_s * _Nonnull _XClassArray;
extern const _XType_s * _Nonnull _XClassMap;
extern const _XType_s * _Nonnull _XClassSet;

#pragma mark - XClass

extern const XClassIdentifier _Nonnull XMateClassIdentifier;
extern const _XType_s _XClassTable[];
extern const _XType_s _XClassNullStorage;
extern const _XType_s _XClassBooleanStorage;

static inline XClass _Nullable _XRefGetClassWithCompressedType(XCompressedType id) {
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

typedef struct {
    _Atomic(XFastUInt) _refCount;
    XUInt size;
    XUInt8 content[0];
} _XBuffer;

/*
(8 + 4 + 4 + opt16 = 16 or 32)
16 32 48 64 80 96 112(128-16) (256-16)
*/
//typedef struct {
//    _XExtendedLayout layout: 4;
//    XUInt32 hasHashCode: 1;
//    XUInt32 __unuse: 3;
//    XUInt32 clearWhenDealloc: 1;
//    XUInt32 __unusedFlags: 7;
//    XUInt32 inlineLength: 16;//layout == _XStringLayoutInline时有效 取值范围(0、16、48、112、 240), 其他情况必须设置为UInt16Max
//    XUInt32 hashCode;
//    XUInt8 extended[0];/* 可能有0、16、48、112、 240 5种可能 */
//} _XByteStorageContent_t;


#pragma pack(push, 4)

typedef struct {
    XUInt32 length;
    XUInt32 __xx;
} _XByteStorageContent_t;
typedef struct {
    XUInt32 length;
    XUInt8 extended[4];/* 可能有0、16、48、112、 240 5种可能 */
} _XByteStorageContentSmall_t;
typedef struct {
    XUInt32 length;
    XUInt32 offset;
    XUInt32 hashCode;
    XUInt32 hasHashCode: 1;
    XUInt32 __unuse: 31;
    _XBuffer * _Nonnull buffer;
} _XByteStorageContentLarge_t;

#pragma pack(pop)
#define _XByteStorageContentBufferSizeMin X_BUILD_UInt(245)

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

typedef _XByteStorage _XString;

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
    XUInt32 clearWhenDealloc: 1;
    XUInt32 hasHashCode: 1;
    XUInt32 contentSize: 30;
    XUInt32 hashCode;
    XUInt8 extended[0];
} _XValueContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XValueContent_t content;
} _XValue;

typedef struct {
    XSize contentSize;
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

//如果ref是个 TaggedObject 返回值有效，否则返回 XCompressedTypeNone
extern XCompressedType _XRefGetTaggedObjectCompressedType(XRef _Nonnull ref);

//如果ref是个 CompressedObject 返回值有效，否则返回 XCompressedTypeNone
extern _XType_s * _Nonnull _XRefGetUnpackedType(XRef _Nonnull ref, XCompressedType * _Nullable compressedType, const char * _Nonnull func);

//如果ref是个 CompressedObject 返回值有效，否则返回 XCompressedTypeNone
extern _XType_s * _Nonnull _XRefUnpackType(uintptr_t type, XCompressedType * _Nullable compressedType, const char * _Nonnull func);

extern uintptr_t _XRefGetType(XRef _Nonnull ref);
extern _XType_s * _Nonnull _XRefGetClass(XRef _Nonnull ref, const char * _Nonnull func);

extern XHashCode _XHashUInt64(XUInt64 i);
extern XHashCode _XHashSInt64(XSInt64 i);
extern XHashCode _XHashFloat64(XFloat64 d);
extern XUInt32 _XELFHashBytes(XUInt8 * _Nullable bytes, XUInt32 length, XUInt32 vi);


#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XRuntime_h */
