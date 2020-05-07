//
//  XRef.c
//  X
//
//  Created by vector on 2020/4/3.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "include/XRef.h"
#include "internal/XRuntimeInternal.h"


//XBool XStringEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XDataEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XDateEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XValueEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XObjectEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//
//
//XHashCode XStringHash(XRef _Nonnull obj) {return 0;};
//XHashCode XDataHash(XRef _Nonnull obj) {return 0;};
//XHashCode XDateHash(XRef _Nonnull obj) {return 0;};
//XHashCode XObjectHash(XRef _Nonnull obj) {return 0;};

#pragma mark - XClass

XBool XClassEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    XClassIdentifier lclsId = XRefGetIdentfierIfIsClass(lhs);
    XClassIdentifier rclsId = XRefGetIdentfierIfIsClass(rhs);
    if (lclsId && rclsId) {
        return lclsId == rclsId;
    } else {
        return false;
    }
};

XHashCode XClassHash(XRef _Nonnull cls) {
    XClassIdentifier clsId = XRefGetIdentfierIfIsClass(cls);
    assert(clsId);
    
    return (XHashCode)(((uintptr_t)clsId) >> 4);
}

#pragma mark - XNull

const _XNull _XNullShared = {
    ._runtime = _XConstantObjectBaseMake(&_XClassNullStorage),
};
const XNull _Nonnull XNullShared = (XNull)&_XNullShared;

XBool XNullEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    //XNull 不允许自己构建， 只可以使用 XNullShared;
    assert(lhs == XNullShared);
    assert(rhs == XNullShared);
    return true;
};

XHashCode XNullHash(XRef _Nonnull ref) {
    assert(ref == XNullShared);
    return 0;
}
extern XNull _Nonnull XNullCreate(void) {
    return XNullShared;
}

#pragma mark - XBoolean

const _XBoolean _XBooleanTrue = {
    ._runtime = _XConstantObjectBaseMake(&_XClassBooleanStorage),
    .content = {.value = true},
};
const _XBoolean _XBooleanFalse = {
    ._runtime = _XConstantObjectBaseMake(&_XClassBooleanStorage),
    .content = {.value = false},
};

const XBoolean _Nonnull XBooleanTrue = (XBoolean)&_XBooleanTrue;
const XBoolean _Nonnull XBooleanFalse = (XBoolean)&_XBooleanFalse;

XBool XBooleanEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    //XBoolean 不允许自己构建， 只可以使用 XBooleanTrue、XBooleanFalse;
    XAssert((lhs == XBooleanTrue || lhs == XBooleanFalse), __func__, "lhs error");
    XAssert((rhs == XBooleanTrue || rhs == XBooleanFalse), __func__, "rhs error");
    return lhs == rhs;
};

XHashCode XBooleanHash(XRef _Nonnull ref) {
    XAssert((ref == XBooleanTrue || ref == XBooleanFalse), __func__, "ref error");
    return ((const _XBoolean *)ref)->content.value ? 1 : 0;
}

XBoolean _Nonnull XBooleanCreate(XBool value) {
    return value ? XBooleanTrue : XBooleanFalse;
}

XBool XBooleanGetValue(XBoolean _Nonnull ref) {
    if (ref == XBooleanTrue) {
        return true;
    } else if (ref == XBooleanFalse) {
        return false;
    } else {
        XAssert(false, __func__, "ref error");
    }
}

#pragma mark - XNumber

//XNumber.c


#pragma mark - private _XByteStorage

extern _XBuffer * _Nonnull _XBufferAllocate(XPtr _Nonnull buffer, XUInt size, XBool clearWhenDealloc);
extern _XBuffer * _Nonnull _XBufferRetain(_XBuffer * _Nonnull buffer);
extern void _XBufferRelease(_XBuffer * _Nonnull buffer);
extern void _XBufferSetClearWhenDealloc(_XBuffer * _Nonnull buffer);

//XByteStorage.c


/*
(8 + 4 + 4 + opt16 = 16 or 32)
16 48 (128-16) (256-16)
*/
//typedef struct {
//    _XExtendedLayout layout: 4;
//    XUInt32 hasHashCode: 1;
//    XUInt32 clearWhenDealloc: 1;
//    XUInt32 __unuse: 10;
//    XUInt32 inlineLength: 16;//layout == _XStringLayoutInline 时有效
//    XUInt32 hashCode;
//    XUInt8 extended[0];/* 可能有0、16、48、112、 240 5种可能 */
//} _XByteContent_t;
//
//typedef struct {
//    XUInt offset: 32;
//    XUInt length: 32;
//    _XBuffer * _Nonnull buffer;
//} _XByteBufferContent_t;
//
//typedef struct {
//    _XObjectCompressedBase _runtime;
//    _XByteBufferContent_t content;
//} _XByteStorage;


