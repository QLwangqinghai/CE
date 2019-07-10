//
//  CCArrayInternal.h
//  CCollection
//
//  Created by vector on 2019/6/26.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCArrayInternal_h
#define CCArrayInternal_h

#include "CCArray.h"
#include "CInteger.h"
#include "CType.h"

#define CF_ARRAY_ALWAYS_BRIDGE 0


struct __CCArrayBucket {
    const void *_item;
};


static inline uint32_t __CCRingBufferRoundUpCapacity(uint32_t capacity) {
    if (capacity < 4) return 4;
    uint64_t c = capacity;
    uint64_t s = 1ull << (CUInt64MostSignificant(c) + 1);
    if (s > 0x40000000L) {
        s = 0x40000000L;
    }
    return (uint32_t)s;
}

struct __CCArrayDeque {
    uintptr_t _leftIdx;
    uintptr_t _capacity;
    /* struct __CCArrayBucket buckets follow here */
};

typedef struct __CCArray {
    //    CFRuntimeBase _base;
    uint32_t _count;        /* number of objects */
    uint32_t _mutations;
    int32_t _mutInProgress;
    uint32_t _elementSize;
    uint32_t _elementType;
    void * _store;           /* can be NULL when MutableDeque */
} CCArray_s;
typedef CCArray_s * _Nonnull CCArrayNonnullPtr;

/* Flag bits */
enum {        /* Bits 0-1 */
    __kCFArrayImmutable = 0,
    __kCFArrayDeque = 2,
};

enum {        /* Bits 2-3 */
    __kCFArrayHasNullCallBacks = 0,
    __kCFArrayHasCFTypeCallBacks = 1,
    __kCFArrayHasCustomCallBacks = 3    /* callbacks are at end of header */
};

static inline uint32_t __CCArrayGetCount(CCArrayNonnullPtr _Nonnull array) {
    return array->_count;
}

static inline void __CCArraySetCount(CCArrayNonnullPtr array, uint32_t v) {
    array->_count = v;
}

/* Only applies to immutable and mutable-deque-using arrays;
 * Returns the bucket holding the left-most real value in the latter case. */
CF_INLINE struct __CCArrayBucket *__CCArrayGetBucketsPtr(CCArrayNonnullPtr array) {
    switch (__CCArrayGetType(array)) {
        case __kCFArrayImmutable:
            // TODO: Refactor the following to just get the custom callbacks value directly, or refactor helper function
            return (struct __CCArrayBucket *)((uint8_t *)array + __CCArrayGetSizeOfType(__CFRuntimeGetValue(array, 6, 0)));
        case __kCFArrayDeque: {
            struct __CCArrayDeque *deque = (struct __CCArrayDeque *)array->_store;
            return (struct __CCArrayBucket *)((uint8_t *)deque + sizeof(struct __CCArrayDeque) + deque->_leftIdx * sizeof(struct __CCArrayBucket));
        }
    }
    return NULL;
}

/* This shouldn't be called if the array count is 0. */
CF_INLINE struct __CCArrayBucket *__CCArrayGetBucketAtIndex(CCArrayNonnullPtr array, uint32_t idx) {
    switch (__CCArrayGetType(array)) {
        case __kCFArrayImmutable:
        case __kCFArrayDeque:
            return __CCArrayGetBucketsPtr(array) + idx;
    }
    return NULL;
}

CF_PRIVATE CFArrayCallBacks *__CCArrayGetCallBacks(CCArrayNonnullPtr array) {
    CFArrayCallBacks *result = NULL;
    switch (__CFRuntimeGetValue(array, 3, 2)) {
        case __kCFArrayHasNullCallBacks:
            return (CFArrayCallBacks *)&__kCFNullArrayCallBacks;
        case __kCFArrayHasCFTypeCallBacks:
            return (CFArrayCallBacks *)&kCFTypeArrayCallBacks;
        case __kCFArrayHasCustomCallBacks:
            break;
    }
    switch (__CCArrayGetType(array)) {
        case __kCFArrayImmutable:
            result = (CFArrayCallBacks *)((uint8_t *)array + sizeof(struct __CCArray));
            break;
        case __kCFArrayDeque:
            result = (CFArrayCallBacks *)((uint8_t *)array + sizeof(struct __CCArray));
            break;
    }
    return result;
}

CF_INLINE bool __CCArrayCallBacksMatchNull(const CFArrayCallBacks *c) {
    return (NULL == c ||
            (c->retain == __kCFNullArrayCallBacks.retain &&
             c->release == __kCFNullArrayCallBacks.release &&
             c->copyDescription == __kCFNullArrayCallBacks.copyDescription &&
             c->equal == __kCFNullArrayCallBacks.equal));
}

CF_INLINE bool __CCArrayCallBacksMatchCFType(const CFArrayCallBacks *c) {
    return (&kCFTypeArrayCallBacks == c ||
            (c->retain == kCFTypeArrayCallBacks.retain &&
             c->release == kCFTypeArrayCallBacks.release &&
             c->copyDescription == kCFTypeArrayCallBacks.copyDescription &&
             c->equal == kCFTypeArrayCallBacks.equal));
}


#define CHECK_FOR_MUTATION(A) do { } while (0)
#define BEGIN_MUTATION(A) do { } while (0)
#define END_MUTATION(A) do { } while (0)

struct _releaseContext {
    void (*release)(CFAllocatorRef, const void *);
    CFAllocatorRef allocator;
};

static void __CCArrayReleaseValues(CCArrayNonnullPtr array, CFRange range, bool releaseStorageIfPossible) {
    const CFArrayCallBacks *cb = __CCArrayGetCallBacks(array);
    CFAllocatorRef allocator;
    uint32_t idx;
    switch (__CCArrayGetType(array)) {
        case __kCFArrayImmutable:
            if (NULL != cb->release && 0 < range.length) {
                struct __CCArrayBucket *buckets = __CCArrayGetBucketsPtr(array);
                allocator = __CFGetAllocator(array);
                for (idx = 0; idx < range.length; idx++) {
                    INVOKE_CALLBACK2(cb->release, allocator, buckets[idx + range.location]._item);
                }
                memset(buckets + range.location, 0, sizeof(struct __CCArrayBucket) * range.length);
            }
            break;
        case __kCFArrayDeque: {
            struct __CCArrayDeque *deque = (struct __CCArrayDeque *)array->_store;
            if (0 < range.length && NULL != deque) {
                struct __CCArrayBucket *buckets = __CCArrayGetBucketsPtr(array);
                if (NULL != cb->release) {
                    allocator = __CFGetAllocator(array);
                    for (idx = 0; idx < range.length; idx++) {
                        INVOKE_CALLBACK2(cb->release, allocator, buckets[idx + range.location]._item);
                    }
                }
                memset(buckets + range.location, 0, sizeof(struct __CCArrayBucket) * range.length);
            }
            if (releaseStorageIfPossible && 0 == range.location && __CCArrayGetCount(array) == range.length) {
                allocator = __CFGetAllocator(array);
                if (NULL != deque) CFAllocatorDeallocate(allocator, deque);
                __CCArraySetCount(array, 0);
                ((struct __CCArray *)array)->_store = NULL;
            }
            break;
        }
    }
}

