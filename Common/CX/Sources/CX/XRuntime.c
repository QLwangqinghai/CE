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

const XRefKind XRefKindUnknown = 0;
const XRefKind XRefKindNormal = 1;
const XRefKind XRefKindClass = 2;
const XRefKind XRefKindMetaClass = 3;

const XCompressedType XCompressedTypeNumber = 1;
const XCompressedType XCompressedTypeString = 2;
const XCompressedType XCompressedTypeData = 3;
const XCompressedType XCompressedTypeDate = 4;
const XCompressedType XCompressedTypeValue = 5;
const XCompressedType XCompressedTypeStorage = 6;
const XCompressedType XCompressedTypeArray = 7;
const XCompressedType XCompressedTypeMap = 8;
const XCompressedType XCompressedTypeSet = 9;

XBool XNumberIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
XBool XStringIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
XBool XDataIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
XBool XDateIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
XBool XValueIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};

XHashCode XNumberHash(XRef _Nonnull obj) {return 0;};
XHashCode XStringHash(XRef _Nonnull obj) {return 0;};
XHashCode XDataHash(XRef _Nonnull obj) {return 0;};
XHashCode XDateHash(XRef _Nonnull obj) {return 0;};
XHashCode XValueHash(XRef _Nonnull obj) {return 0;};

#define _XTypeIdentifierMakeValue(Type) \
{\
    .super = NULL,\
    .name = #Type,\
    .hashCode = X##Type##Hash,\
    .equalTo = X##Type##IsEqualTo,\
}\

#define _XTypeIdentifierMakeRootObject(Type) \
{\
    .super = NULL,\
    .name = #Type,\
    .hashCode = XObjectHash,\
    .equalTo = XObjectIsEqualTo,\
}\

const _XTypeIdentifier_s _XTypeIdentifierTable[] = {
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



const _XType_s _XMetaClass = {
    ._runtime = {
        .typeInfo = ATOMIC_VAR_INIT((uintptr_t)&_XMetaClass),
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


#define _XCompressedValueClassMake(Type) \
{\
    ._runtime = {\
        .typeInfo = (uintptr_t)&_XMetaClass,\
        .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectFlagReadOnly)),\
    },\
    .base = {\
        .identifier = &_XTypeIdentifierTable[X_BUILD_TypeIdentifier_##Type],\
        .kind = XTypeKindValue,\
        .xx = 0,\
        .domain = 0,\
        .tableSize = 0,\
        .super = (uintptr_t)NULL,\
        .allocator = &_XObjectAllocator,\
        .deinit = NULL,\
        .describe = NULL,\
    },\
}

#define _XConstantValueClassMake(Type) \
{\
    ._runtime = {\
        .typeInfo = (uintptr_t)&_XMetaClass,\
        .rcInfo = ATOMIC_VAR_INIT((X_BUILD_ObjectRcMax | X_BUILD_ObjectFlagReadOnly)),\
    },\
    .base = {\
        .identifier = &_XTypeIdentifierTable[X_BUILD_TypeIdentifier_##Type],\
        .kind = XTypeKindValue,\
        .xx = 0,\
        .domain = 0,\
        .tableSize = 0,\
        .super = (uintptr_t)NULL,\
        .allocator = NULL,\
        .deinit = NULL,\
        .describe = NULL,\
    },\
}

#define _XValueClassMake(kind, Type) \
{\
    ._runtime = {\
        .typeInfo = (uintptr_t)&_XMetaClass,\
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


//Value = 1;
//Object = 2;
//WeakableObject = 3;
//MetaClass = 0xf;

const _XType_s _XClassTable[] = {
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

const _XType_s _XClassNull = _XConstantValueClassMake(Null);
const _XType_s _XClassBoolean = _XConstantValueClassMake(Boolean);


const XClass _Nonnull XClassType = (XClass)&_XMetaClass;//0
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



/*
 #if BUILD_TARGET_RT_64_BIT

 typedef struct {
     _Atomic(uintptr_t) typeInfo;
 } _XObjectCompressedBase;

 typedef struct {
     _Atomic(uintptr_t) typeInfo;
     _Atomic(XFastUInt) rcInfo;
 } _XObjectBase;

 #elif BUILD_TARGET_RT_32_BIT

 typedef struct {
     _Atomic(uintptr_t) typeInfo;
     _Atomic(XFastUInt) rcInfo;
 } _XObjectCompressedBase;

 typedef _XObjectCompressedBase _XObjectBase;
 #else
     #error unknown rt
 #endif
 */

XRefKind XRefGetKind(XRef _Nonnull ref) {
    if (NULL == ref) {
        return XRefKindUnknown;
    }
    uintptr_t cls = _XRefGetTypeInfo(ref);
    
#if BUILD_TARGET_RT_64_BIT
    /* flagBegin:3 type:6 flag:4 count:18+32 flagEnd:1  */
    if (0 == (cls & X_BUILD_TypeInfoCompressedMask)) {
        uintptr_t id = (cls >> 55) & 0x3f;
        XClass cls = _XRefGetCompressedType(id);
        assert(cls);
        return XRefKindNormal;
    }
#endif
    
    const _XType_s * type = (const _XType_s *)cls;
    assert(type);
    if (XTypeKindMetaClass == type->base.kind) {
        const _XType_s * cls = (const _XType_s *)ref;
        if (XTypeKindMetaClass == cls->base.kind) {
            return XRefKindMetaClass;
        } else {
            return XRefKindClass;
        }
    } else if (XTypeKindValue == type->base.kind || XTypeKindObject == type->base.kind || XTypeKindWeakableObject == type->base.kind) {
        return XRefKindNormal;
    } else {
        return XRefKindUnknown;
    }
}

XClassIdentifier _Nullable XRefGetIdentfierIfIsClass(XRef _Nonnull ref) {
    assert(ref);
    const _XType_s * type = (const _XType_s *)XRefGetType(ref);
    if (XTypeKindMetaClass == type->base.kind) {
        const _XType_s * cls = (const _XType_s *)ref;
        return (XClassIdentifier)(cls->base.identifier);
    } else {
        return NULL;
    }
}

XBool XRefIsMetaClass(XRef _Nonnull ref) {
    assert(ref);
    const _XType_s * type = (const _XType_s *)XRefGetType(ref);
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


uintptr_t _XRefGetTypeInfo(XRef _Nonnull ref) {
    _Atomic(uintptr_t) * tmp = (_Atomic(uintptr_t) *)ref;
    uintptr_t info = atomic_load(tmp);
    return info;
}

XClass _Nullable _XRefGetType(XRef _Nonnull ref) {
    uintptr_t info = _XRefGetTypeInfo(ref);
    
#if BUILD_TARGET_RT_64_BIT
    /* flagBegin:3 type:6 flag:4 count:18+32 flagEnd:1  */
    if (0 == (info & X_BUILD_TypeInfoCompressedMask)) {
        uintptr_t id = (info >> 55) & 0x3f;
        XClass cls = _XRefGetCompressedType(id);
        return cls;
    }
#endif
    return (XClass)info;
}

XClass _Nonnull XRefGetType(XRef _Nonnull ref) {
    assert(ref);
    XClass info = _XRefGetType(ref);
    assert(info);
    return info;
}

