//
//  CCArray.c
//  CCollection
//
//  Created by vector on 2019/6/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCArray.h"
#include "CInteger.h"
#include "CType.h"


static const size_t CCElementLoadSize[13] = {
    0, 1, 1, 1, sizeof(sint16_t), sizeof(uint16_t), sizeof(sint32_t), sizeof(uint32_t), sizeof(float_t), sizeof(CPointer), sizeof(sint64_t), sizeof(uint64_t), sizeof(double_t),
};

#define CF_ARRAY_ALWAYS_BRIDGE 0


struct __CCArrayBucket {
    const void *_item;
};


static inline uint32_t __CCArrayDequeRoundUpCapacity(uint32_t capacity) {
    if (capacity < 4) return 4;
    uint64_t c = capacity;
    uint64_t s = 1ull << (CUInt64MostSignificant(c) + 1);
    if (s > 0x20000000L) {
        s = 0x20000000L;
    }
    return (uint32_t)s;
}

typedef struct __CCArrayRingBuffer {
    uint32_t _capacity;
    uint32_t _elementSize;
    
    uint32_t _indexOffset;
    uint32_t _indexMask;
    
    uint8_t items[];
} CCArrayRingBuffer_s;

typedef struct __CCArrayImmutableBuffer {
#if CBuild64Bit
    _Atomic(uint_fast64_t) ref;
#else
    _Atomic(uint_fast32_t) ref;
#endif
    uint32_t _capacity;
    uint32_t _elementSize;
    
    
    uint8_t items[];
} CCArrayImmutableBuffer_s;


uint32_t counts[16] = {0x10L, 0x20L, 0x40L, 0x80L, 0x100L, 0x200L, 0x400L, 0x800L, 0x1000L, 0x2000L, 0x4000L, 0x8000L, };

static const uint32_t __CCArrayImmutable = 0;
static const uint32_t __CCArrayMutable = 1;



typedef struct {
//    CFRuntimeBase _base;
    CCBaseCallBacks _callBacks;
    uint32_t _elementSize: 24;//
    uint32_t _mutable: 8;
    uint32_t _count;        /* number of objects */

    void * _store;           /* can be NULL when CCArrayMutable */
} CCArray_s;
typedef CCArray_s * _Nonnull CCArrayNonnullPtr;

static inline CCArrayNonnullPtr __CCArrayImmutableAllocate(CCBaseCallBacks * _Nullable callBacks, uint32_t elementSize, CCArrayImmutableBuffer_s * _Nonnull buffer) {
    CCArrayNonnullPtr array = CCAllocate(sizeof(CCArray_s));
    array->_mutable = __CCArrayImmutable;
    array->_store = buffer;
    array->_elementSize = buffer->_elementSize;
    array->_count = buffer->_capacity;
    return array;
}

static inline CCArrayImmutableBuffer_s * _Nonnull __CCArrayImmutableBufferAllocate(uint32_t capacity, uint32_t elementStoreSize) {
    size_t s = capacity;
    s = s * elementStoreSize;
    CCArrayImmutableBuffer_s * buffer = CCAllocate(sizeof(CCArrayImmutableBuffer_s) + s);
    buffer->_elementSize = elementStoreSize;
    buffer->_capacity = capacity;
    return buffer;
}



static inline uint32_t __CCArrayGetType(CCArrayNonnullPtr _Nonnull array) {
    return array->_mutable;
}

static inline void __CCArraySetType(CCArrayNonnullPtr array, uint32_t v) {
    array->_mutable = v;
}

static inline uint32_t __CCArrayGetCount(CCArrayNonnullPtr _Nonnull array) {
    return array->_count;
}

static inline void __CCArraySetCount(CCArrayNonnullPtr array, uint32_t v) {
    array->_count = v;
}

//static inline size_t __CCArrayGetMemoryOffset(CCArrayNonnullPtr array, uint32_t index) {
//    size_t size = index;
//    size = size * array->_elementSize;
//    return size;
//}

/* This shouldn't be called if the array count is 0. */
static inline void * _Nonnull __CCArrayGetItemAtIndex(CCArrayNonnullPtr array, uint32_t idx) {
    switch (__CCArrayGetType(array)) {
        case __CCArrayImmutable: {
            CCArrayImmutableBuffer_s * buffer = (CCArrayImmutableBuffer_s *)(array->_store);
            return &(buffer->items[idx * buffer->_elementSize]);
        }
            break;
        case __CCArrayMutable: {
            CCArrayRingBuffer_s * buffer = (CCArrayRingBuffer_s *)(array->_store);
            uint32_t index = idx + buffer->_indexOffset;
            index = index & buffer->_indexMask;
            size_t memoryOffset = index;
            memoryOffset = memoryOffset * buffer->_elementSize;
            return &(buffer->items[memoryOffset]);
        }
            break;
        default:
            abort();
            break;
    }
}


/*
 CCRetainCallBack_f _Nullable retain;
 CCReleaseCallBack_f _Nullable release;
 CCEqualCallBack_f _Nonnull equal;
 */