#pragma mark - XString

const _XString _XStringEmpty = {
    ._runtime = _XConstantObjectCompressedBaseMake(X_BUILD_CompressedType_String),
    .content = {
        .length = 0,
        .__xx = 0,
    },
};

const XString _Nonnull XStringEmpty = (XString)&_XStringEmpty;

#pragma mark - XData


const _XData _XDataEmpty = {
    ._runtime = _XConstantObjectCompressedBaseMake(X_BUILD_CompressedType_Data),
    .content = {
        .length = 0,
        .__xx = 0,
    },
};

const XData _Nonnull XDataEmpty = (XData)&_XDataEmpty;


#pragma mark - XDate

//XDate.c


#pragma mark - XValue

const XSize XValueSizeMax = X_BUILD_ValueSizeMax;

const _XValue _XValueEmpty = {
    ._runtime = _XConstantObjectCompressedBaseMake(X_BUILD_CompressedType_Value),
    .content = {
        .clearWhenDealloc = 0,
        .contentSize = 0,
        .hashCode = ATOMIC_VAR_INIT(0),
    },
};
const XValue _Nonnull XValueEmpty = (XValue)&_XValueEmpty;

static inline XSize __XValueSizeAligned(XSize size) {
    #if BUILD_TARGET_RT_64_BIT
        XSize s = (size + 7) & (~X_BUILD_UInt(0x7));
    #else
        XSize s = (size + 3) & (~X_BUILD_UInt(0x3));
    #endif
    return s;
};
//通过copy content
XValue _Nonnull XValueCreate(XUInt flag, XPtr _Nullable content, XSize contentSize) {
    if (contentSize > 0) {
        XAssert(NULL != content, __func__, "contentSize>0, but content is NULL");
        XAssert(contentSize <= X_BUILD_ValueSizeMax, __func__, "contentSize too large");
    } else {
        XAssert(contentSize == 0, __func__, "contentSize < 0");
        return XValueEmpty;
    }
    const _XAllocator_s * allocator = (const _XAllocator_s *)(_XClassValue->base.allocator);
//    typedef XRef _Nonnull (*XRefAllocate_f)(_XAllocatorPtr _Nonnull allocator, XClass _Nonnull cls, XSize contentSize, XObjectRcFlag flag);

    XObjectRcFlag rcFlag = 0;
    
    XRef ref = allocator->allocateRef((_XAllocatorPtr)allocator, XClassValue, __XValueSizeAligned(contentSize), rcFlag);
    _XValue * valueRef = (_XValue *)(ref);

    valueRef->content.clearWhenDealloc = ((flag & XObjectFlagClearWhenDealloc) == XObjectFlagClearWhenDealloc) ? 1 : 0;
    valueRef->content.contentSize = (XUInt32)contentSize;
    atomic_store(&(valueRef->content.hashCode), XHash32NoneFlag);
    return ref;
}
static _XValue * _Nonnull __XRefAsValue(XValue _Nonnull ref, const char * _Nonnull func) {
    XCompressedType compressedType = XCompressedTypeNone;
    
#if BUILD_TARGET_RT_64_BIT
    __unused
#endif
    XClass info = _XRefGetUnpackedType(ref, &compressedType, func);
    
#if BUILD_TARGET_RT_64_BIT
    XAssert(XCompressedTypeValue == compressedType, func, "not Value instance");
    return (_XValue *)ref;
#else
    const _XType_s * type = (const _XType_s *)info;
    XAssert(type->base.identifier == _XClassTable[X_BUILD_CompressedType_Value - 1].base.identifier, func, "not Value instance");
    return (_XValue *)ref;

#endif
}
XSize XValueGetSize(XValue _Nonnull ref) {
    XAssert(NULL != ref, __func__, "require ref != NULL");
    _XValue * valueRef = __XRefAsValue(ref, __func__);
    return valueRef->content.contentSize;
}
void XValueCopyContent(XValue _Nonnull ref, XPtr _Nonnull buffer, XSize offset, XSize length) {
    XAssert(NULL != ref, __func__, "require ref != NULL");
    XAssert(NULL != buffer, __func__, "require buffer != NULL");

    _XValue * valueRef = __XRefAsValue(ref, __func__);
    if (length == 0) {
        return;
    }
    XAssert(length >= 0, __func__, "require length>=0");
    XAssert(offset >= 0, __func__, "require offset>=0");
    XAssert(length <= valueRef->content.contentSize, __func__, "range error");
    XAssert(offset <= valueRef->content.contentSize - length, __func__, "range error");
    
    memcpy(buffer, &(valueRef->content.extended[offset]), length);
}
XHashCode XValueHash(XValue _Nonnull ref) {
    XAssert(NULL != ref, __func__, "require ref != NULL");

    _XValue * valueRef = __XRefAsValue(ref, __func__);
    if (valueRef->content.hashCode >= XHash32NoneFlag) {
        XUInt32 code = _XELFHashBytes(&(valueRef->content.extended[0]), MIN(valueRef->content.contentSize, XHashEverythingLimit));
        valueRef->content.hashCode = (code & XHash32Mask);
    }
    return valueRef->content.hashCode;
}