#if defined(DEBUG)
CF_INLINE void __CCArrayValidateRange(CCArrayNonnullPtr array, CFRange range, const char *func) {
    CFAssert3(0 <= range.location && range.location <= CFArrayGetCount(array), __kCFLogAssertion, "%s(): range.location index (%ld) out of bounds (0, %ld)", func, range.location, CFArrayGetCount(array));
    CFAssert2(0 <= range.length, __kCFLogAssertion, "%s(): range.length (%ld) cannot be less than zero", func, range.length);
    CFAssert3(range.location + range.length <= CFArrayGetCount(array), __kCFLogAssertion, "%s(): ending index (%ld) out of bounds (0, %ld)", func, range.location + range.length, CFArrayGetCount(array));
}
#else
#define __CCArrayValidateRange(a,r,f)
#endif

static Boolean __CCArrayEqual(CFTypeRef cf1, CFTypeRef cf2) {
    CCArrayNonnullPtr array1 = (CCArrayNonnullPtr)cf1;
    CCArrayNonnullPtr array2 = (CCArrayNonnullPtr)cf2;
    const CFArrayCallBacks *cb1, *cb2;
    uint32_t idx, cnt;
    if (array1 == array2) return true;
    cnt = __CCArrayGetCount(array1);
    if (cnt != __CCArrayGetCount(array2)) return false;
    cb1 = __CCArrayGetCallBacks(array1);
    cb2 = __CCArrayGetCallBacks(array2);
    if (cb1->equal != cb2->equal) return false;
    if (0 == cnt) return true;    /* after function comparison! */
    for (idx = 0; idx < cnt; idx++) {
        const void *val1 = __CCArrayGetBucketAtIndex(array1, idx)->_item;
        const void *val2 = __CCArrayGetBucketAtIndex(array2, idx)->_item;
        if (val1 != val2) {
            if (NULL == cb1->equal) return false;
            if (!INVOKE_CALLBACK2(cb1->equal, val1, val2)) return false;
        }
    }
    return true;
}

static CFHashCode __CCArrayHash(CFTypeRef cf) {
    CCArrayNonnullPtr array = (CCArrayNonnullPtr)cf;
    return __CCArrayGetCount(array);
}

static CFStringRef __CCArrayCopyDescription(CFTypeRef cf) {
    CCArrayNonnullPtr array = (CCArrayNonnullPtr)cf;
    CFMutableStringRef result;
    const CFArrayCallBacks *cb;
    CFAllocatorRef allocator;
    uint32_t idx, cnt;
    cnt = __CCArrayGetCount(array);
    allocator = __CFGetAllocator(array);
    result = CFStringCreateMutable(allocator, 0);
    switch (__CCArrayGetType(array)) {
        case __kCFArrayImmutable:
            CFStringAppendFormat(result, NULL, CFSTR("<CFArray %p [%p]>{type = immutable, count = %lu, values = (%s"), cf, allocator, (unsigned long)cnt, cnt ? "\n" : "");
            break;
        case __kCFArrayDeque:
            CFStringAppendFormat(result, NULL, CFSTR("<CFArray %p [%p]>{type = mutable-small, count = %lu, values = (%s"), cf, allocator, (unsigned long)cnt, cnt ? "\n" : "");
            break;
    }
    cb = __CCArrayGetCallBacks(array);
    for (idx = 0; idx < cnt; idx++) {
        CFStringRef desc = NULL;
        const void *val = __CCArrayGetBucketAtIndex(array, idx)->_item;
        if (NULL != cb->copyDescription) {
            desc = (CFStringRef)INVOKE_CALLBACK1(cb->copyDescription, val);
        }
        if (NULL != desc) {
            CFStringAppendFormat(result, NULL, CFSTR("\t%lu : %@\n"), (unsigned long)idx, desc);
            CFRelease(desc);
        } else {
            CFStringAppendFormat(result, NULL, CFSTR("\t%lu : <%p>\n"), (unsigned long)idx, val);
        }
    }
    CFStringAppend(result, CFSTR(")}"));
    return result;
}


static void __CCArrayDeallocate(CFTypeRef cf) {
    CCArrayNonnullPtr array = (CCArrayNonnullPtr)cf;
    BEGIN_MUTATION(array);
    __CCArrayReleaseValues(array, CFRangeMake(0, __CCArrayGetCount(array)), true);
    END_MUTATION(array);
}

const CFRuntimeClass __CCArrayClass = {
    _kCFRuntimeScannedObject,
    "CFArray",
    NULL,    // init
    NULL,    // copy
    __CCArrayDeallocate,
    __CCArrayEqual,
    __CCArrayHash,
    NULL,    //
    __CCArrayCopyDescription
};

CFTypeID CFArrayGetTypeID(void) {
    return _kCFRuntimeIDCFArray;
}