static inline CCRetainCallBack_f _Nullable __CCArrayGetRetainCallBack(CCArrayNonnullPtr array) {
    return array->_callBacks.retain;
}
static inline CCReleaseCallBack_f _Nullable __CCArrayGetReleaseCallBack(CCArrayNonnullPtr array) {
    return array->_callBacks.release;
}
static inline CCEqualCallBack_f _Nonnull __CCArrayGetEqualCallBack(CCArrayNonnullPtr array) {
    return array->_callBacks.equal;
}



static inline CCArrayNonnullPtr __CCArraySubarrayInRange(CCArrayNonnullPtr array, CCRange_s range, _Bool retainStorageIfPossible) {
    CCRetainCallBack_f retainFunc = __CCArrayGetRetainCallBack(array);

    CCArrayImmutableBuffer_s * buffer = __CCArrayImmutableBufferAllocate(range.length, array->_elementSize);

    uint32_t idx;
    switch (__CCArrayGetType(array)) {
        case __CCArrayImmutable: {
            memcpy(buffer->items, __CCArrayGetItemAtIndex(array, range.location), buffer->_elementSize * range.length);
            if (NULL != retainFunc && retainStorageIfPossible) {
                uint8_t * ptr = buffer->items;
                for (idx = 0; idx < range.length; idx++) {
                    retainFunc(ptr, array->_elementSize);
                    ptr += array->_elementSize;
                }
            }
        }
            break;
        case __CCArrayMutable: {
            CCArrayRingBuffer_s * ringBuffer = (CCArrayRingBuffer_s *)array->_store;
            uint32_t index = range.location + ringBuffer->_indexOffset;
            index = index & ringBuffer->_indexMask;
            if (index + range.length > ringBuffer->_capacity + 1) {//2部分
                uint32_t length = ringBuffer->_capacity + 1 - index;
                size_t memoryOffset = index;
                memoryOffset = memoryOffset * buffer->_elementSize;
                memcpy(buffer->items, &(buffer->items[memoryOffset]), array->_elementSize * length);
                memcpy(buffer->items, buffer->items, array->_elementSize * (range.length - length));
            } else {
                size_t memoryOffset = index;
                memoryOffset = memoryOffset * buffer->_elementSize;
                memcpy(buffer->items, &(buffer->items[memoryOffset]), array->_elementSize * range.length);
            }

            if (NULL != retainFunc && retainStorageIfPossible) {
                uint8_t * ptr = buffer->items;
                for (idx = 0; idx < range.length; idx++) {
                    retainFunc(ptr, array->_elementSize);
                    ptr += array->_elementSize;
                }
            }
        }
            break;
        default:
            abort();
            break;
    }
    return __CCArrayImmutableAllocate(&(array->_callBacks), array->_elementSize, buffer);
}


static void __CCArrayReleaseValues(CCArrayNonnullPtr array, CCRange_s range) {
    CCReleaseCallBack_f releaseFunc = __CCArrayGetReleaseCallBack(array);
    if (NULL == releaseFunc) {
        return;
    }
    if (range.length == 0) {
        return;
    }
    uint32_t idx;
    switch (__CCArrayGetType(array)) {
        case __CCArrayImmutable: {
            uint8_t * ptr = __CCArrayGetItemAtIndex(array, range.location);
            for (idx = 0; idx < range.length; idx++) {
                releaseFunc(ptr, array->_elementSize);
                ptr += array->_elementSize;
            }
        }
            break;
        case __CCArrayMutable: {
            CCArrayRingBuffer_s * ringBuffer = (CCArrayRingBuffer_s *)array->_store;
            uint32_t index = range.location + ringBuffer->_indexOffset;
            index = index & ringBuffer->_indexMask;
            if (index + range.length > ringBuffer->_capacity + 1) {//2部分
                uint32_t length = ringBuffer->_capacity + 1 - index;
                size_t memoryOffset = index;
                memoryOffset = memoryOffset * ringBuffer->_elementSize;
                uint8_t * ptr = __CCArrayGetItemAtIndex(array, range.location);
                for (idx = 0; idx < length; idx++) {
                    releaseFunc(ptr, array->_elementSize);
                    ptr += array->_elementSize;
                }
                
                ptr = __CCArrayGetItemAtIndex(array, range.location + length);
                length = range.length - length;
                for (idx = 0; idx < length; idx++) {
                    releaseFunc(ptr, array->_elementSize);
                    ptr += array->_elementSize;
                }
            } else {
                size_t memoryOffset = index;
                memoryOffset = memoryOffset * ringBuffer->_elementSize;
                uint8_t * ptr = __CCArrayGetItemAtIndex(array, range.location);
                for (idx = 0; idx < range.length; idx++) {
                    releaseFunc(ptr, array->_elementSize);
                    ptr += array->_elementSize;
                }
            }
            break;
        }
    }
}

static inline _Bool __CCArrayValidateRange(CCArrayNonnullPtr array, CCRange_s range, const char *func) {
    uint32_t count = __CCArrayGetCount(array);
    if (range.location <= count) {
        
        return true;
    } else {
        return false;
    }
//    CFAssert2(0 <= range.length, __kCFLogAssertion, "%s(): range.length (%ld) cannot be less than zero", func, range.length);
//    CFAssert3(range.location + range.length <= CCArrayGetCount(array), __kCFLogAssertion, "%s(): ending index (%ld) out of bounds (0, %ld)", func, range.location + range.length, CCArrayGetCount(array));
}