#pragma mark - XStorageRef

const XSize XStorageSizeMax = X_BUILD_StorageSizeMax;


static inline XSize __XStorageSizeAligned(XSize size) {
    #if BUILD_TARGET_RT_64_BIT
        XSize s = (size + 7) & (~X_BUILD_UInt(0x7));
    #else
        XSize s = (size + 3) & (~X_BUILD_UInt(0x3));

    #endif
    return s;
};
extern XStorageRef _Nonnull XStorageCreate(XUInt flag, XSize size, XStorageClear_f _Nullable clear, XPtr _Nullable content, XSize contentSize) {
    if (contentSize > 0) {
        XAssert(NULL != content, __func__, "contentSize>0, but content is NULL");
        XAssert(contentSize <= X_BUILD_ValueSizeMax, __func__, "contentSize too large");
    } else {
        XAssert(contentSize == 0, __func__, "contentSize < 0");
        return XValueEmpty;
    }
    const _XAllocator_s * allocator = (const _XAllocator_s *)(_XClassStorage->base.allocator);
//    typedef XRef _Nonnull (*XRefAllocate_f)(_XAllocatorPtr _Nonnull allocator, XClass _Nonnull cls, XSize contentSize, XObjectRcFlag flag);

    XObjectRcFlag rcFlag = 0;
    
    XRef ref = allocator->allocateRef((_XAllocatorPtr)allocator, XClassStorage, __XStorageSizeAligned(contentSize), rcFlag);
    _XStorage * storageRef = (_XStorage *)(ref);

//    storageRef->content.clearWhenDealloc = ((flag & XObjectFlagClearWhenDealloc) == XObjectFlagClearWhenDealloc) ? 1 : 0;
//    storageRef->content.hasHashCode = 0;
    storageRef->content.contentSize = (XUInt32)contentSize;
//    storageRef->content.hashCode = 0;
    return ref;
}
static _XStorage * _Nonnull __XRefAsStorage(XValue _Nonnull ref, const char * _Nonnull func) {
    XCompressedType compressedType = XCompressedTypeNone;
    
#if BUILD_TARGET_RT_64_BIT
    __unused
#endif
    XClass info = _XRefGetUnpackedType(ref, &compressedType, func);
    
#if BUILD_TARGET_RT_64_BIT
    XAssert(XCompressedTypeStorage == compressedType, func, "not Value instance");
    return (_XStorage *)ref;
#else
    const _XType_s * type = (const _XType_s *)info;
    XAssert(type->base.identifier == _XClassTable[X_BUILD_CompressedType_Storage - 1].base.identifier, func, "not Value instance");
    return (_XStorage *)ref;

#endif
}
XSize XStorageGetSize(XStorageRef _Nonnull ref) {
    XAssert(NULL != ref, __func__, "require ref != NULL");
    _XStorage * storageRef = __XRefAsStorage(ref, __func__);
    return storageRef->content.contentSize;
}

XPtr _Nullable XStorageGetContent(XStorageRef _Nonnull ref) {
    XAssert(NULL != ref, __func__, "require ref != NULL");
    _XStorage * storageRef = __XRefAsStorage(ref, __func__);
    if (storageRef->content.contentSize == 0) {
        return NULL;
    }
    return &(storageRef->content.extended[0]);
}


#pragma mark - XArrayRef
#pragma mark - XMapRef
#pragma mark - XSetRef




//typedef struct {
//    _XObjectBase _runtime;
//} _XNull;
//
//typedef struct {
//    _XObjectBase _runtime;
//    XBool value;
//} _XBoolean;
//
//typedef struct {
//    _XObjectCompressedBase _runtime;
//    XSize size;
//    XUInt8 content[0];
//} _XNumber;
//
//typedef struct {
//    _XObjectCompressedBase _runtime;
//    XSize size;
//    XUInt8 content[0];
//} _XValue;


#pragma mark - XRef

XRef _Nonnull XRefRetain(XRef _Nonnull ref) {
    return ref;
}
XRef _Nullable XRefTryRetain(XRef _Nonnull ref) {
    return ref;
}
void XRefRelease(XRef _Nonnull ref) {
    
}


