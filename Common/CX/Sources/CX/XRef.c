//
//  XRef.c
//  X
//
//  Created by vector on 2020/4/3.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "include/XRef.h"

#include "internal/XRuntimeInternal.h"


//XBool XClassIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XNullIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XBooleanIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XNumberIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XStringIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XDataIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XDateIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XValueIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//XBool XObjectIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {return false;};
//
//
//XHashCode XClassHash(XRef _Nonnull obj) {return 0;};
//XHashCode XNullHash(XRef _Nonnull obj) {return 0;};
//XHashCode XBooleanHash(XRef _Nonnull obj) {return 0;};
//XHashCode XNumberHash(XRef _Nonnull obj) {return 0;};
//XHashCode XStringHash(XRef _Nonnull obj) {return 0;};
//XHashCode XDataHash(XRef _Nonnull obj) {return 0;};
//XHashCode XDateHash(XRef _Nonnull obj) {return 0;};
//XHashCode XValueHash(XRef _Nonnull obj) {return 0;};
//XHashCode XObjectHash(XRef _Nonnull obj) {return 0;};

#pragma mark - XClass

XBool XClassIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {
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

XBool XNullIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {
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

XBool XBooleanIsEqualTo(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    //XBoolean 不允许自己构建， 只可以使用 XBooleanTrue、XBooleanFalse;
    assert(lhs == XBooleanTrue || lhs == XBooleanFalse);
    assert(rhs == XBooleanTrue || rhs == XBooleanFalse);
    return lhs == rhs;
};

XHashCode XBooleanHash(XRef _Nonnull ref) {
    assert(ref == XBooleanTrue || ref == XBooleanFalse);
    return ((const _XBoolean *)ref)->content.value ? 1 : 0;
}

extern XBoolean _Nonnull XBooleanCreate(XBool value) {
    return value ? XBooleanTrue : XBooleanFalse;
}


#pragma mark - XNumber

//XNumber.c

#pragma mark - XString

/*
32:
base: 4
layout + length: 4
hashCode: 4
content: 4, 20, 52

64:
base: 8
layout + length: 4
hashCode: 4
content: 16, 48, 112
*/
const _XStringLayout _XStringLayoutInline = 0;

/*
 32: =24
 base: 4
 layout + xxx: 4
 hashCode: 4
 length: 4
 offset: 4
 buffer: 4
 
 64: = 32
 base: 8
 layout + xxx: 4
 hashCode: 4
 length: 4
 offset: 4
 buffer: 8
 */
const _XStringLayout _XStringLayoutBuffer = 1;

/*
 typedef struct {
     XUInt layout: 4;
     XUInt hasHashCode: 1;
     XUInt __unuse: 3;
     XUInt mask: 8;
     XUInt inlineLength: 16;
     XUInt hashCode: 32;
 } _XStringContent_t;

 */
const _XInlineEmptyString _XStringEmpty = {
    ._runtime = _XConstantObjectCompressedBaseMake(X_BUILD_CompressedType_String),
    .content = {
        .layout = 0,
        .hasHashCode = 1,
        .__unuse = 0,
        .mask = 0,
        .inlineLength = sizeof(XUInt),
        .hashCode = 0,
        .extended = {0},
    },
};

const XString _Nonnull XStringEmpty = (XString)&_XStringEmpty;

#pragma mark - XData

#pragma mark - XDate

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