static CCArrayNonnullPtr __CCArrayInit(CFAllocatorRef allocator, UInt32 flags, uint32_t capacity, const CFArrayCallBacks *callBacks) {
    struct __CCArray *memory;
    UInt32 size;
    __CFBitfieldSetValue(flags, 31, 2, 0);
    if (__CCArrayCallBacksMatchNull(callBacks)) {
        __CFBitfieldSetValue(flags, 3, 2, __kCFArrayHasNullCallBacks);
    } else if (__CCArrayCallBacksMatchCFType(callBacks)) {
        __CFBitfieldSetValue(flags, 3, 2, __kCFArrayHasCFTypeCallBacks);
    } else {
        __CFBitfieldSetValue(flags, 3, 2, __kCFArrayHasCustomCallBacks);
    }
    size = __CCArrayGetSizeOfType(flags) - sizeof(CFRuntimeBase);
    switch (__CFBitfieldGetValue(flags, 1, 0)) {
        case __kCFArrayImmutable:
            size += capacity * sizeof(struct __CCArrayBucket);
            break;
        case __kCFArrayDeque:
            break;
    }
    memory = (struct __CCArray*)_CFRuntimeCreateInstance(allocator, CFArrayGetTypeID(), size, NULL);
    if (NULL == memory) {
        return NULL;
    }
    __CFRuntimeSetValue(memory, 6, 0, flags);
    __CCArraySetCount((CCArrayNonnullPtr)memory, 0);
    switch (__CFBitfieldGetValue(flags, 1, 0)) {
        case __kCFArrayImmutable:
            if (__CFOASafe) __CFSetLastAllocationEventName(memory, "CFArray (immutable)");
            break;
        case __kCFArrayDeque:
            if (__CFOASafe) __CFSetLastAllocationEventName(memory, "CFArray (mutable-variable)");
            ((struct __CCArray *)memory)->_mutations = 1;
            ((struct __CCArray *)memory)->_mutInProgress = 0;
            ((struct __CCArray*)memory)->_store = NULL;
            break;
    }
    if (__kCFArrayHasCustomCallBacks == __CFBitfieldGetValue(flags, 3, 2)) {
        CFArrayCallBacks *cb = (CFArrayCallBacks *)__CCArrayGetCallBacks((CCArrayNonnullPtr)memory);
        *cb = *callBacks;
        FAULT_CALLBACK((void **)&(cb->retain));
        FAULT_CALLBACK((void **)&(cb->release));
        FAULT_CALLBACK((void **)&(cb->copyDescription));
        FAULT_CALLBACK((void **)&(cb->equal));
    }
    return (CCArrayNonnullPtr)memory;
}

CF_PRIVATE CCArrayNonnullPtr __CCArrayCreateTransfer(CFAllocatorRef allocator, const void **values, uint32_t numValues) {
    CFAssert2(0 <= numValues, __kCFLogAssertion, "%s(): numValues (%ld) cannot be less than zero", __PRETTY_FUNCTION__, numValues);
    UInt32 flags = __kCFArrayImmutable;
    __CFBitfieldSetValue(flags, 31, 2, 0);
    __CFBitfieldSetValue(flags, 3, 2, __kCFArrayHasCFTypeCallBacks);
    UInt32 size = __CCArrayGetSizeOfType(flags) - sizeof(CFRuntimeBase);
    size += numValues * sizeof(struct __CCArrayBucket);
    struct __CCArray *memory = (struct __CCArray*)_CFRuntimeCreateInstance(allocator, CFArrayGetTypeID(), size, NULL);
    if (NULL == memory) {
        return NULL;
    }
    __CFRuntimeSetValue(memory, 6, 0, flags);
    __CCArraySetCount(memory, numValues);
    memmove(__CCArrayGetBucketsPtr(memory), values, sizeof(void *) * numValues);
    if (__CFOASafe) __CFSetLastAllocationEventName(memory, "CFArray (immutable)");
    return (CCArrayNonnullPtr)memory;
}

CF_PRIVATE CCArrayNonnullPtr __CCArrayCreate0(CFAllocatorRef allocator, const void **values, uint32_t numValues, const CFArrayCallBacks *callBacks) {
    CCArrayNonnullPtr result;
    const CFArrayCallBacks *cb;
    struct __CCArrayBucket *buckets;
    uint32_t idx;
    CFAssert2(0 <= numValues, __kCFLogAssertion, "%s(): numValues (%ld) cannot be less than zero", __PRETTY_FUNCTION__, numValues);
    result = __CCArrayInit(allocator, __kCFArrayImmutable, numValues, callBacks);
    cb = __CCArrayGetCallBacks(result);
    buckets = __CCArrayGetBucketsPtr(result);
    if (NULL != cb->retain) {
        for (idx = 0; idx < numValues; idx++) {
            *((void **)&buckets->_item) = (void *)INVOKE_CALLBACK2(cb->retain, allocator, *values);
            values++;
            buckets++;
        }
    }
    else {
        for (idx = 0; idx < numValues; idx++) {
            *((void **)&buckets->_item) = (void *)*values;
            values++;
            buckets++;
        }
    }
    __CCArraySetCount(result, numValues);
    return result;
}

CF_PRIVATE CFMutableArrayRef __CCArrayCreateMutable0(CFAllocatorRef allocator, uint32_t capacity, const CFArrayCallBacks *callBacks) {
    CFAssert2(0 <= capacity, __kCFLogAssertion, "%s(): capacity (%ld) cannot be less than zero", __PRETTY_FUNCTION__, capacity);
    CFAssert2(capacity <= LONG_MAX / sizeof(void *), __kCFLogAssertion, "%s(): capacity (%ld) is too large for this architecture", __PRETTY_FUNCTION__, capacity);
    return (CFMutableArrayRef)__CCArrayInit(allocator, __kCFArrayDeque, capacity, callBacks);
}

CF_PRIVATE CCArrayNonnullPtr __CCArrayCreateCopy0(CFAllocatorRef allocator, CCArrayNonnullPtr array) {
    CCArrayNonnullPtr result;
    const CFArrayCallBacks *cb;
    struct __CCArrayBucket *buckets;
    uint32_t numValues = CFArrayGetCount(array);
    uint32_t idx;
    if (CF_IS_OBJC(CFArrayGetTypeID(), array) || CF_IS_SWIFT(CFArrayGetTypeID(), array)) {
        cb = &kCFTypeArrayCallBacks;
    } else {
        cb = __CCArrayGetCallBacks(array);
    }
    result = __CCArrayInit(allocator, __kCFArrayImmutable, numValues, cb);
    cb = __CCArrayGetCallBacks(result);
    buckets = __CCArrayGetBucketsPtr(result);
    for (idx = 0; idx < numValues; idx++) {
        const void *value = CFArrayGetValueAtIndex(array, idx);
        if (NULL != cb->retain) {
            value = (void *)INVOKE_CALLBACK2(cb->retain, allocator, value);
        }
        buckets->_item = value;
        buckets++;
    }
    __CCArraySetCount(result, numValues);
    return result;
}

