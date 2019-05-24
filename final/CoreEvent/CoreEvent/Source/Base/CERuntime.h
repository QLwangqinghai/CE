//
//  CERuntime.h
//  CoreEvent
//
//  Created by vector on 2019/5/11.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CERuntime_h
#define CERuntime_h

#include "CEBase.h"

#define CERuntimeVersion 1

typedef void * CERef;

typedef CERef _Nonnull (*CERuntimeRetain_f)(CERef _Nonnull object);
typedef CERef _Nullable (*CERuntimeTryRetain_f)(CERef _Nonnull object);
typedef void (*CERuntimeRelease_f)(CERef _Nonnull object);


typedef void (*CEDescript_f)(void const * _Nonnull handler, char const * _Nonnull buffer);

struct __CERuntimeBase;
typedef struct __CERuntimeBase CERuntimeBase_t;

struct __CERuntimeAtomicRcBase;
typedef struct __CERuntimeAtomicRcBase CERuntimeAtomicRcBase_t;

struct __CERuntimeUnsafeRcBase;
typedef struct __CERuntimeUnsafeRcBase CERuntimeUnsafeRcBase_t;

struct __CEType;
typedef struct __CEType CEType_s;
typedef CEType_s const * CETypeRef;


struct __CEAlloctor;
typedef struct __CEAlloctor CEAlloctor_s;
typedef const CEAlloctor_s * CEAlloctorRef;

typedef uint16_t CETypeMask_t;

#define CETypeBitHasRc 0x8000u
#define CETypeBitRcAtomic 0x4000u
#define CETypeBitWeakable 0x2000u

#define CETypeMaskRcAtomic (CETypeBitHasRc | CETypeBitRcAtomic)
#define CETypeMaskRcUnsafe (CETypeBitHasRc)
#define CETypeMaskNoRc 0x0u

#define CETypeMaskDefaultRc CETypeMaskRcAtomic


#define CETypeMaskVersionBitCount 16

#if CEBuild64Bit
#define CETypeBaseLayoutSize 64
#else
#define CETypeBaseLayoutSize 36
#endif

#if CEBuild64Bit

#define CERuntimeRcStatic 0x7FFFFFFFFFFFFFFEull
#define CERuntimeRcDeallocing 0x7FFFFFFFFFFFFFFFull //不允许retain， 清空弱引用

#define CERuntimeRcWeakBit 0x8000000000000000ull
#define CERuntimeRcMask 0x7FFFFFFFFFFFFFFFull

#else

#define CERuntimeRcStatic 0xFFFFFFFEul
#define CERuntimeRcDeallocing 0x7FFFFFFFul

#define CERuntimeRcWeakBit 0x80000000ul
#define CERuntimeRcMask 0x7FFFFFFFul

#endif

void CETypeDefaultDescript(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript);

//do nothing
void CETypeDefaultDeinit(CERef _Nonnull object);


#if CEBuild64Bit
#define CERcType_t uint64_t
#else
#define CERcType_t uint32_t
#endif


#pragma pack(push)
#pragma pack(1)

struct __CERuntimeBase {
    CETypeRef const _Nonnull type;
};

struct __CERuntimeAtomicRcBase {
    CETypeRef const _Nonnull type;
#if CEBuild64Bit
    _Atomic(uint_fast64_t) rcInfo;
#else
    _Atomic(uint_fast32_t) rcInfo;
#endif
};
struct __CERuntimeUnsafeRcBase {
    CETypeRef const _Nonnull type;
#if CEBuild64Bit
    uint64_t rcInfo;
#else
    uint32_t rcInfo;
#endif
};

struct __CEType {
    CETypeRef const _Nonnull type;
    uint16_t version;
    uint16_t masks;
    uint32_t objectSize;//objectSize 为0 时 size 为变长
    char * _Nonnull name;
    uintptr_t identifier;
    CEAlloctor_s const * _Nullable alloctor;//CMAlloctor_s * description
    
    size_t (* _Nonnull getSize)(CERef _Nonnull object);
    void (* _Nonnull deinit)(CERef _Nonnull object);
    void (* _Nonnull descript)(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript/*会被调用多次*/);
};

struct __CEAlloctor {
    void * _Nullable context;
    void * _Nonnull (* _Nonnull allocate)(CETypeRef _Nonnull type, size_t size);
    void (* _Nonnull deallocate)(CETypeRef _Nonnull type, void * _Nonnull ptr, size_t size);
    void (* _Nonnull destroyWeakRefrences)(CERef _Nonnull object);
    CERuntimeRetain_f _Nonnull retain;
    CERuntimeTryRetain_f _Nonnull tryRetain;
    CERuntimeRelease_f _Nonnull release;
};
#pragma pack(pop)

extern CEType_s __CETypeMate;
#define CETypeMate &__CETypeMate

extern const CEAlloctor_s __CETypeDefaultAlloctor;


CERef _Nonnull CERetain(CERef _Nonnull object);
CERef _Nullable CETryRetain(CERef _Nonnull object);
void CERelease(CERef _Nonnull object);


_Bool CETypeIsEqual(CETypeRef _Nonnull type0, CETypeRef _Nonnull type1);
CETypeRef _Nonnull CERefGetType(CERef _Nonnull ref);


#endif /* CERuntime_h */
