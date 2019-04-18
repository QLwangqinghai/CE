//
//  CEClosur.c
//  CoreEvent
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEClosure.h"
#include "CEMemory.h"


uintptr_t CEClosureInit(CEClosureExecute_f _Nonnull execute,
                        CEClosureClearData_f _Nullable clearData,
                        const void * _Nullable data,
                        size_t dataSize,
                        size_t paramsSize,
                        void * _Nullable resultReceiver,
                        size_t resultSize,
                        CEThreadWaiter_s * _Nullable waiter) {
    
    if (NULL == execute) {
        return (uintptr_t)NULL;
    }
    if (dataSize < 0 || dataSize > 0x40000000 || paramsSize < 0 || paramsSize > 0x40000000 || resultSize < 0 || resultSize > 0x40000000) {
        return (uintptr_t)NULL;
    }
    if (dataSize != 0) {
        if (NULL == data) {
            return (uintptr_t)NULL;
        }
    }
    if (resultSize != 0) {
        if (NULL == resultReceiver || NULL == waiter) {
            return (uintptr_t)NULL;
        }
    }


    CEClosure_s entity = {
//        .waiter = waiter,
//        .execute = execute,
//        .clearData = clearData,
//        .dataSize = (uint32_t)dataSize,
//        .paramsSize = (uint32_t)paramsSize,
//        .resultSize = (uint32_t)resultSize,
    };
    
    size_t bCount = CEAtomicMemoryBlockCountFromSize(sizeof(CEClosure_s));
    size_t bSize = CEAtomicMemoryBlockSize * bCount;
    size_t dCount = CEAtomicMemoryBlockCountFromSize(dataSize);
    size_t dSize = CEAtomicMemoryBlockSize * dCount;
    size_t rCount = CEAtomicMemoryBlockCountFromSize(resultSize);
    size_t rSize = CEAtomicMemoryBlockSize * rCount;
    
    size_t contextSize = dSize + rSize;
    size_t entitySize = bSize + contextSize;
    CEAtomicMemoryBlock_t * entityPtr = (CEAtomicMemoryBlock_t *)CEAllocate(entitySize);
    CEAtomicMemoryBlock_t * dataPtr = entityPtr + bCount;
    CEAtomicMemoryBlock_t * resultPtr = dataPtr + dCount;
    
    CEAtomicMemoryStore(entityPtr, &entity, sizeof(CEClosure_s));
    CEAtomicMemoryStore(dataPtr, data, dataSize);
    CEAtomicMemorySet(resultPtr, 0, rSize);
    return (uintptr_t)entityPtr;
}


void CEClosureGetInfo(uintptr_t closure, CEClosureInfo_s * _Nonnull infoPtr) {
    CEClosure_s entity = {};
    CEAtomicMemoryBlock_t * entityPtr = (CEAtomicMemoryBlock_t *)closure;
    assert(entityPtr);
    assert(infoPtr);

    CEAtomicMemoryLoad(entityPtr, &entity, sizeof(CEClosure_s));

//    infoPtr->waiter = entity.waiter;
//    infoPtr->execute = entity.execute;
//    infoPtr->clearData = entity.clearData;
//    infoPtr->dataSize = (size_t)entity.dataSize;
//    infoPtr->paramsSize = (size_t)entity.paramsSize;
//    infoPtr->resultSize = (size_t)entity.resultSize;
}


