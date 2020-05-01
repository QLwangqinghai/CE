//
//  XRuntime.c
//  
//
//  Created by vector on 2020/4/4.
//

#include "include/XRuntime.h"
#include "internal/XRuntimeInternal.h"
#include <stdatomic.h>

#include "XRef.h"
#include "XObject.h"

_Static_assert(sizeof(XUInt) == sizeof(size_t), "unknown error");
_Static_assert(sizeof(_Atomic(uintptr_t)) == sizeof(XUInt), "unknown error");
_Static_assert(BUILD_TARGET_RT_64_BIT || BUILD_TARGET_RT_32_BIT, "unknown rt");

_Static_assert(X_BUILD_ObjectFlagReadOnly == 1, "X_BUILD_ObjectFlagReadOnly must be 1");
_Static_assert(X_BUILD_ObjectFlagClearOnDealloc == 2, "X_BUILD_ObjectFlagClearOnDealloc must be 2");


_Static_assert(X_BUILD_ObjectRcMax == (XUIntMax - 15), "X_BUILD_ObjectRcMax error");


const XUInt XObjectFlagReadOnly = X_BUILD_ObjectFlagReadOnly;
const XUInt XObjectFlagClearOnDealloc = X_BUILD_ObjectFlagClearOnDealloc;

const XTypeKind XTypeKindValue = 1;
const XTypeKind XTypeKindObject = 2;
const XTypeKind XTypeKindWeakableObject = 3;
const XTypeKind XTypeKindMetaClass = 0xf;

//const XRefKind XRefKindUnknown = 0;
const XRefKind XRefKindInstance = 1;
const XRefKind XRefKindClass = 2;
const XRefKind XRefKindMetaClass = 3;


const XCompressedType XCompressedTypeNone = 0;
const XCompressedType XCompressedTypeNumber = 1;
const XCompressedType XCompressedTypeString = 2;
const XCompressedType XCompressedTypeData = 3;
const XCompressedType XCompressedTypeDate = 4;
const XCompressedType XCompressedTypeValue = 5;
const XCompressedType XCompressedTypeStorage = 6;
const XCompressedType XCompressedTypeArray = 7;
const XCompressedType XCompressedTypeMap = 8;
const XCompressedType XCompressedTypeSet = 9;

XBool XStringEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
XBool XDataEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
XBool XValueEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};

XHashCode XStringHash(XRef _Nonnull obj) {return 0;};
XHashCode XDataHash(XRef _Nonnull obj) {return 0;};
XHashCode XValueHash(XRef _Nonnull obj) {return 0;};

#define _XTypeIdentifierMakeValue(Type) \
{\
    .super = NULL,\
    .name = #Type,\
    .hashCode = X##Type##Hash,\
    .equalTo = X##Type##Equal,\
}\

#define _XTypeIdentifierMakeRootObject(Type) \
{\
    .super = NULL,\
    .name = #Type,\
    .hashCode = XObjectHash,\
    .equalTo = XObjectEqual,\
}\

//要求指针类型必须8字节对齐， 不管是32 位机器 还是64位
const _XTypeIdentifier_s _XTypeIdentifierTable[] __attribute__((aligned(8))) = {
    _XTypeIdentifierMakeValue(Class),
    _XTypeIdentifierMakeValue(Null),
    _XTypeIdentifierMakeValue(Boolean),
    _XTypeIdentifierMakeValue(Number),
    _XTypeIdentifierMakeValue(String),
    _XTypeIdentifierMakeValue(Data),
    _XTypeIdentifierMakeValue(Date),
    _XTypeIdentifierMakeValue(Value),
    _XTypeIdentifierMakeRootObject(Storage),
    _XTypeIdentifierMakeRootObject(Array),
    _XTypeIdentifierMakeRootObject(Map),
    _XTypeIdentifierMakeRootObject(Set),
};