//static Boolean __CCArrayEqual(CFTypeRef cf1, CFTypeRef cf2) {
//    CCArrayNonnullPtr array1 = (CCArrayNonnullPtr)cf1;
//    CCArrayNonnullPtr array2 = (CCArrayNonnullPtr)cf2;
//    const CCBaseCallBacks *cb1, *cb2;
//    uint32_t idx, cnt;
//    if (array1 == array2) return true;
//    cnt = __CCArrayGetCount(array1);
//    if (cnt != __CCArrayGetCount(array2)) return false;
//    cb1 = __CCArrayGetCallBacks(array1);
//    cb2 = __CCArrayGetCallBacks(array2);
//    if (cb1->equal != cb2->equal) return false;
//    if (0 == cnt) return true;    /* after function comparison! */
//    for (idx = 0; idx < cnt; idx++) {
//        const void *val1 = __CCArrayGetBucketAtIndex(array1, idx)->_item;
//        const void *val2 = __CCArrayGetBucketAtIndex(array2, idx)->_item;
//        if (val1 != val2) {
//            if (NULL == cb1->equal) return false;
//            if (!INVOKE_CALLBACK2(cb1->equal, val1, val2)) return false;
//        }
//    }
//    return true;
//}

static void __CCArrayDeallocate(CCArrayNonnullPtr array) {
    __CCArrayReleaseValues(array, CCRangeMake(0, __CCArrayGetCount(array)));
}

//static CCArrayNonnullPtr __CCArrayInit(CFAllocatorRef allocator, UInt32 flags, uint32_t capacity, const CCBaseCallBacks *callBacks) {
//    struct __CCArray *memory;
//    UInt32 size;
//    __CFBitfieldSetValue(flags, 31, 2, 0);
//    if (__CCBaseCallBacksMatchNull(callBacks)) {
//        __CFBitfieldSetValue(flags, 3, 2, __kCCArrayHasNullCallBacks);
//    } else if (__CCBaseCallBacksMatchCFType(callBacks)) {
//        __CFBitfieldSetValue(flags, 3, 2, __kCCArrayHasCFTypeCallBacks);
//    } else {
//        __CFBitfieldSetValue(flags, 3, 2, __kCCArrayHasCustomCallBacks);
//    }
//    size = __CCArrayGetSizeOfType(flags) - sizeof(CFRuntimeBase);
//    switch (__CFBitfieldGetValue(flags, 1, 0)) {
//        case __kCCArrayImmutable:
//            size += capacity * sizeof(struct __CCArrayBucket);
//            break;
//        case __kCCArrayDeque:
//            break;
//    }
//    memory = (struct __CCArray*)_CFRuntimeCreateInstance(allocator, CCArrayGetTypeID(), size, NULL);
//    if (NULL == memory) {
//        return NULL;
//    }
//    __CFRuntimeSetValue(memory, 6, 0, flags);
//    __CCArraySetCount((CCArrayNonnullPtr)memory, 0);
//    switch (__CFBitfieldGetValue(flags, 1, 0)) {
//        case __kCCArrayImmutable:
//            if (__CFOASafe) __CFSetLastAllocationEventName(memory, "CCArray (immutable)");
//            break;
//        case __kCCArrayDeque:
//            if (__CFOASafe) __CFSetLastAllocationEventName(memory, "CCArray (mutable-variable)");
//            ((struct __CCArray *)memory)->_mutations = 1;
//            ((struct __CCArray *)memory)->_mutInProgress = 0;
//            ((struct __CCArray*)memory)->_store = NULL;
//            break;
//    }
//    if (__kCCArrayHasCustomCallBacks == __CFBitfieldGetValue(flags, 3, 2)) {
//        CCBaseCallBacks *cb = (CCBaseCallBacks *)__CCArrayGetCallBacks((CCArrayNonnullPtr)memory);
//        *cb = *callBacks;
//        FAULT_CALLBACK((void **)&(cb->retain));
//        FAULT_CALLBACK((void **)&(cb->release));
//        FAULT_CALLBACK((void **)&(cb->copyDescription));
//        FAULT_CALLBACK((void **)&(cb->equal));
//    }
//    return (CCArrayNonnullPtr)memory;
//}

