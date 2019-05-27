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
typedef struct __CERuntimeBase CERuntimeBase_s;

struct __CERuntimeAtomicRcBase;
typedef struct __CERuntimeAtomicRcBase CERuntimeAtomicRcBase_s;

struct __CERuntimeUnsafeRcBase;
typedef struct __CERuntimeUnsafeRcBase CERuntimeUnsafeRcBase_s;

struct __CEDefaultType;
typedef struct __CEDefaultType CEDefaultType_s;

struct __CERcableType;
typedef struct __CERcableType CERcableType_s;

typedef void * CETypeRef;

struct __CEDefaultTypeSpecific;
typedef struct __CEDefaultTypeSpecific CETypeSpecific_s;
typedef CETypeSpecific_s const * CETypeSpecificPtr;

struct __CEAlloctor;
typedef struct __CEAlloctor CEAlloctor_s;
typedef CEAlloctor_s const * CEAlloctorPtr;

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
#define CEDefaultTypeLayoutSize 72
#else
#define CEDefaultTypeLayoutSize 40
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

typedef struct __CETypeContent {
    uint16_t version;
    uint16_t masks;
    uint32_t objectSize;//objectSize 不能为0
    uintptr_t identifier;
    
    CEAlloctorPtr _Nonnull alloctor;
    CETypeSpecificPtr _Nonnull specific;
    CERef _Nullable class;
} CETypeContent_s;
typedef CETypeContent_s * CETypeContentPtr;


struct __CEDefaultType {
    CERuntimeBase_s runtime;
    CETypeContent_s content;
};

struct __CERcableType {
    CERuntimeAtomicRcBase_s runtime;
    CETypeContent_s content;
};

struct __CEAlloctor {
    void * _Nullable context;
    void * _Nonnull (* _Nonnull allocate)(CETypeRef _Nonnull type, size_t size);
    void (* _Nonnull deallocate)(CETypeRef _Nonnull type, void * _Nonnull ptr);
    void (* _Nonnull destroyWeakRefrences)(CERef _Nonnull object);
    
    CERuntimeRetain_f _Nonnull retain;
    CERuntimeTryRetain_f _Nonnull tryRetain;
    CERuntimeRelease_f _Nonnull release;
};

struct __CEDefaultTypeSpecific {
    char * _Nonnull name;
    void (* _Nonnull descript)(CERef _Nonnull object, void const * _Nonnull handler, CEDescript_f _Nonnull descript/*会被调用多次*/);
    void (* _Nonnull deinit)(CERef _Nonnull object);
};

#pragma pack(pop)

extern CEDefaultType_s __CEDefaultTypeMate;
#define CETypeMate &__CEDefaultTypeMate

extern const CEAlloctor_s CERuntimeDefaultAlloctor;


#define CEDefaultType(identifier, specific) {\
    .runtime = {.type = CETypeMate},\
    .content = {\
        .version = CERuntimeVersion,\
        .masks = CETypeMaskNoRc,\
        .objectSize = CEDefaultTypeLayoutSize,\
        .identifier = identifier,\
        .alloctor = &CERuntimeDefaultAlloctor,\
        .specific = specific,\
        .class = NULL,\
    },\
}



CERef _Nonnull CERetain(const CERef _Nonnull object);
CERef _Nullable CETryRetain(const CERef _Nonnull object);
void CERelease(const CERef _Nonnull object);


_Bool CETypeIsEqual(CETypeRef _Nonnull type0, CETypeRef _Nonnull type1);
CETypeRef _Nonnull CERefGetType(CERef _Nonnull ref);

CETypeContent_s * _Nonnull CERefGetTypeContent(CERef _Nonnull ref);



void * _Nonnull CERuntimeDefaultAllocate(CETypeRef _Nonnull type, size_t size);

void CERuntimeDefaultDeallocate(CETypeRef _Nonnull type, void * _Nonnull ptr);

void CERuntimeDefaultDestroyWeakRefrences(CERef _Nonnull object);

static inline CERef _Nullable ___CERuntimeRetain(CERef _Nonnull object, _Bool tryR);

CERef _Nonnull CERuntimeDefaultRetain(CERef _Nonnull object);
CERef _Nullable CERuntimeDefaultTryRetain(CERef _Nonnull object);

void CERuntimeDefaultRelease(CERef _Nonnull object);



#endif /* CERuntime_h */