CF_PRIVATE CFMutableArrayRef __CCArrayCreateMutableCopy0(CFAllocatorRef allocator, uint32_t capacity, CCArrayNonnullPtr array) {
    CFMutableArrayRef result;
    const CFArrayCallBacks *cb;
    uint32_t idx, numValues = CFArrayGetCount(array);
    UInt32 flags;
    if (CF_IS_OBJC(CFArrayGetTypeID(), array) || CF_IS_SWIFT(CFArrayGetTypeID(), array)) {
        cb = &kCFTypeArrayCallBacks;
    }
    else {
        cb = __CCArrayGetCallBacks(array);
    }
    flags = __kCFArrayDeque;
    result = (CFMutableArrayRef)__CCArrayInit(allocator, flags, capacity, cb);
    if (0 == capacity) _CFArraySetCapacity(result, numValues);
    for (idx = 0; idx < numValues; idx++) {
        const void *value = CFArrayGetValueAtIndex(array, idx);
        CFArrayAppendValue(result, value);
    }
    return result;
}

#define DEFINE_CREATION_METHODS 1

#if DEFINE_CREATION_METHODS

CCArrayNonnullPtr CFArrayCreate(CFAllocatorRef allocator, const void **values, uint32_t numValues, const CFArrayCallBacks *callBacks) {
    return __CCArrayCreate0(allocator, values, numValues, callBacks);
}

CFMutableArrayRef CFArrayCreateMutable(CFAllocatorRef allocator, uint32_t capacity, const CFArrayCallBacks *callBacks) {
    return __CCArrayCreateMutable0(allocator, capacity, callBacks);
}

CCArrayNonnullPtr CFArrayCreateCopy(CFAllocatorRef allocator, CCArrayNonnullPtr array) {
    return __CCArrayCreateCopy0(allocator, array);
}

CFMutableArrayRef CFArrayCreateMutableCopy(CFAllocatorRef allocator, uint32_t capacity, CCArrayNonnullPtr array) {
    return __CCArrayCreateMutableCopy0(allocator, capacity, array);
}

#endif

CF_PRIVATE uint32_t _CFNonObjCArrayGetCount(CCArrayNonnullPtr array) {
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CHECK_FOR_MUTATION(array);
    return __CCArrayGetCount(array);
}

uint32_t CFArrayGetCount(CCArrayNonnullPtr array) {
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), uint32_t, (CFSwiftRef)array, NSArray.count);
    CF_OBJC_FUNCDISPATCHV(CFArrayGetTypeID(), uint32_t, (NSArray *)array, count);
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CHECK_FOR_MUTATION(array);
    return __CCArrayGetCount(array);
}

