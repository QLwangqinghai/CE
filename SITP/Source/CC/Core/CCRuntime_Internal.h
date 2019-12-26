//
//  CCRuntime_Internal.h
//  SITP
//
//  Created by vector on 2019/12/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCRuntime_Internal_h
#define CCRuntime_Internal_h

#include "CCBase.h"
#include "CCRuntime.h"

typedef CCInt CCTypeKindCode;

extern const CCTypeKindCode CCTypeKindCodeConstantValue;
/*
 ref
 type
 type'type
 type'type'type
 
 
 
 
 type'type'type'type
 */


struct __CCType;
typedef struct __CCType CCType;

typedef struct {
    void * _Nonnull type;
    const char * _Nonnull name; // must be a pure ASCII string, nul-terminated
    uint32_t kind: 8;
    uint32_t isType: 1;

} CCTypeBase_t;

typedef struct {
    CCRefFunc_f _Nonnull init;
    CCRefFunc_f _Nonnull finalize;
    CCRefHashFunc_f _Nonnull hash;
    CCRefIsEqualFunc_f _Nonnull equal;
    CCRefCountFunc_f _Nonnull refcount;
//    CFStringRef (* _Nonnull descript)(CFTypeRef cf);    // return str with retain
} CCTypeObjectFunc_t;

typedef CCTypeObjectFunc_t * CCTypeObjectFuncPtr;

struct __CCRefType {
    void * _Nonnull type;
    CCTypeObjectFuncPtr _Nonnull funcPtr;
#if CCBuild64Bit
    uintptr_t _xxxx: 25;
    uintptr_t headSize: 6;
    uintptr_t size: 33;
#else
    uintptr_t _xxxx: 10;
    uintptr_t headSize: 5;
    uintptr_t size: 17;
#endif
};

struct __CCType {
    void * _Nonnull type;
    CCTypeObjectFuncPtr _Nonnull funcPtr;
#if CCBuild64Bit
    uintptr_t _xxxx: 25;
    uintptr_t headSize: 6;
    uintptr_t size: 33;
#else
    uintptr_t _xxxx: 10;
    uintptr_t headSize: 5;
    uintptr_t size: 17;
#endif
    
//    CFStringRef (* _Nonnull descript)(CFTypeRef cf, CFDictionaryRef formatOptions);    // return str with retain
//    CFStringRef (* _Nonnull copyDebugDesc)(CFTypeRef cf);    // return str with retain

    

};

extern CCType const CCMetaType;

extern CCType const CCTypeRoot;


#endif /* CCRuntime_Internal_h */
