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
//XHashCode XValueHash(XRef _Nonnull obj) {return 0;};
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
    ._runtime = _XConstantObjectBaseMake(&_XClassNull),
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
    ._runtime = _XConstantObjectBaseMake(&_XClassBoolean),
    .content = {.value = true},
};
const _XBoolean _XBooleanFalse = {
    ._runtime = _XConstantObjectBaseMake(&_XClassBoolean),
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

//XByteStorage.c


/*
32: (8 + 4 + 4 + opt12 = 16 or 28)
16 48 (128-16)
64: (8 + 4 + 4 + opt16 = 16 or 32)
16 48 (128-16) (256-16)
*/
//typedef struct {
//    _XExtendedLayout layout: 4;
//    XUInt32 hasHashCode: 1;
//    XUInt32 clearOnDealloc: 1;
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

const _XInlineEmptyString _XStringEmpty = {
    ._base = {
        ._runtime = _XConstantObjectCompressedBaseMake(X_BUILD_CompressedType_String),
        .content = {
            .layout = X_BUILD_ExtendedLayoutInline,
            .hasHashCode = 1,
            .clearOnDealloc = 0,
            .__unuse = 0,
            .inlineLength = 0,
            .hashCode = 0,
        },
    },
    .extended = {0},
};

const XString _Nonnull XStringEmpty = (XString)&_XStringEmpty;

#pragma mark - XData


const _XData _XDataEmpty = {
    ._runtime = _XConstantObjectCompressedBaseMake(X_BUILD_CompressedType_Data),
    .content = {
        .layout = X_BUILD_ExtendedLayoutInline,
        .hasHashCode = 1,
        .clearOnDealloc = 0,
        .__unuse = 0,
        .inlineLength = 0,
        .hashCode = 0,
    },
};

const XData _Nonnull XDataEmpty = (XData)&_XDataEmpty;


#pragma mark - XDate

//XDate.c


#pragma mark - XValue

#pragma mark - XStorageRef

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






