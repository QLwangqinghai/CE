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



//正无穷
extern const XNumber XNumberPositiveInfinity;

//负无穷
extern const XNumber kCFNumberNegativeInfinity;

//非数
extern const XNumber kCFNumberNaN;


#pragma mark - XString

extern const XString _Nonnull XStringEmpty;

#pragma mark - XData

extern const XData _Nonnull XDataEmpty;

#pragma mark - XDate

#pragma mark - XValue

extern const XValue _Nonnull XValueEmpty;


#pragma mark - XStorageRef

#pragma mark - XArrayRef
#pragma mark - XMapRef
#pragma mark - XSetRef



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