uint32_t CFArrayGetCountOfValue(CCArrayNonnullPtr array, CFRange range, const void *value) {
    uint32_t idx, count = 0;
    __CFGenericValidateType(array, CFArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    const CFArrayCallBacks *cb = (CF_IS_OBJC(CFArrayGetTypeID(), array) || CF_IS_SWIFT(CFArrayGetTypeID(), array)) ? &kCFTypeArrayCallBacks : __CCArrayGetCallBacks(array);
    for (idx = 0; idx < range.length; idx++) {
        const void *item = CFArrayGetValueAtIndex(array, range.location + idx);
        if (value == item || (cb->equal && INVOKE_CALLBACK2(cb->equal, value, item))) {
            count++;
        }
    }
    return count;
}

Boolean CFArrayContainsValue(CCArrayNonnullPtr array, CFRange range, const void *value) {
    uint32_t idx;
    __CFGenericValidateType(array, CFArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    const CFArrayCallBacks *cb = (CF_IS_OBJC(CFArrayGetTypeID(), array) || CF_IS_SWIFT(CFArrayGetTypeID(), array)) ? &kCFTypeArrayCallBacks : __CCArrayGetCallBacks(array);
    for (idx = 0; idx < range.length; idx++) {
        const void *item = CFArrayGetValueAtIndex(array, range.location + idx);
        if (value == item || (cb->equal && INVOKE_CALLBACK2(cb->equal, value, item))) {
            return true;
        }
    }
    return false;
}

const void *CFArrayGetValueAtIndex(CCArrayNonnullPtr array, uint32_t idx) {
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), const void *, (CFSwiftRef)array, NSArray.objectAtIndex, idx);
    
    
#if !CF_ARRAY_ALWAYS_BRIDGE
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CFAssert2(0 <= idx && idx < __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index (%ld) out of bounds", __PRETTY_FUNCTION__, idx);
    Boolean outOfBounds = false;
    const void *result = _CFArrayCheckAndGetValueAtIndex(array, idx, &outOfBounds);
    if (outOfBounds) HALT;
    return result;
#endif
}

// This is for use by NSCFArray; it avoids ObjC dispatch, and checks for out of bounds
const void *_CFArrayCheckAndGetValueAtIndex(CCArrayNonnullPtr array, uint32_t idx, Boolean *outOfBounds) {
    CHECK_FOR_MUTATION(array);
    if (0 <= idx && idx < __CCArrayGetCount(array)) return __CCArrayGetBucketAtIndex(array, idx)->_item;
    if (outOfBounds) *outOfBounds = true;
    return (void *)(-1);
}


void CFArrayGetValues(CCArrayNonnullPtr array, CFRange range, const void **values) {
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), void, (CFSwiftRef)array, NSArray.getObjects, range, values);
    CF_OBJC_FUNCDISPATCHV(CFArrayGetTypeID(), void, (NSArray *)array, getObjects:(id *)values range:NSMakeRange(range.location, range.length));
    __CFGenericValidateType(array, CFArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(NULL != values, __kCFLogAssertion, "%s(): pointer to values may not be NULL", __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    if (0 < range.length) {
        switch (__CCArrayGetType(array)) {
            case __kCFArrayImmutable:
            case __kCFArrayDeque:
                memmove(values, __CCArrayGetBucketsPtr(array) + range.location, range.length * sizeof(struct __CCArrayBucket));
                break;
        }
    }
}

CF_EXPORT unsigned long _CFArrayFastEnumeration(CCArrayNonnullPtr array, struct __objcFastEnumerationStateEquivalent *state, void *stackbuffer, unsigned long count) {
    CHECK_FOR_MUTATION(array);
    if (array->_count == 0) return 0;
    enum { ATSTART = 0, ATEND = 1 };
    switch (__CCArrayGetType(array)) {
        case __kCFArrayImmutable:
            if (state->state == ATSTART) { /* first time */
                static const unsigned long const_mu = 1;
                state->state = ATEND;
                state->mutationsPtr = (unsigned long *)&const_mu;
                state->itemsPtr = (unsigned long *)__CCArrayGetBucketsPtr(array);
                return array->_count;
            }
            return 0;
        case __kCFArrayDeque:
            if (state->state == ATSTART) { /* first time */
                state->state = ATEND;
                state->mutationsPtr = (unsigned long *)&array->_mutations;
                state->itemsPtr = (unsigned long *)__CCArrayGetBucketsPtr(array);
                return array->_count;
            }
            return 0;
    }
    return 0;
}


void CFArrayApplyFunction(CCArrayNonnullPtr array, CFRange range, CFArrayApplierFunction applier, void *context) {
    uint32_t idx;
    FAULT_CALLBACK((void **)&(applier));
    __CFGenericValidateType(array, CFArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(NULL != applier, __kCFLogAssertion, "%s(): pointer to applier function may not be NULL", __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    for (idx = 0; idx < range.length; idx++) {
        const void *item = CFArrayGetValueAtIndex(array, range.location + idx);
        INVOKE_CALLBACK2(applier, item, context);
    }
}

uint32_t CFArrayGetFirstIndexOfValue(CCArrayNonnullPtr array, CFRange range, const void *value) {
    uint32_t idx;
    __CFGenericValidateType(array, CFArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    const CFArrayCallBacks *cb = (CF_IS_OBJC(CFArrayGetTypeID(), array) || CF_IS_SWIFT(CFArrayGetTypeID(), array)) ? &kCFTypeArrayCallBacks : __CCArrayGetCallBacks(array);
    for (idx = 0; idx < range.length; idx++) {
        const void *item = CFArrayGetValueAtIndex(array, range.location + idx);
        if (value == item || (cb->equal && INVOKE_CALLBACK2(cb->equal, value, item)))
            return idx + range.location;
    }
    return kCFNotFound;
}

uint32_t CFArrayGetLastIndexOfValue(CCArrayNonnullPtr array, CFRange range, const void *value) {
    uint32_t idx;
    __CFGenericValidateType(array, CFArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    const CFArrayCallBacks *cb = (CF_IS_OBJC(CFArrayGetTypeID(), array) || CF_IS_SWIFT(CFArrayGetTypeID(), array)) ? &kCFTypeArrayCallBacks : __CCArrayGetCallBacks(array);
    for (idx = range.length; idx--;) {
        const void *item = CFArrayGetValueAtIndex(array, range.location + idx);
        if (value == item || (cb->equal && INVOKE_CALLBACK2(cb->equal, value, item)))
            return idx + range.location;
    }
    return kCFNotFound;
}

void CFArrayAppendValue(CFMutableArrayRef array, const void *value) {
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.addObject, value);
    CF_OBJC_FUNCDISPATCHV(CFArrayGetTypeID(), void, (NSMutableArray *)array, addObject:(id)value);
    
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCFArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    _CFArrayReplaceValues(array, CFRangeMake(__CCArrayGetCount(array), 0), &value, 1);
}

void CFArraySetValueAtIndex(CFMutableArrayRef array, uint32_t idx, const void *value) {
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.setObject, value, idx);
    CF_OBJC_FUNCDISPATCHV(CFArrayGetTypeID(), void, (NSMutableArray *)array, setObject:(id)value atIndex:(NSUInteger)idx);
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCFArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert2(0 <= idx && idx <= __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index (%ld) out of bounds", __PRETTY_FUNCTION__, idx);
    CHECK_FOR_MUTATION(array);
    if (idx == __CCArrayGetCount(array)) {
        _CFArrayReplaceValues(array, CFRangeMake(idx, 0), &value, 1);
    } else {
        BEGIN_MUTATION(array);
        const void *old_value;
        const CFArrayCallBacks *cb = __CCArrayGetCallBacks(array);
        CFAllocatorRef allocator = __CFGetAllocator(array);
        struct __CCArrayBucket *bucket = __CCArrayGetBucketAtIndex(array, idx);
        if (NULL != cb->retain) {
            value = (void *)INVOKE_CALLBACK2(cb->retain, allocator, value);
        }
        old_value = bucket->_item;
        bucket->_item = value;
        if (NULL != cb->release) {
            INVOKE_CALLBACK2(cb->release, allocator, old_value);
        }
        array->_mutations++;
        END_MUTATION(array);
    }
}

void CFArrayInsertValueAtIndex(CFMutableArrayRef array, uint32_t idx, const void *value) {
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.insertObject, idx, value);
    CF_OBJC_FUNCDISPATCHV(CFArrayGetTypeID(), void, (NSMutableArray *)array, insertObject:(id)value atIndex:(NSUInteger)idx);
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCFArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert2(0 <= idx && idx <= __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index (%ld) out of bounds", __PRETTY_FUNCTION__, idx);
    CHECK_FOR_MUTATION(array);
    _CFArrayReplaceValues(array, CFRangeMake(idx, 0), &value, 1);
}

// NB: AddressBook on the Phone is a fragile flower, so this function cannot do anything
// that causes the values to be retained or released.
void CFArrayExchangeValuesAtIndices(CFMutableArrayRef array, uint32_t idx1, uint32_t idx2) {
    const void *tmp;
    struct __CCArrayBucket *bucket1, *bucket2;
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.exchangeObjectAtIndex, idx1, idx2);
    CF_OBJC_FUNCDISPATCHV(CFArrayGetTypeID(), void, (NSMutableArray *)array, exchangeObjectAtIndex:(NSUInteger)idx1 withObjectAtIndex:(NSUInteger)idx2);
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CFAssert2(0 <= idx1 && idx1 < __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index #1 (%ld) out of bounds", __PRETTY_FUNCTION__, idx1);
    CFAssert2(0 <= idx2 && idx2 < __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index #2 (%ld) out of bounds", __PRETTY_FUNCTION__, idx2);
    CFAssert1(__CCArrayGetType(array) != __kCFArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    BEGIN_MUTATION(array);
    bucket1 = __CCArrayGetBucketAtIndex(array, idx1);
    bucket2 = __CCArrayGetBucketAtIndex(array, idx2);
    tmp = bucket1->_item;
    bucket1->_item = bucket2->_item;
    bucket2->_item = tmp;
    array->_mutations++;
    END_MUTATION(array);
}

void CFArrayRemoveValueAtIndex(CFMutableArrayRef array, uint32_t idx) {
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.removeObjectAtIndex, idx);
    CF_OBJC_FUNCDISPATCHV(CFArrayGetTypeID(), void, (NSMutableArray *)array, removeObjectAtIndex:(NSUInteger)idx);
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCFArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert2(0 <= idx && idx < __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index (%ld) out of bounds", __PRETTY_FUNCTION__, idx);
    CHECK_FOR_MUTATION(array);
    _CFArrayReplaceValues(array, CFRangeMake(idx, 1), NULL, 0);
}

void CFArrayRemoveAllValues(CFMutableArrayRef array) {
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.removeAllObjects);
    CF_OBJC_FUNCDISPATCHV(CFArrayGetTypeID(), void, (NSMutableArray *)array, removeAllObjects);
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCFArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    BEGIN_MUTATION(array);
    __CCArrayReleaseValues(array, CFRangeMake(0, __CCArrayGetCount(array)), true);
    __CCArraySetCount(array, 0);
    array->_mutations++;
    END_MUTATION(array);
}

// may move deque storage, as it may need to grow deque
static void __CCArrayRepositionDequeRegions(CFMutableArrayRef array, CFRange range, uint32_t newCount) {
    // newCount elements are going to replace the range, and the result will fit in the deque
    struct __CCArrayDeque *deque = (struct __CCArrayDeque *)array->_store;
    struct __CCArrayBucket *buckets;
    uint32_t cnt, futureCnt, numNewElems;
    uint32_t L, A, B, C, R;
    
    buckets = (struct __CCArrayBucket *)((uint8_t *)deque + sizeof(struct __CCArrayDeque));
    cnt = __CCArrayGetCount(array);
    futureCnt = cnt - range.length + newCount;
    
    L = deque->_leftIdx;        // length of region to left of deque
    A = range.location;            // length of region in deque to left of replaced range
    B = range.length;            // length of replaced range
    C = cnt - B - A;            // length of region in deque to right of replaced range
    R = deque->_capacity - cnt - L;    // length of region to right of deque
    numNewElems = newCount - B;
    
    uint32_t wiggle = deque->_capacity >> 17;
    if (wiggle < 4) wiggle = 4;
    if (deque->_capacity < (uint32_t)futureCnt || (cnt < futureCnt && L + R < wiggle)) {
        // must be inserting or space is tight, reallocate and re-center everything
        uint32_t capacity = __CCRingBufferRoundUpCapacity(futureCnt + wiggle);
        uint32_t size = sizeof(struct __CCArrayDeque) + capacity * sizeof(struct __CCArrayBucket);
        CFAllocatorRef allocator = __CFGetAllocator(array);
        struct __CCArrayDeque *newDeque = (struct __CCArrayDeque *)CFAllocatorAllocate(allocator, size, 0);
        if (__CFOASafe) __CFSetLastAllocationEventName(newDeque, "CFArray (store-deque)");
        struct __CCArrayBucket *newBuckets = (struct __CCArrayBucket *)((uint8_t *)newDeque + sizeof(struct __CCArrayDeque));
        uint32_t oldL = L;
        uint32_t newL = (capacity - futureCnt) / 2;
        uint32_t oldC0 = oldL + A + B;
        uint32_t newC0 = newL + A + newCount;
        newDeque->_leftIdx = newL;
        newDeque->_capacity = capacity;
        if (0 < A) memmove(newBuckets + newL, buckets + oldL, A * sizeof(struct __CCArrayBucket));
        if (0 < C) memmove(newBuckets + newC0, buckets + oldC0, C * sizeof(struct __CCArrayBucket));
        array->_store = newDeque;
        if (deque) CFAllocatorDeallocate(allocator, deque);
        //printf("3:  array %p store is now %p (%lx)\n", array, array->_store, *(unsigned long *)(array->_store));
        return;
    }
    
    if ((numNewElems < 0 && C < A) || (numNewElems <= R && C < A)) {    // move C
        // deleting: C is smaller
        // inserting: C is smaller and R has room
        uint32_t oldC0 = L + A + B;
        uint32_t newC0 = L + A + newCount;
        if (0 < C) memmove(buckets + newC0, buckets + oldC0, C * sizeof(struct __CCArrayBucket));
        if (oldC0 > newC0) memset(buckets + newC0 + C, 0, (oldC0 - newC0) * sizeof(struct __CCArrayBucket));
    } else if ((numNewElems < 0) || (numNewElems <= L && A <= C)) {    // move A
        // deleting: A is smaller or equal (covers remaining delete cases)
        // inserting: A is smaller and L has room
        uint32_t oldL = L;
        uint32_t newL = L - numNewElems;
        deque->_leftIdx = newL;
        if (0 < A) memmove(buckets + newL, buckets + oldL, A * sizeof(struct __CCArrayBucket));
        if (newL > oldL) memset(buckets + oldL, 0, (newL - oldL) * sizeof(struct __CCArrayBucket));
    } else {
        // now, must be inserting, and either:
        //    A<=C, but L doesn't have room (R might have, but don't care)
        //    C<A, but R doesn't have room (L might have, but don't care)
        // re-center everything
        uint32_t oldL = L;
        uint32_t newL = (L + R - numNewElems) / 2;
        newL = newL - newL / 2;
        uint32_t oldC0 = oldL + A + B;
        uint32_t newC0 = newL + A + newCount;
        deque->_leftIdx = newL;
        if (newL < oldL) {
            if (0 < A) memmove(buckets + newL, buckets + oldL, A * sizeof(struct __CCArrayBucket));
            if (0 < C) memmove(buckets + newC0, buckets + oldC0, C * sizeof(struct __CCArrayBucket));
            if (oldC0 > newC0) memset(buckets + newC0 + C, 0, (oldC0 - newC0) * sizeof(struct __CCArrayBucket));
        } else {
            if (0 < C) memmove(buckets + newC0, buckets + oldC0, C * sizeof(struct __CCArrayBucket));
            if (0 < A) memmove(buckets + newL, buckets + oldL, A * sizeof(struct __CCArrayBucket));
            if (newL > oldL) memset(buckets + oldL, 0, (newL - oldL) * sizeof(struct __CCArrayBucket));
        }
    }
}

static void __CCArrayHandleOutOfMemory(CFTypeRef obj, uint32_t numBytes) {
    CFStringRef msg = CFStringCreateWithFormat(kCFAllocatorSystemDefault, NULL, CFSTR("Attempt to allocate %ld bytes for CFArray failed"), numBytes);
    {
        CFLog(kCFLogLevelCritical, CFSTR("%@"), msg);
        HALT;
    }
    CFRelease(msg);
}

// This function is for Foundation's benefit; no one else should use it.
void _CFArraySetCapacity(CFMutableArrayRef array, uint32_t cap) {
    if (CF_IS_OBJC(CFArrayGetTypeID(), array) || CF_IS_SWIFT(CFArrayGetTypeID(), array)) return;
    __CFGenericValidateType(array, CFArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCFArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert3(__CCArrayGetCount(array) <= cap, __kCFLogAssertion, "%s(): desired capacity (%ld) is less than count (%ld)", __PRETTY_FUNCTION__, cap, __CCArrayGetCount(array));
    CHECK_FOR_MUTATION(array);
    BEGIN_MUTATION(array);
    // Currently, attempting to set the capacity of an array which is the CFStorage
    // variant, or set the capacity larger than __CF_MAX_BUCKETS_PER_DEQUE, has no
    // effect.  The primary purpose of this API is to help avoid a bunch of the
    // resizes at the small capacities 4, 8, 16, etc.
    if (__CCArrayGetType(array) == __kCFArrayDeque) {
        struct __CCArrayDeque *deque = (struct __CCArrayDeque *)array->_store;
        uint32_t capacity = __CCRingBufferRoundUpCapacity(cap);
        uint32_t size = sizeof(struct __CCArrayDeque) + capacity * sizeof(struct __CCArrayBucket);
        CFAllocatorRef allocator = __CFGetAllocator(array);
        if (NULL == deque) {
            deque = (struct __CCArrayDeque *)CFAllocatorAllocate(allocator, size, 0);
            if (NULL == deque) __CCArrayHandleOutOfMemory(array, size);
            if (__CFOASafe) __CFSetLastAllocationEventName(deque, "CFArray (store-deque)");
            deque->_leftIdx = capacity / 2;
        } else {
            struct __CCArrayDeque *olddeque = deque;
            uint32_t oldcap = deque->_capacity;
            deque = (struct __CCArrayDeque *)CFAllocatorAllocate(allocator, size, 0);
            if (NULL == deque) __CCArrayHandleOutOfMemory(array, size);
            memmove(deque, olddeque, sizeof(struct __CCArrayDeque) + oldcap * sizeof(struct __CCArrayBucket));
            CFAllocatorDeallocate(allocator, olddeque);
            if (__CFOASafe) __CFSetLastAllocationEventName(deque, "CFArray (store-deque)");
        }
        deque->_capacity = capacity;
        array->_store = deque;
    }
    END_MUTATION(array);
}


void CFArrayReplaceValues(CFMutableArrayRef array, CFRange range, const void **newValues, uint32_t newCount) {
    CF_SWIFT_FUNCDISPATCHV(CFArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.replaceObjectsInRange, range, newValues, newCount);
    CF_OBJC_FUNCDISPATCHV(CFArrayGetTypeID(), void, (NSMutableArray *)array, replaceObjectsInRange:NSMakeRange(range.location, range.length) withObjects:(id *)newValues count:(NSUInteger)newCount);
    __CFGenericValidateType(array, CFArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(__CCArrayGetType(array) != __kCFArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert2(0 <= newCount, __kCFLogAssertion, "%s(): newCount (%ld) cannot be less than zero", __PRETTY_FUNCTION__, newCount);
    CHECK_FOR_MUTATION(array);
    return _CFArrayReplaceValues(array, range, newValues, newCount);
}

// This function does no ObjC dispatch or argument checking;
// It should only be called from places where that dispatch and check has already been done, or NSCFArray
void _CFArrayReplaceValues(CFMutableArrayRef array, CFRange range, const void **newValues, uint32_t newCount) {
    CHECK_FOR_MUTATION(array);
    BEGIN_MUTATION(array);
    const CFArrayCallBacks *cb;
    uint32_t idx, cnt, futureCnt;
    const void **newv, *buffer[256];
    cnt = __CCArrayGetCount(array);
    futureCnt = cnt - range.length + newCount;
    CFAssert1(newCount <= futureCnt, __kCFLogAssertion, "%s(): internal error 1", __PRETTY_FUNCTION__);
    cb = __CCArrayGetCallBacks(array);
    CFAllocatorRef allocator = __CFGetAllocator(array);
    
    /* Retain new values if needed, possibly allocating a temporary buffer for them */
    if (NULL != cb->retain) {
        newv = (newCount <= 256) ? (const void **)buffer : (const void **)CFAllocatorAllocate(kCFAllocatorSystemDefault, newCount * sizeof(void *), 0);
        if (newv != buffer && __CFOASafe) __CFSetLastAllocationEventName(newv, "CFArray (temp)");
        for (idx = 0; idx < newCount; idx++) {
            newv[idx] = (void *)INVOKE_CALLBACK2(cb->retain, allocator, (void *)newValues[idx]);
        }
    } else {
        newv = newValues;
    }
    array->_mutations++;
    
    /* Now, there are three regions of interest, each of which may be empty:
     *   A: the region from index 0 to one less than the range.location
     *   B: the region of the range
     *   C: the region from range.location + range.length to the end
     * Note that index 0 is not necessarily at the lowest-address edge
     * of the available storage. The values in region B need to get
     * released, and the values in regions A and C (depending) need
     * to get shifted if the number of new values is different from
     * the length of the range being replaced.
     */
    if (0 < range.length) {
        __CCArrayReleaseValues(array, range, false);
    }
    // region B elements are now "dead"
    if (0) {
    } else if (NULL == array->_store) {
        if (0) {
        } else if (0 <= futureCnt) {
            struct __CCArrayDeque *deque;
            uint32_t capacity = __CCRingBufferRoundUpCapacity(futureCnt);
            uint32_t size = sizeof(struct __CCArrayDeque) + capacity * sizeof(struct __CCArrayBucket);
            deque = (struct __CCArrayDeque *)CFAllocatorAllocate((allocator), size, 0);
            if (__CFOASafe) __CFSetLastAllocationEventName(deque, "CFArray (store-deque)");
            deque->_leftIdx = (capacity - newCount) / 2;
            deque->_capacity = capacity;
            array->_store = deque;
        }
    } else {        // Deque
        // reposition regions A and C for new region B elements in gap
        if (0) {
        } else if (range.length != newCount) {
            __CCArrayRepositionDequeRegions(array, range, newCount);
        }
    }
    // copy in new region B elements
    if (0 < newCount) {
        if (0) {
        } else {    // Deque
            struct __CCArrayDeque *deque = (struct __CCArrayDeque *)array->_store;
            struct __CCArrayBucket *raw_buckets = (struct __CCArrayBucket *)((uint8_t *)deque + sizeof(struct __CCArrayDeque));
            memmove(raw_buckets + deque->_leftIdx + range.location, newv, newCount * sizeof(struct __CCArrayBucket));
        }
    }
    __CCArraySetCount(array, futureCnt);
    if (newv != buffer && newv != newValues) CFAllocatorDeallocate(kCFAllocatorSystemDefault, newv);
    END_MUTATION(array);
}

struct _acompareContext {
    CFComparatorFunction func;
    void *context;
};

static CFComparisonResult __CCArrayCompareValues(const void *v1, const void *v2, struct _acompareContext *context) {
    const void **val1 = (const void **)v1;
    const void **val2 = (const void **)v2;
    return (CFComparisonResult)(INVOKE_CALLBACK3(context->func, *val1, *val2, context->context));
}

CF_INLINE void __CFZSort(CFMutableArrayRef array, CFRange range, CFComparatorFunction comparator, void *context) {
    uint32_t cnt = range.length;
    while (1 < cnt) {
        for (uint32_t idx = range.location; idx < range.location + cnt - 1; idx++) {
            const void *a = CFArrayGetValueAtIndex(array, idx);
            const void *b = CFArrayGetValueAtIndex(array, idx + 1);
            if ((CFComparisonResult)(INVOKE_CALLBACK3(comparator, b, a, context)) < 0) {
                CFArrayExchangeValuesAtIndices(array, idx, idx + 1);
            }
        }
        cnt--;
    }
}

CF_PRIVATE void _CFArraySortValues(CFMutableArrayRef array, CFComparatorFunction comparator, void *context) {
    CFRange range = {0, CFArrayGetCount(array)};
    if (range.length < 2) {
        return;
    }
    // implemented abstractly, careful!
    const void **values, *buffer[256];
    values = (range.length <= 256) ? (const void **)buffer : (const void **)CFAllocatorAllocate(kCFAllocatorSystemDefault, range.length * sizeof(void *), 0);
    CFArrayGetValues(array, range, values);
    struct _acompareContext ctx;
    ctx.func = comparator;
    ctx.context = context;
    CFQSortArray(values, range.length, sizeof(void *), (CFComparatorFunction)__CCArrayCompareValues, &ctx);
    CFArrayReplaceValues(array, range, values, range.length);
    if (values != buffer) CFAllocatorDeallocate(kCFAllocatorSystemDefault, values);
}

void CFArraySortValues(CFMutableArrayRef array, CFRange range, CFComparatorFunction comparator, void *context) {
    FAULT_CALLBACK((void **)&(comparator));
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(NULL != comparator, __kCFLogAssertion, "%s(): pointer to comparator function may not be NULL", __PRETTY_FUNCTION__);
    Boolean immutable = false;
    if (CF_IS_OBJC(CFArrayGetTypeID(), array) || CF_IS_SWIFT(CFArrayGetTypeID(), array)) {
        BOOL result;
        result = CF_OBJC_CALLV((NSMutableArray *)array, isKindOfClass:[NSMutableArray class]); // TODO: Fixme for swift (we need a isKindOfClass replacement: (array as? NSMutableArray) != nil)
        immutable = !result;
    } else if (__kCFArrayImmutable == __CCArrayGetType(array)) {
        immutable = true;
    }
    const CFArrayCallBacks *cb = NULL;
    if (CF_IS_OBJC(CFArrayGetTypeID(), array) || CF_IS_SWIFT(CFArrayGetTypeID(), array)) {
        cb = &kCFTypeArrayCallBacks;
    } else {
        cb = __CCArrayGetCallBacks(array);
    }
    if (!immutable && ((cb->retain && !cb->release) || (!cb->retain && cb->release))) {
        __CFZSort(array, range, comparator, context);
        return;
    }
    if (range.length < 2) {
        return;
    }
    // implemented abstractly, careful!
    const void **values, *buffer[256];
    values = (range.length <= 256) ? (const void **)buffer : (const void **)CFAllocatorAllocate(kCFAllocatorSystemDefault, range.length * sizeof(void *), 0);
    CFArrayGetValues(array, range, values);
    struct _acompareContext ctx;
    ctx.func = comparator;
    ctx.context = context;
    CFQSortArray(values, range.length, sizeof(void *), (CFComparatorFunction)__CCArrayCompareValues, &ctx);
    if (!immutable) CFArrayReplaceValues(array, range, values, range.length);
    if (values != buffer) CFAllocatorDeallocate(kCFAllocatorSystemDefault, values);
}

uint32_t CFArrayBSearchValues(CCArrayNonnullPtr array, CFRange range, const void *value, CFComparatorFunction comparator, void *context) {
    FAULT_CALLBACK((void **)&(comparator));
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(NULL != comparator, __kCFLogAssertion, "%s(): pointer to comparator function may not be NULL", __PRETTY_FUNCTION__);
    // implemented abstractly, careful!
    if (range.length <= 0) return range.location;
    const void *item = CFArrayGetValueAtIndex(array, range.location + range.length - 1);
    if ((CFComparisonResult)(INVOKE_CALLBACK3(comparator, item, value, context)) < 0) {
        return range.location + range.length;
    }
    item = CFArrayGetValueAtIndex(array, range.location);
    if ((CFComparisonResult)(INVOKE_CALLBACK3(comparator, value, item, context)) < 0) {
        return range.location;
    }
    SInt32 lg = flsl(range.length) - 1;    // lg2(range.length)
    item = CFArrayGetValueAtIndex(array, range.location + -1 + (1 << lg));
    // idx will be the current probe index into the range
    uint32_t idx = (comparator(item, value, context) < 0) ? range.length - (1 << lg) : -1;
    while (lg--) {
        item = CFArrayGetValueAtIndex(array, range.location + idx + (1 << lg));
        if (comparator(item, value, context) < 0) {
            idx += (1 << lg);
        }
    }
    idx++;
    return idx + range.location;
}

void CFArrayAppendArray(CFMutableArrayRef array, CCArrayNonnullPtr otherArray, CFRange otherRange) {
    __CCArrayValidateRange(otherArray, otherRange, __PRETTY_FUNCTION__);
    // implemented abstractly, careful!
    for (uint32_t idx = otherRange.location; idx < otherRange.location + otherRange.length; idx++) {
        CFArrayAppendValue(array, CFArrayGetValueAtIndex(otherArray, idx));
    }
}




#endif /* CCArrayInternal_h */
