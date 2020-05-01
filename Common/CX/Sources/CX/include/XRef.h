//
//  XRef.h
//  
//
//  Created by vector on 2020/4/4.
//

#ifndef XRef_h
#define XRef_h


#if defined(__cplusplus)
extern "C" {
#endif

#include "XRuntime.h"

#pragma mark - XClass



#pragma mark - XNull

extern const XNull _Nonnull XNullShared;
extern XNull _Nonnull XNullCreate(void);

#pragma mark - XBoolean

extern const XBoolean _Nonnull XBooleanTrue;
extern const XBoolean _Nonnull XBooleanFalse;
extern XBoolean _Nonnull XBooleanCreate(XBool value);
extern XBool XBooleanGetValue(XBoolean _Nonnull ref);

#pragma mark - XNumber

extern const XNumberType XNumberTypeSInt8;
extern const XNumberType XNumberTypeUInt8;

extern const XNumberType XNumberTypeSInt16;
extern const XNumberType XNumberTypeUInt16;

extern const XNumberType XNumberTypeSInt32;
extern const XNumberType XNumberTypeUInt32;
extern const XNumberType XNumberTypeFloat32;

extern const XNumberType XNumberTypeSInt64;
extern const XNumberType XNumberTypeUInt64;
extern const XNumberType XNumberTypeFloat64;

extern XNumber _Nonnull XNumberCreate(XNumberType theType, const void * _Nonnull valuePtr);

extern XComparisonResult XNumberCompare(XNumber _Nonnull lhs, XNumber _Nonnull rhs);

extern XBool XNumberGetValue(XNumber _Nonnull ref, XNumberType theType, void * _Nonnull valuePtr);
extern XNumberType XNumberGetType(XNumber _Nonnull ref);
extern XBool XNumberIsFloatType(XNumber _Nonnull ref);
extern XBool XNumberIsSignedType(XNumber _Nonnull ref);

#pragma mark - XString

extern const XString _Nonnull XStringEmpty;

#pragma mark - XData

extern const XData _Nonnull XDataEmpty;

#pragma mark - XDate

extern const XDate _Nonnull XDateMin;
extern const XDate _Nonnull XDateMax;

//time 是以 2020年1月1日00:00 作为原点的时间
extern XDate _Nonnull XDateCreate(XUInt flag, XTimeInterval time);
extern XComparisonResult XDateCompare(XDate _Nonnull lhs, XDate _Nonnull rhs);
extern XTimeInterval XDateGetValue(XDate _Nonnull ref);

#pragma mark - XValue

extern const XValue _Nonnull XValueEmpty;

//通过copy content
extern XValue _Nonnull XValueCreate(XUInt flag, XPtr _Nonnull content, XSize contentSize);
extern XSize XValueGetSize(XValue _Nonnull ref);
extern void XValueCopyContent(XValue _Nonnull ref, XPtr _Nonnull buffer, XSize offset, XSize length);

#pragma mark - XStorageRef

typedef void (*XStorageClear_f)(XUInt8 * _Nullable content, XSize size);

//size 必须 > 0, 当 contentSize > 0 时，content 必须有值
extern XStorageRef _Nonnull XStorageCreate(XUInt flag, XSize size, XStorageClear_f _Nullable clear, XPtr _Nullable content, XSize contentSize);

extern XSize XStorageGetSize(XStorageRef _Nonnull ref);
extern XPtr _Nonnull XStorageGetContent(XStorageRef _Nonnull ref);

#pragma mark - XArrayRef


#pragma mark - XMapRef


#pragma mark - XSetRef





#pragma mark - XRef


extern XRef _Nonnull XRefRetain(XRef _Nonnull ref);
extern XRef _Nullable XRefTryRetain(XRef _Nonnull ref);
extern void XRefRelease(XRef _Nonnull ref);


#pragma mark - equalTo

extern XBool XClassEqual(XRef _Nonnull lhs, XRef _Nonnull rhs);
extern XBool XNullEqual(XRef _Nonnull lhs, XRef _Nonnull rhs);
extern XBool XBooleanEqual(XRef _Nonnull lhs, XRef _Nonnull rhs);
extern XBool XNumberEqual(XRef _Nonnull lhs, XRef _Nonnull rhs);
extern XBool XStringEqual(XRef _Nonnull lhs, XRef _Nonnull rhs);
extern XBool XDataEqual(XRef _Nonnull lhs, XRef _Nonnull rhs);
extern XBool XDateEqual(XRef _Nonnull lhs, XRef _Nonnull rhs);
extern XBool XValueEqual(XRef _Nonnull lhs, XRef _Nonnull rhs);

#pragma mark - hash

extern XHashCode XClassHash(XRef _Nonnull ref);
extern XHashCode XNullHash(XRef _Nonnull ref);
extern XHashCode XBooleanHash(XRef _Nonnull ref);
extern XHashCode XNumberHash(XRef _Nonnull ref);
extern XHashCode XStringHash(XRef _Nonnull ref);
extern XHashCode XDataHash(XRef _Nonnull ref);
extern XHashCode XDateHash(XRef _Nonnull ref);
extern XHashCode XValueHash(XRef _Nonnull ref);

#if defined(__cplusplus)

}  // extern C
#endif

#endif /* XRef_h */
