//
//  CCType.c
//  SITP
//
//  Created by vector on 2019/12/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCType.h"

const CCIndex CCIndexNotFound = CCConstantIndexNotFound;
const CCIndex CCIndex32NotFound = CCConstantIndex32NotFound;
const CCIndex CCIndex64NotFound = CCConstantIndex64NotFound;


//typedef struct __CFRuntimeClass {
//    CFIndex version;
//    const char *className; // must be a pure ASCII string, nul-terminated
//    void (*init)(CFTypeRef cf);
//    CFTypeRef (*copy)(CFAllocatorRef allocator, CFTypeRef cf);
//    void (*finalize)(CFTypeRef cf);
//    Boolean (*equal)(CFTypeRef cf1, CFTypeRef cf2);
//    CFHashCode (*hash)(CFTypeRef cf);
//    CFStringRef (*copyFormattingDesc)(CFTypeRef cf, CFDictionaryRef formatOptions);    // return str with retain
//    CFStringRef (*copyDebugDesc)(CFTypeRef cf);    // return str with retain
//
//#define CF_RECLAIM_AVAILABLE 1
//    void (*reclaim)(CFTypeRef cf); // Or in _kCFRuntimeResourcefulObject in the .version to indicate this field should be used
//
//#define CF_REFCOUNT_AVAILABLE 1
//    uint32_t (*refcount)(intptr_t op, CFTypeRef cf); // Or in _kCFRuntimeCustomRefCount in the .version to indicate this field should be used
//        // this field must be non-NULL when _kCFRuntimeCustomRefCount is in the .version field
//        // - if the callback is passed 1 in 'op' it should increment the 'cf's reference count and return 0
//        // - if the callback is passed 0 in 'op' it should return the 'cf's reference count, up to 32 bits
//        // - if the callback is passed -1 in 'op' it should decrement the 'cf's reference count; if it is now zero, 'cf' should be cleaned up and deallocated; then return 0
//        // remember to use saturation arithmetic logic and stop incrementing and decrementing when the ref count hits UINT32_MAX, or you will have a security bug
//        // remember that reference count incrementing/decrementing must be done thread-safely/atomically
//        // objects should be created/initialized with a custom ref-count of 1 by the class creation functions
//        // do not attempt to use any bits within the CFRuntimeBase for your reference count; store that in some additional field in your CF object
//
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
//#define CF_REQUIRED_ALIGNMENT_AVAILABLE 1
//    uintptr_t requiredAlignment; // Or in _kCFRuntimeRequiresAlignment in the .version field to indicate this field should be used; the allocator to _CFRuntimeCreateInstance() will be ignored in this case; if this is less than the minimum alignment the system supports, you'll get higher alignment; if this is not an alignment the system supports (e.g., most systems will only support powers of two, or if it is too high), the result (consequences) will be up to CF or the system to decide
//
//} CFRuntimeClass;