#define X_BUILD_TypeIdentifier_Class 0
#define X_BUILD_TypeIdentifier_Null 1
#define X_BUILD_TypeIdentifier_Boolean 2
#define X_BUILD_TypeIdentifier_Number 3
#define X_BUILD_TypeIdentifier_String 4
#define X_BUILD_TypeIdentifier_Data 5
#define X_BUILD_TypeIdentifier_Date 6
#define X_BUILD_TypeIdentifier_Value 7
#define X_BUILD_TypeIdentifier_Storage 8
#define X_BUILD_TypeIdentifier_Array 9
#define X_BUILD_TypeIdentifier_Map 10
#define X_BUILD_TypeIdentifier_Set 11

const XClassIdentifier _Nonnull XMateClassIdentifier = (XClassIdentifier)&(_XTypeIdentifierTable[X_BUILD_TypeIdentifier_Class]);


//这是一个常量
const _XType_s _XRootMetaClass __attribute__((aligned(8))) = {
    ._runtime = {
        .typeInfo = ATOMIC_VAR_INIT((uintptr_t)&_XRootMetaClass),
        .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectFlagReadOnly)),
    },
    .base = {
        .identifier = &_XTypeIdentifierTable[0],
        .kind = XTypeKindMetaClass,
        .domain = 0,
        .tableSize = 0,
        .super = (uintptr_t)NULL,
        .allocator = &_XConstantClassAllocator,
        .deinit = NULL,
        .describe = NULL,
    },
};

const _XType_s _XMetaClass __attribute__((aligned(8))) = {
    ._runtime = {
        .typeInfo = ATOMIC_VAR_INIT((uintptr_t)&_XRootMetaClass),
        .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectFlagReadOnly)),
    },
    .base = {
        .identifier = &_XTypeIdentifierTable[0],
        .kind = XTypeKindMetaClass,
        .domain = 0,
        .tableSize = 0,
        .super = (uintptr_t)NULL,
        .allocator = &_XClassAllocator,
        .deinit = NULL,
        .describe = NULL,
    },
};


#define _XConstantValueClassMake(Type) \
{\
    ._runtime = {\
        .typeInfo = (uintptr_t)&_XRootMetaClass,\
        .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectFlagReadOnly)),\
    },\
    .base = {\
        .identifier = &_XTypeIdentifierTable[X_BUILD_TypeIdentifier_##Type],\
        .kind = XTypeKindValue,\
        .xx = 0,\
        .domain = 0,\
        .tableSize = 0,\
        .super = (uintptr_t)NULL,\
        .allocator = &_XConstantAllocator,\
        .deinit = NULL,\
        .describe = NULL,\
    },\
}

#define _XValueClassMake(kind, Type) \
{\
    ._runtime = {\
        .typeInfo = (uintptr_t)&_XRootMetaClass,\
        .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectFlagReadOnly)),\
    },\
    .base = {\
        .identifier = &_XTypeIdentifierTable[X_BUILD_TypeIdentifier_##Type],\
        .kind = XTypeKind##kind,\
        .domain = 0,\
        .tableSize = 0,\
        .super = NULL,\
        .allocator = &_XObjectAllocator,\
        .deinit = NULL,\
        .describe = NULL,\
    },\
}

#define _XCompressedValueClassMake(Type) \
{\
    ._runtime = {\
        .typeInfo = (uintptr_t)&_XRootMetaClass,\
        .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectFlagReadOnly)),\
    },\
    .base = {\
        .identifier = &_XTypeIdentifierTable[X_BUILD_TypeIdentifier_##Type],\
        .kind = XTypeKindValue,\
        .xx = 0,\
        .domain = 0,\
        .tableSize = 0,\
        .super = (uintptr_t)NULL,\
        .allocator = &_XCompressedObjectAllocator,\
        .deinit = NULL,\
        .describe = NULL,\
    },\
}

//Value = 1;
//Object = 2;
//WeakableObject = 3;
//MetaClass = 0xf;

const _XType_s _XClassTable[] __attribute__((aligned(8))) = {
    _XCompressedValueClassMake(Number),
    _XCompressedValueClassMake(String),
    _XCompressedValueClassMake(Data),
    _XCompressedValueClassMake(Date),
    _XCompressedValueClassMake(Value),
    _XCompressedValueClassMake(Storage),
    _XCompressedValueClassMake(Array),
    _XCompressedValueClassMake(Map),
    _XCompressedValueClassMake(Set),
};

