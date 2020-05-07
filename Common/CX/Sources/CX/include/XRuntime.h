//
//  XRuntime.h
//  
//
//  Created by vector on 2020/4/4.
//

#ifndef XRuntime_h
#define XRuntime_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"


typedef XUInt32 XRefKind;
typedef XUInt32 XTypeKind;

extern const XTypeKind XTypeKindValue;//1
extern const XTypeKind XTypeKindObject;//2
extern const XTypeKind XTypeKindWeakableObject;//3

//extern const XRefKind XRefKindUnknown;
extern const XRefKind XRefKindInstance;
extern const XRefKind XRefKindClass;
extern const XRefKind XRefKindMetaClass;


typedef XUInt32 XNumberType;


#define X_BUILD_ObjectRcFlagReadOnly ((XUInt)1)

#if BUILD_TARGET_RT_64_BIT
    #define X_BUILD_UInt(value) value##ULL
#else
    #define X_BUILD_UInt(value) value##UL

#endif


#define X_BUILD_CompressedType_Number X_BUILD_UInt(1)
#define X_BUILD_CompressedType_String X_BUILD_UInt(2)
#define X_BUILD_CompressedType_Data X_BUILD_UInt(3)
#define X_BUILD_CompressedType_Date X_BUILD_UInt(4)
#define X_BUILD_CompressedType_Value X_BUILD_UInt(5)
#define X_BUILD_CompressedType_Storage X_BUILD_UInt(6)
#define X_BUILD_CompressedType_Array X_BUILD_UInt(7)
#define X_BUILD_CompressedType_Map X_BUILD_UInt(8)
#define X_BUILD_CompressedType_Set X_BUILD_UInt(9)

typedef XUInt XCompressedType;
extern const XCompressedType XCompressedTypeNone;
extern const XCompressedType XCompressedTypeNumber;
extern const XCompressedType XCompressedTypeString;
extern const XCompressedType XCompressedTypeData;
extern const XCompressedType XCompressedTypeDate;
extern const XCompressedType XCompressedTypeValue;
extern const XCompressedType XCompressedTypeArray;
extern const XCompressedType XCompressedTypeMap;
extern const XCompressedType XCompressedTypeSet;
extern const XCompressedType XCompressedTypeStorage;

typedef XUInt XObjectRcFlag;
extern const XObjectRcFlag XObjectRcFlagReadOnly;
extern const XObjectRcFlag XObjectRcFlagStatic;


typedef XUInt XObjectFlag;

//初始化单例对象， 不进行引用计数、一直不释放
extern const XObjectFlag XObjectFlagStatic;

//对象释放时clear内存, XString、XData、XValue、XStorage 支持
extern const XObjectFlag XObjectFlagClearWhenDealloc;




extern const XClass _Nonnull XClassType;//0
extern const XClass _Nonnull XClassNull;//1
extern const XClass _Nonnull XClassBoolean;//2

extern const XClass _Nonnull XClassNumber;//3
extern const XClass _Nonnull XClassString;//4
extern const XClass _Nonnull XClassData;//5
extern const XClass _Nonnull XClassDate;//6
extern const XClass _Nonnull XClassValue;//7
extern const XClass _Nonnull XClassStorage;//8

extern const XClass _Nonnull XClassArray;//9
extern const XClass _Nonnull XClassMap;//10
extern const XClass _Nonnull XClassSet;//11

struct _XAllocator;
typedef const struct _XAllocator * _XAllocatorPtr;

typedef XRef _Nonnull (*XRefAllocate_f)(_XAllocatorPtr _Nonnull allocator, XClass _Nonnull cls, XSize contentSize, XObjectRcFlag flag);
typedef void (*XRefDeallocate_f)(_XAllocatorPtr _Nonnull allocator, XObject _Nonnull obj);

typedef XBool (*XRefEqualTo_f)(XRef _Nonnull lhs, XRef _Nonnull rhs);
typedef XHashCode (*XRefHashCode_f)(XObject _Nonnull obj);

extern XRefKind XRefGetKind(XRef _Nonnull ref);
extern XClassIdentifier _Nullable XRefGetIdentfierIfIsClass(XRef _Nonnull ref);
extern XClass _Nonnull XRefGetClass(XRef _Nonnull ref);
extern XBool XRefIsMetaClass(XRef _Nonnull ref);


#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XRuntime_h */
