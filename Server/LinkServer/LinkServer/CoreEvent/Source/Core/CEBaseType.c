//
//  CEBaseType.c
//  CoreEvent
//
//  Created by CaiLianfeng on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#include "CEBaseType.h"
#include <CCFoundation/CCAtomic.h>


const CEFileEventMask_es CEFileEventMaskNone = 0;
const CEFileEventMask_es CEFileEventMaskReadable = 1;
const CEFileEventMask_es CEFileEventMaskWritable = 2;

const CEFileEventMask_es CEFileEventMaskReadWritable = 3;


#if __LLP64__ || __LP64__
#define CEAtomicMemoryBlockSizeUse64 1
#else
#define CEAtomicMemoryBlockSizeUse64 0
#endif

#if CEAtomicMemoryBlockSizeUse64

typedef uint64_t CEMemoryBlock_t;
typedef _Atomic(uint_fast64_t) CEAtomicMemoryBlock_t;
typedef uint64_t CEAtomicMemoryBlockBridgeType_t;
#define CEMemoryBlockSize (8)
#define CEAtomicMemoryBlockSize (8)
#define CEAtomicMemoryBlockSizeShift (3)

#else

typedef uint32_t CEMemoryBlock_t;
typedef _Atomic(uint_fast32_t) CEAtomicMemoryBlock_t;
typedef uint32_t CEAtomicMemoryBlockBridgeType_t;
#define CEMemoryBlockSize (4)
#define CEAtomicMemoryBlockSize (4)
#define CEAtomicMemoryBlockSizeShift 2

#endif

static inline size_t CEMemoryBlockCountFromSize(size_t size) {
    return (size <= CEMemoryBlockSize) ? CEMemoryBlockSize
    : (((size + CEMemoryBlockSize - 1) >> CEAtomicMemoryBlockSizeShift) << CEAtomicMemoryBlockSizeShift);
}
static inline size_t CEAtomicMemoryBlockCountFromSize(size_t size) {
    size_t result = size / CEAtomicMemoryBlockSize;
    if (size % CEAtomicMemoryBlockSize != 0) {
        result += 1;
    }
    return result;
}
static inline void CEAtomicMemoryStore(CEAtomicMemoryBlock_t * _Nonnull dst, const void * _Nonnull src, size_t srcSize) {

    size_t count = srcSize / CEAtomicMemoryBlockSize;
    const CEAtomicMemoryBlockBridgeType_t * tmp = (const CEAtomicMemoryBlockBridgeType_t *)src;
    for (size_t i=0; i<count; i++) {
        atomic_store(dst, *tmp);
        dst ++;
        tmp ++;
    }
    
    size_t remain = srcSize % CEAtomicMemoryBlockSize;
    if (remain != 0) {
        CEAtomicMemoryBlockBridgeType_t t = 0;
        memcpy(&t, tmp, remain);
        atomic_store(dst, t);
    }
}
static inline void CEAtomicMemoryLoad(CEAtomicMemoryBlock_t * _Nonnull src, void * _Nonnull dst, size_t dstSize) {
    
    size_t count = dstSize / CEAtomicMemoryBlockSize;
    CEAtomicMemoryBlockBridgeType_t * tmp = (CEAtomicMemoryBlockBridgeType_t *)dst;
    for (size_t i=0; i<count; i++) {
        *tmp = atomic_load(src);
        src ++;
        tmp ++;
    }
    
    size_t remain = dstSize % CEAtomicMemoryBlockSize;
    if (remain != 0) {
        CEAtomicMemoryBlockBridgeType_t t = atomic_load(src);
        memcpy(tmp, &t, remain);
    }
}
static inline void CEAtomicMemorySet(CEAtomicMemoryBlock_t * _Nonnull dst, CEAtomicMemoryBlockBridgeType_t src, size_t size) {
    size_t count = size / CEAtomicMemoryBlockSize;
    for (size_t i=0; i<count; i++) {
        atomic_store(dst, src);
        dst ++;
    }
    size_t remain = size % CEAtomicMemoryBlockSize;
    if (remain != 0) {
        atomic_store(dst, src);
    }
}