const _XType_s _XClassNull __attribute__((aligned(8))) = _XConstantValueClassMake(Null);
const _XType_s _XClassBoolean __attribute__((aligned(8))) = _XConstantValueClassMake(Boolean);


const XClass _Nonnull XClassType = (XClass)&_XRootMetaClass;//0
const XClass _Nonnull XClassNull = (XClass)&_XClassNull;//1
const XClass _Nonnull XClassBoolean = (XClass)&_XClassBoolean;//2

const XClass _Nonnull XClassNumber = (XClass)&(_XClassTable[X_BUILD_CompressedType_Number - 1]);//3
const XClass _Nonnull XClassString = (XClass)&(_XClassTable[X_BUILD_CompressedType_String - 1]);//4
const XClass _Nonnull XClassData = (XClass)&(_XClassTable[X_BUILD_CompressedType_Data - 1]);//5
const XClass _Nonnull XClassDate = (XClass)&(_XClassTable[X_BUILD_CompressedType_Date - 1]);//6
const XClass _Nonnull XClassValue = (XClass)&(_XClassTable[X_BUILD_CompressedType_Value - 1]);//7
const XClass _Nonnull XClassStorage = (XClass)&(_XClassTable[X_BUILD_CompressedType_Storage - 1]);//8

const XClass _Nonnull XClassArray = (XClass)&(_XClassTable[X_BUILD_CompressedType_Array - 1]);//9
const XClass _Nonnull XClassMap = (XClass)&(_XClassTable[X_BUILD_CompressedType_Map - 1]);//10
const XClass _Nonnull XClassSet = (XClass)&(_XClassTable[X_BUILD_CompressedType_Set - 1]);//11


//64bit 压缩的头
 /* flagBegin:3 type:6 (rcInfo54(flag:4 count:18+32)) flagEnd:1  */

//32bit rc info
/* flag:4 count:18+32 */

XRefKind XRefGetKind(XRef _Nonnull ref) {
    XAssert(NULL != ref, __func__, "ref is NULL");

    XCompressedType compressedType = _XRefGetTaggedObjectCompressedType(ref);
    if (XCompressedTypeNone != compressedType) {
        return XRefKindInstance;
    }
    const _XType_s * type = _XRefGetUnpackedType(ref, &compressedType, __func__);
    assert(type);
    if (XTypeKindMetaClass == type->base.kind) {
        //ref 是一个类对象
        const _XType_s * cls = (const _XType_s *)ref;
        if (XTypeKindMetaClass == cls->base.kind) {
            return XRefKindMetaClass;
        } else {//ref 是个普通的类
            return XRefKindClass;
        }
    } else if (XTypeKindValue == type->base.kind || XTypeKindObject == type->base.kind || XTypeKindWeakableObject == type->base.kind) {
        return XRefKindInstance;
    } else {
        XAssert(false, __func__, "unknown error");
    }
}

XClassIdentifier _Nullable XRefGetIdentfierIfIsClass(XRef _Nonnull ref) {
    assert(ref);
    const _XType_s * type = (const _XType_s *)XRefGetClass(ref);
    if (XTypeKindMetaClass == type->base.kind) {
        const _XType_s * cls = (const _XType_s *)ref;
        return (XClassIdentifier)(cls->base.identifier);
    } else {
        return NULL;
    }
}