//CCArrayNonnullPtr __CCArrayCreateTransfer(CFAllocatorRef allocator, const void **values, uint32_t numValues) {
//    CFAssert2(0 <= numValues, __kCFLogAssertion, "%s(): numValues (%ld) cannot be less than zero", __PRETTY_FUNCTION__, numValues);
//    UInt32 flags = __CCArrayImmutable;
//    __CFBitfieldSetValue(flags, 31, 2, 0);
//    __CFBitfieldSetValue(flags, 3, 2, __kCCArrayHasCFTypeCallBacks);
//    UInt32 size = __CCArrayGetSizeOfType(flags) - sizeof(CFRuntimeBase);
//    size += numValues * sizeof(struct __CCArrayBucket);
//    struct __CCArray *memory = (struct __CCArray*)_CFRuntimeCreateInstance(allocator, CCArrayGetTypeID(), size, NULL);
//    if (NULL == memory) {
//        return NULL;
//    }
//    __CFRuntimeSetValue(memory, 6, 0, flags);
//    __CCArraySetCount(memory, numValues);
//    memmove(__CCArrayGetBucketsPtr(memory), values, sizeof(void *) * numValues);
//    if (__CFOASafe) __CFSetLastAllocationEventName(memory, "CCArray (immutable)");
//    return (CCArrayNonnullPtr)memory;
//}

CCArrayNonnullPtr __CCArrayCreate0(CFAllocatorRef allocator, const void **values, uint32_t numValues, const CCBaseCallBacks *callBacks) {
    CCArrayNonnullPtr result;
    const CCBaseCallBacks *cb;
    struct __CCArrayBucket *buckets;
    uint32_t idx;
    CFAssert2(0 <= numValues, __kCFLogAssertion, "%s(): numValues (%ld) cannot be less than zero", __PRETTY_FUNCTION__, numValues);
    result = __CCArrayInit(allocator, __kCCArrayImmutable, numValues, callBacks);
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

//CFMutableArrayRef __CCArrayCreateMutable0(CFAllocatorRef allocator, uint32_t capacity, const CCBaseCallBacks *callBacks) {
//    CFAssert2(0 <= capacity, __kCFLogAssertion, "%s(): capacity (%ld) cannot be less than zero", __PRETTY_FUNCTION__, capacity);
//    CFAssert2(capacity <= LONG_MAX / sizeof(void *), __kCFLogAssertion, "%s(): capacity (%ld) is too large for this architecture", __PRETTY_FUNCTION__, capacity);
//    return (CFMutableArrayRef)__CCArrayInit(allocator, __kCCArrayDeque, capacity, callBacks);
//}

CCArrayNonnullPtr __CCArrayCreateCopy0(CCArrayNonnullPtr array) {
    
    switch (__CCArrayGetType(array)) {
        case __CCArrayImmutable: {

        }
            break;
        case __CCArrayMutable: {
            return __CCArraySubarrayInRange(array, CCRangeMake(0, __CCArrayGetCount(array)), true);
        }
            break;
        default:
            abort();
            break;
    }

    return result;
}

CF_PRIVATE CFMutableArrayRef __CCArrayCreateMutableCopy0(CFAllocatorRef allocator, uint32_t capacity, CCArrayNonnullPtr array) {
    CFMutableArrayRef result;
    const CCBaseCallBacks *cb;
    uint32_t idx, numValues = CCArrayGetCount(array);
    UInt32 flags;
    if (CF_IS_OBJC(CCArrayGetTypeID(), array) || CF_IS_SWIFT(CCArrayGetTypeID(), array)) {
        cb = &kCFTypeArrayCallBacks;
    }
    else {
        cb = __CCArrayGetCallBacks(array);
    }
    flags = __kCCArrayDeque;
    result = (CFMutableArrayRef)__CCArrayInit(allocator, flags, capacity, cb);
    if (0 == capacity) _CCArraySetCapacity(result, numValues);
    for (idx = 0; idx < numValues; idx++) {
        const void *value = CCArrayGetValueAtIndex(array, idx);
        CCArrayAppendValue(result, value);
    }
    return result;
}


CCArrayNonnullPtr CCArrayCreate(CFAllocatorRef allocator, const void **values, uint32_t numValues, const CCBaseCallBacks *callBacks) {
    return __CCArrayCreate0(allocator, values, numValues, callBacks);
}

CFMutableArrayRef CCArrayCreateMutable(CFAllocatorRef allocator, uint32_t capacity, const CCBaseCallBacks *callBacks) {
    return __CCArrayCreateMutable0(allocator, capacity, callBacks);
}

CCArrayNonnullPtr CCArrayCreateCopy(CFAllocatorRef allocator, CCArrayNonnullPtr array) {
    return __CCArrayCreateCopy0(allocator, array);
}

CFMutableArrayRef CCArrayCreateMutableCopy(CFAllocatorRef allocator, uint32_t capacity, CCArrayNonnullPtr array) {
    return __CCArrayCreateMutableCopy0(allocator, capacity, array);
}

#endif

CF_PRIVATE uint32_t _CFNonObjCArrayGetCount(CCArrayNonnullPtr array) {
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CHECK_FOR_MUTATION(array);
    return __CCArrayGetCount(array);
}

uint32_t CCArrayGetCount(CCArrayNonnullPtr array) {
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), uint32_t, (CFSwiftRef)array, NSArray.count);
    CF_OBJC_FUNCDISPATCHV(CCArrayGetTypeID(), uint32_t, (NSArray *)array, count);
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CHECK_FOR_MUTATION(array);
    return __CCArrayGetCount(array);
}

