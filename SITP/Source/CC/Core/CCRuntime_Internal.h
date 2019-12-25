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

struct __CCType;
typedef struct __CCType CCType;

typedef struct {
    const char * _Nonnull name; // must be a pure ASCII string, nul-terminated
    CCRefFunc_f _Nonnull init;
    CCRefHashFunc_f _Nonnull hash;
    CCRefIsEqualFunc_f _Nonnull equal;
    CCRefCountFunc_f _Nonnull refcount;
//    CFStringRef (* _Nonnull descript)(CFTypeRef cf);    // return str with retain
    
    
    
    
} CCTypeFunc_t;

typedef CCTypeFunc_t * CCTypeFuncPtr;

struct __CCType {
    void * _Nonnull type;
    CCRefFunc_f _Nonnull finalize;
    CCTypeFuncPtr _Nonnull funcPtr;
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



#endif /* CCRuntime_Internal_h */