XBool XRefIsMetaClass(XRef _Nonnull ref) {
    assert(ref);
    const _XType_s * type = (const _XType_s *)XRefGetClass(ref);
    if (XTypeKindMetaClass == type->base.kind) {
        //is a class
        const _XType_s * cls = (const _XType_s *)ref;
        if (XTypeKindMetaClass == cls->base.kind) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


uintptr_t _XRefGetType(XRef _Nonnull ref) {
    _Atomic(uintptr_t) * tmp = (_Atomic(uintptr_t) *)ref;
    uintptr_t info = atomic_load(tmp);
    return info;
}

#if BUILD_TARGET_RT_64_BIT
    #define X_BUILD_UInt(value) value##ULL
#elif BUILD_TARGET_RT_32_BIT
    #define X_BUILD_UInt(value) value##UL
#else
    #error unknown rt
#endif


static const XCompressedType _XRefTaggedObjectClassTable[4] = {XCompressedTypeNumber, XCompressedTypeString, XCompressedTypeData, XCompressedTypeDate};

XCompressedType _XRefGetTaggedObjectCompressedType(XRef _Nonnull ref) {
    XUInt v = (XUInt)((uintptr_t)ref);
    if ((v & X_BUILD_TaggedMask) == X_BUILD_TaggedObjectFlag) {
        XUInt id = ((X_BUILD_TaggedObjectClassMask & v) >> X_BUILD_TaggedObjectClassShift);
        //id 只有4个值
        return _XRefTaggedObjectClassTable[id];
    } else {
        return XCompressedTypeNone;
    }
}


_XType_s * _Nonnull _XRefUnpackType(uintptr_t info, XCompressedType * _Nullable compressedTypePtr, const char * _Nonnull func) {
    XAssert((uintptr_t)NULL != info, func, "not ref");
#if BUILD_TARGET_RT_64_BIT
    if((info & X_BUILD_TaggedMask) == X_BUILD_TaggedObjectHeaderFlag) {
        XCompressedType clsId = (XCompressedType)((info & X_BUILD_TaggedObjectHeaderClassMask) >> X_BUILD_TaggedObjectHeaderClassShift);
        
        XClass type = _XRefGetClassWithCompressedType(clsId);
        XAssert(NULL != type, func, "not ref");
        if (NULL != compressedTypePtr) {
            *compressedTypePtr = clsId;
        }
        return type;
    }
#endif
    if (NULL != compressedTypePtr) {
        *compressedTypePtr = XCompressedTypeNone;
    }
    return (XClass)info;
}
_XType_s * _Nonnull _XRefGetUnpackedType(XRef _Nonnull ref, XCompressedType * _Nullable compressedType, const char * _Nonnull func) {
    uintptr_t info = _XRefGetType(ref);
    return _XRefUnpackType(info, compressedType, func);
}





/*
 32:
 bitcount: 6 .. 25 .. 1
 bitcount: 6 .. 57 .. 1
 */
_XType_s * _Nonnull _XRefGetClass(XRef _Nonnull ref, const char * _Nonnull func) {
    XCompressedType compressedType = _XRefGetTaggedObjectCompressedType(ref);
    XClass result = NULL;
    if (XCompressedTypeNone != compressedType) {
        result = _XRefGetClassWithCompressedType(compressedType);
        XAssert(NULL != result, func, "unknown error");
        return result;
    }
    return _XRefGetUnpackedType(ref, NULL, func);
}

XClass _Nonnull XRefGetClass(XRef _Nonnull ref) {
    assert(ref);
    XClass info = (XClass)_XRefGetClass(ref, __func__);
    return info;
}





#define HASHFACTOR 2654435761U

XHashCode _XHashUInt64(XUInt64 i) {
    return (XHashCode)(i);
}
XHashCode _XHashSInt64(XSInt64 i) {
    XUInt64 u = 0;
    if (i < 0) {
        u = (XUInt64)(i * -1);
    } else {
        u = (XUInt64)i;
    }
    return _XHashUInt64(u);
}
XHashCode _XHashFloat64(XFloat64 d) {
    //转化成正数
    const XFloat64 positive = (d < 0) ? -d : d;
    
    //四舍五入
    const XFloat64 positiveInt = floor(positive + 0.5);
    
    //小数部分
    const XFloat64 fractional = (positive - positiveInt) * 18446744073709551616.0L;

    XUInt64 result = (XUInt64)fmod(positiveInt, 18446744073709551616.0L);
    if (fractional < 0) {
        result += -((XUInt64)(fabs(fractional)));
    } else if (fractional > 0) {
        result += (XUInt64)fractional;
    }
    return result;
}