uint32_t CCArrayGetCountOfValue(CCArrayNonnullPtr array, CFRange range, const void *value) {
    uint32_t idx, count = 0;
    __CFGenericValidateType(array, CCArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    const CCBaseCallBacks *cb = (CF_IS_OBJC(CCArrayGetTypeID(), array) || CF_IS_SWIFT(CCArrayGetTypeID(), array)) ? &kCFTypeArrayCallBacks : __CCArrayGetCallBacks(array);
    for (idx = 0; idx < range.length; idx++) {
        const void *item = CCArrayGetValueAtIndex(array, range.location + idx);
        if (value == item || (cb->equal && INVOKE_CALLBACK2(cb->equal, value, item))) {
            count++;
        }
    }
    return count;
}

Boolean CCArrayContainsValue(CCArrayNonnullPtr array, CFRange range, const void *value) {
    uint32_t idx;
    __CFGenericValidateType(array, CCArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    const CCBaseCallBacks *cb = (CF_IS_OBJC(CCArrayGetTypeID(), array) || CF_IS_SWIFT(CCArrayGetTypeID(), array)) ? &kCFTypeArrayCallBacks : __CCArrayGetCallBacks(array);
    for (idx = 0; idx < range.length; idx++) {
        const void *item = CCArrayGetValueAtIndex(array, range.location + idx);
        if (value == item || (cb->equal && INVOKE_CALLBACK2(cb->equal, value, item))) {
            return true;
        }
    }
    return false;
}

const void *CCArrayGetValueAtIndex(CCArrayNonnullPtr array, uint32_t idx) {
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), const void *, (CFSwiftRef)array, NSArray.objectAtIndex, idx);
    
    
#if !CF_ARRAY_ALWAYS_BRIDGE
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CFAssert2(0 <= idx && idx < __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index (%ld) out of bounds", __PRETTY_FUNCTION__, idx);
    Boolean outOfBounds = false;
    const void *result = _CCArrayCheckAndGetValueAtIndex(array, idx, &outOfBounds);
    if (outOfBounds) HALT;
    return result;
#endif
}

// This is for use by NSCCArray; it avoids ObjC dispatch, and checks for out of bounds
const void *_CCArrayCheckAndGetValueAtIndex(CCArrayNonnullPtr array, uint32_t idx, Boolean *outOfBounds) {
    CHECK_FOR_MUTATION(array);
    if (0 <= idx && idx < __CCArrayGetCount(array)) return __CCArrayGetBucketAtIndex(array, idx)->_item;
    if (outOfBounds) *outOfBounds = true;
    return (void *)(-1);
}


void CCArrayGetValues(CCArrayNonnullPtr array, CFRange range, const void **values) {
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), void, (CFSwiftRef)array, NSArray.getObjects, range, values);
    CF_OBJC_FUNCDISPATCHV(CCArrayGetTypeID(), void, (NSArray *)array, getObjects:(id *)values range:NSMakeRange(range.location, range.length));
    __CFGenericValidateType(array, CCArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(NULL != values, __kCFLogAssertion, "%s(): pointer to values may not be NULL", __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    if (0 < range.length) {
        switch (__CCArrayGetType(array)) {
            case __kCCArrayImmutable:
            case __kCCArrayDeque:
                memmove(values, __CCArrayGetBucketsPtr(array) + range.location, range.length * sizeof(struct __CCArrayBucket));
                break;
        }
    }
}

CF_EXPORT unsigned long _CCArrayFastEnumeration(CCArrayNonnullPtr array, struct __objcFastEnumerationStateEquivalent *state, void *stackbuffer, unsigned long count) {
    CHECK_FOR_MUTATION(array);
    if (array->_count == 0) return 0;
    enum { ATSTART = 0, ATEND = 1 };
    switch (__CCArrayGetType(array)) {
        case __kCCArrayImmutable:
            if (state->state == ATSTART) { /* first time */
                static const unsigned long const_mu = 1;
                state->state = ATEND;
                state->mutationsPtr = (unsigned long *)&const_mu;
                state->itemsPtr = (unsigned long *)__CCArrayGetBucketsPtr(array);
                return array->_count;
            }
            return 0;
        case __kCCArrayDeque:
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


void CCArrayApplyFunction(CCArrayNonnullPtr array, CFRange range, CCArrayApplierFunction applier, void *context) {
    uint32_t idx;
    FAULT_CALLBACK((void **)&(applier));
    __CFGenericValidateType(array, CCArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(NULL != applier, __kCFLogAssertion, "%s(): pointer to applier function may not be NULL", __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    for (idx = 0; idx < range.length; idx++) {
        const void *item = CCArrayGetValueAtIndex(array, range.location + idx);
        INVOKE_CALLBACK2(applier, item, context);
    }
}

uint32_t CCArrayGetFirstIndexOfValue(CCArrayNonnullPtr array, CFRange range, const void *value) {
    uint32_t idx;
    __CFGenericValidateType(array, CCArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    const CCBaseCallBacks *cb = (CF_IS_OBJC(CCArrayGetTypeID(), array) || CF_IS_SWIFT(CCArrayGetTypeID(), array)) ? &kCFTypeArrayCallBacks : __CCArrayGetCallBacks(array);
    for (idx = 0; idx < range.length; idx++) {
        const void *item = CCArrayGetValueAtIndex(array, range.location + idx);
        if (value == item || (cb->equal && INVOKE_CALLBACK2(cb->equal, value, item)))
            return idx + range.location;
    }
    return kCFNotFound;
}

uint32_t CCArrayGetLastIndexOfValue(CCArrayNonnullPtr array, CFRange range, const void *value) {
    uint32_t idx;
    __CFGenericValidateType(array, CCArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    const CCBaseCallBacks *cb = (CF_IS_OBJC(CCArrayGetTypeID(), array) || CF_IS_SWIFT(CCArrayGetTypeID(), array)) ? &kCFTypeArrayCallBacks : __CCArrayGetCallBacks(array);
    for (idx = range.length; idx--;) {
        const void *item = CCArrayGetValueAtIndex(array, range.location + idx);
        if (value == item || (cb->equal && INVOKE_CALLBACK2(cb->equal, value, item)))
            return idx + range.location;
    }
    return kCFNotFound;
}

void CCArrayAppendValue(CFMutableArrayRef array, const void *value) {
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.addObject, value);
    CF_OBJC_FUNCDISPATCHV(CCArrayGetTypeID(), void, (NSMutableArray *)array, addObject:(id)value);
    
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCCArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CHECK_FOR_MUTATION(array);
    _CCArrayReplaceValues(array, CFRangeMake(__CCArrayGetCount(array), 0), &value, 1);
}

void CCArraySetValueAtIndex(CFMutableArrayRef array, uint32_t idx, const void *value) {
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.setObject, value, idx);
    CF_OBJC_FUNCDISPATCHV(CCArrayGetTypeID(), void, (NSMutableArray *)array, setObject:(id)value atIndex:(NSUInteger)idx);
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCCArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert2(0 <= idx && idx <= __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index (%ld) out of bounds", __PRETTY_FUNCTION__, idx);
    CHECK_FOR_MUTATION(array);
    if (idx == __CCArrayGetCount(array)) {
        _CCArrayReplaceValues(array, CFRangeMake(idx, 0), &value, 1);
    } else {
        BEGIN_MUTATION(array);
        const void *old_value;
        const CCBaseCallBacks *cb = __CCArrayGetCallBacks(array);
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



void CCArrayInsertValueAtIndex(CFMutableArrayRef array, uint32_t idx, const void *value) {
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.insertObject, idx, value);
    CF_OBJC_FUNCDISPATCHV(CCArrayGetTypeID(), void, (NSMutableArray *)array, insertObject:(id)value atIndex:(NSUInteger)idx);
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCCArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert2(0 <= idx && idx <= __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index (%ld) out of bounds", __PRETTY_FUNCTION__, idx);
    CHECK_FOR_MUTATION(array);
    _CCArrayReplaceValues(array, CFRangeMake(idx, 0), &value, 1);
}

// NB: AddressBook on the Phone is a fragile flower, so this function cannot do anything
// that causes the values to be retained or released.
void CCArrayExchangeValuesAtIndices(CFMutableArrayRef array, uint32_t idx1, uint32_t idx2) {
    const void *tmp;
    struct __CCArrayBucket *bucket1, *bucket2;
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.exchangeObjectAtIndex, idx1, idx2);
    CF_OBJC_FUNCDISPATCHV(CCArrayGetTypeID(), void, (NSMutableArray *)array, exchangeObjectAtIndex:(NSUInteger)idx1 withObjectAtIndex:(NSUInteger)idx2);
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CFAssert2(0 <= idx1 && idx1 < __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index #1 (%ld) out of bounds", __PRETTY_FUNCTION__, idx1);
    CFAssert2(0 <= idx2 && idx2 < __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index #2 (%ld) out of bounds", __PRETTY_FUNCTION__, idx2);
    CFAssert1(__CCArrayGetType(array) != __kCCArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
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

void CCArrayRemoveValueAtIndex(CFMutableArrayRef array, uint32_t idx) {
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.removeObjectAtIndex, idx);
    CF_OBJC_FUNCDISPATCHV(CCArrayGetTypeID(), void, (NSMutableArray *)array, removeObjectAtIndex:(NSUInteger)idx);
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCCArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert2(0 <= idx && idx < __CCArrayGetCount(array), __kCFLogAssertion, "%s(): index (%ld) out of bounds", __PRETTY_FUNCTION__, idx);
    CHECK_FOR_MUTATION(array);
    _CCArrayReplaceValues(array, CFRangeMake(idx, 1), NULL, 0);
}

void CCArrayRemoveAllValues(CFMutableArrayRef array) {
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.removeAllObjects);
    CF_OBJC_FUNCDISPATCHV(CCArrayGetTypeID(), void, (NSMutableArray *)array, removeAllObjects);
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCCArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
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
        uint32_t capacity = __CCArrayDequeRoundUpCapacity(futureCnt + wiggle);
        uint32_t size = sizeof(struct __CCArrayDeque) + capacity * sizeof(struct __CCArrayBucket);
        CFAllocatorRef allocator = __CFGetAllocator(array);
        struct __CCArrayDeque *newDeque = (struct __CCArrayDeque *)CFAllocatorAllocate(allocator, size, 0);
        if (__CFOASafe) __CFSetLastAllocationEventName(newDeque, "CCArray (store-deque)");
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
    CFStringRef msg = CFStringCreateWithFormat(kCFAllocatorSystemDefault, NULL, CFSTR("Attempt to allocate %ld bytes for CCArray failed"), numBytes);
    {
        CFLog(kCFLogLevelCritical, CFSTR("%@"), msg);
        HALT;
    }
    CFRelease(msg);
}

// This function is for Foundation's benefit; no one else should use it.
void _CCArraySetCapacity(CFMutableArrayRef array, uint32_t cap) {
    if (CF_IS_OBJC(CCArrayGetTypeID(), array) || CF_IS_SWIFT(CCArrayGetTypeID(), array)) return;
    __CFGenericValidateType(array, CCArrayGetTypeID());
    CFAssert1(__CCArrayGetType(array) != __kCCArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert3(__CCArrayGetCount(array) <= cap, __kCFLogAssertion, "%s(): desired capacity (%ld) is less than count (%ld)", __PRETTY_FUNCTION__, cap, __CCArrayGetCount(array));
    CHECK_FOR_MUTATION(array);
    BEGIN_MUTATION(array);
    // Currently, attempting to set the capacity of an array which is the CFStorage
    // variant, or set the capacity larger than __CF_MAX_BUCKETS_PER_DEQUE, has no
    // effect.  The primary purpose of this API is to help avoid a bunch of the
    // resizes at the small capacities 4, 8, 16, etc.
    if (__CCArrayGetType(array) == __kCCArrayDeque) {
        struct __CCArrayDeque *deque = (struct __CCArrayDeque *)array->_store;
        uint32_t capacity = __CCArrayDequeRoundUpCapacity(cap);
        uint32_t size = sizeof(struct __CCArrayDeque) + capacity * sizeof(struct __CCArrayBucket);
        CFAllocatorRef allocator = __CFGetAllocator(array);
        if (NULL == deque) {
            deque = (struct __CCArrayDeque *)CFAllocatorAllocate(allocator, size, 0);
            if (NULL == deque) __CCArrayHandleOutOfMemory(array, size);
            if (__CFOASafe) __CFSetLastAllocationEventName(deque, "CCArray (store-deque)");
            deque->_leftIdx = capacity / 2;
        } else {
            struct __CCArrayDeque *olddeque = deque;
            uint32_t oldcap = deque->_capacity;
            deque = (struct __CCArrayDeque *)CFAllocatorAllocate(allocator, size, 0);
            if (NULL == deque) __CCArrayHandleOutOfMemory(array, size);
            memmove(deque, olddeque, sizeof(struct __CCArrayDeque) + oldcap * sizeof(struct __CCArrayBucket));
            CFAllocatorDeallocate(allocator, olddeque);
            if (__CFOASafe) __CFSetLastAllocationEventName(deque, "CCArray (store-deque)");
        }
        deque->_capacity = capacity;
        array->_store = deque;
    }
    END_MUTATION(array);
}


void CCArrayReplaceValues(CFMutableArrayRef array, CFRange range, const void **newValues, uint32_t newCount) {
    CF_SWIFT_FUNCDISPATCHV(CCArrayGetTypeID(), void, (CFSwiftRef)array, NSMutableArray.replaceObjectsInRange, range, newValues, newCount);
    CF_OBJC_FUNCDISPATCHV(CCArrayGetTypeID(), void, (NSMutableArray *)array, replaceObjectsInRange:NSMakeRange(range.location, range.length) withObjects:(id *)newValues count:(NSUInteger)newCount);
    __CFGenericValidateType(array, CCArrayGetTypeID());
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(__CCArrayGetType(array) != __kCCArrayImmutable, __kCFLogAssertion, "%s(): array is immutable", __PRETTY_FUNCTION__);
    CFAssert2(0 <= newCount, __kCFLogAssertion, "%s(): newCount (%ld) cannot be less than zero", __PRETTY_FUNCTION__, newCount);
    CHECK_FOR_MUTATION(array);
    return _CCArrayReplaceValues(array, range, newValues, newCount);
}

// This function does no ObjC dispatch or argument checking;
// It should only be called from places where that dispatch and check has already been done, or NSCCArray
void _CCArrayReplaceValues(CFMutableArrayRef array, CFRange range, const void **newValues, uint32_t newCount) {
    CHECK_FOR_MUTATION(array);
    BEGIN_MUTATION(array);
    const CCBaseCallBacks *cb;
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
        if (newv != buffer && __CFOASafe) __CFSetLastAllocationEventName(newv, "CCArray (temp)");
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
            uint32_t capacity = __CCArrayDequeRoundUpCapacity(futureCnt);
            uint32_t size = sizeof(struct __CCArrayDeque) + capacity * sizeof(struct __CCArrayBucket);
            deque = (struct __CCArrayDeque *)CFAllocatorAllocate((allocator), size, 0);
            if (__CFOASafe) __CFSetLastAllocationEventName(deque, "CCArray (store-deque)");
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

static inline void __CFZSort(CFMutableArrayRef array, CFRange range, CFComparatorFunction comparator, void *context) {
    uint32_t cnt = range.length;
    while (1 < cnt) {
        for (uint32_t idx = range.location; idx < range.location + cnt - 1; idx++) {
            const void *a = CCArrayGetValueAtIndex(array, idx);
            const void *b = CCArrayGetValueAtIndex(array, idx + 1);
            if ((CFComparisonResult)(INVOKE_CALLBACK3(comparator, b, a, context)) < 0) {
                CCArrayExchangeValuesAtIndices(array, idx, idx + 1);
            }
        }
        cnt--;
    }
}

CF_PRIVATE void _CCArraySortValues(CFMutableArrayRef array, CFComparatorFunction comparator, void *context) {
    CFRange range = {0, CCArrayGetCount(array)};
    if (range.length < 2) {
        return;
    }
    // implemented abstractly, careful!
    const void **values, *buffer[256];
    values = (range.length <= 256) ? (const void **)buffer : (const void **)CFAllocatorAllocate(kCFAllocatorSystemDefault, range.length * sizeof(void *), 0);
    CCArrayGetValues(array, range, values);
    struct _acompareContext ctx;
    ctx.func = comparator;
    ctx.context = context;
    CFQSortArray(values, range.length, sizeof(void *), (CFComparatorFunction)__CCArrayCompareValues, &ctx);
    CCArrayReplaceValues(array, range, values, range.length);
    if (values != buffer) CFAllocatorDeallocate(kCFAllocatorSystemDefault, values);
}

void CCArraySortValues(CFMutableArrayRef array, CFRange range, CFComparatorFunction comparator, void *context) {
    FAULT_CALLBACK((void **)&(comparator));
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(NULL != comparator, __kCFLogAssertion, "%s(): pointer to comparator function may not be NULL", __PRETTY_FUNCTION__);
    Boolean immutable = false;
    if (CF_IS_OBJC(CCArrayGetTypeID(), array) || CF_IS_SWIFT(CCArrayGetTypeID(), array)) {
        BOOL result;
        result = CF_OBJC_CALLV((NSMutableArray *)array, isKindOfClass:[NSMutableArray class]); // TODO: Fixme for swift (we need a isKindOfClass replacement: (array as? NSMutableArray) != nil)
        immutable = !result;
    } else if (__kCCArrayImmutable == __CCArrayGetType(array)) {
        immutable = true;
    }
    const CCBaseCallBacks *cb = NULL;
    if (CF_IS_OBJC(CCArrayGetTypeID(), array) || CF_IS_SWIFT(CCArrayGetTypeID(), array)) {
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
    CCArrayGetValues(array, range, values);
    struct _acompareContext ctx;
    ctx.func = comparator;
    ctx.context = context;
    CFQSortArray(values, range.length, sizeof(void *), (CFComparatorFunction)__CCArrayCompareValues, &ctx);
    if (!immutable) CCArrayReplaceValues(array, range, values, range.length);
    if (values != buffer) CFAllocatorDeallocate(kCFAllocatorSystemDefault, values);
}

uint32_t CCArrayBSearchValues(CCArrayNonnullPtr array, CFRange range, const void *value, CFComparatorFunction comparator, void *context) {
    FAULT_CALLBACK((void **)&(comparator));
    __CCArrayValidateRange(array, range, __PRETTY_FUNCTION__);
    CFAssert1(NULL != comparator, __kCFLogAssertion, "%s(): pointer to comparator function may not be NULL", __PRETTY_FUNCTION__);
    // implemented abstractly, careful!
    if (range.length <= 0) return range.location;
    const void *item = CCArrayGetValueAtIndex(array, range.location + range.length - 1);
    if ((CFComparisonResult)(INVOKE_CALLBACK3(comparator, item, value, context)) < 0) {
        return range.location + range.length;
    }
    item = CCArrayGetValueAtIndex(array, range.location);
    if ((CFComparisonResult)(INVOKE_CALLBACK3(comparator, value, item, context)) < 0) {
        return range.location;
    }
    SInt32 lg = flsl(range.length) - 1;    // lg2(range.length)
    item = CCArrayGetValueAtIndex(array, range.location + -1 + (1 << lg));
    // idx will be the current probe index into the range
    uint32_t idx = (comparator(item, value, context) < 0) ? range.length - (1 << lg) : -1;
    while (lg--) {
        item = CCArrayGetValueAtIndex(array, range.location + idx + (1 << lg));
        if (comparator(item, value, context) < 0) {
            idx += (1 << lg);
        }
    }
    idx++;
    return idx + range.location;
}

void CCArrayAppendArray(CFMutableArrayRef array, CCArrayNonnullPtr otherArray, CFRange otherRange) {
    __CCArrayValidateRange(otherArray, otherRange, __PRETTY_FUNCTION__);
    // implemented abstractly, careful!
    for (uint32_t idx = otherRange.location; idx < otherRange.location + otherRange.length; idx++) {
        CCArrayAppendValue(array, CCArrayGetValueAtIndex(otherArray, idx));
    }
}


