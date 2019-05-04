//
//  CEOperation.h
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEOperation_h
#define CEOperation_h

#include <stdio.h>


typedef uint32_t CEOperationStatus_e;
static const CEOperationStatus_e CEOperationStatusNone = 0;
static const CEOperationStatus_e CEOperationStatusExecuting = 1;
static const CEOperationStatus_e CEOperationStatusCanceled = 2;
static const CEOperationStatus_e CEOperationStatusFinished = 3;

//typedef struct _CEOperationContext {
//    CEClosureExecute_f _Nonnull execute;
//    CEClosureClearData_f _Nullable clearData;
//    uint32_t dataSize;
//    uint32_t paramsSize;
//    uint32_t resultSize;
//} CEOperationContext_s;



typedef uint32_t CEOperationParamItemType_e;
static const CEOperationParamItemType_e CEOperationParamItemTypeSInt8 = 1;
static const CEOperationParamItemType_e CEOperationParamItemTypeUInt8 = 2;
static const CEOperationParamItemType_e CEOperationParamItemTypeSInt16 = 3;
static const CEOperationParamItemType_e CEOperationParamItemTypeUInt16 = 4;
static const CEOperationParamItemType_e CEOperationParamItemTypeSInt32 = 5;
static const CEOperationParamItemType_e CEOperationParamItemTypeUInt32 = 6;
static const CEOperationParamItemType_e CEOperationParamItemTypeSInt64 = 7;
static const CEOperationParamItemType_e CEOperationParamItemTypeUInt64 = 8;
static const CEOperationParamItemType_e CEOperationParamItemTypeFloat = 9;
static const CEOperationParamItemType_e CEOperationParamItemTypeDouble = 10;
static const CEOperationParamItemType_e CEOperationParamItemTypeStruct = 11;
static const CEOperationParamItemType_e CEOperationParamItemTypePtr = 12;


struct _CEOperationParam;
typedef struct _CEOperationParam CEOperationParam_s;
typedef CEOperationParam_s * CEOperationParamRef;

struct _CEOperation;
typedef struct _CEOperation CEOperation_s;
typedef CEOperation_s * CEOperationRef;





typedef void (*CEOperationParamClear_f)(CEOperationParamRef _Nonnull param);


struct _CEOperationParam {
    uint32_t layoutSize;
    uint32_t contentSize;
    uint32_t count;//参数个数
    uint32_t paramsBufferSize;//入参buffer size
    uint32_t paramsSizeOffset;//入参size偏移量
    
    uint32_t resultsOffset;//出参偏移量
    uint32_t resultsBufferSize;//出参buffer size
    uint32_t resultSizeOffset;//出参size偏移量
    uint8_t content[];
};

typedef void (*CEOperationExecute_f)(uintptr_t closure, const void * _Nullable params);
typedef void (*CEOperationClearData_f)(uintptr_t closure, const void * _Nullable data, size_t dataSize);


typedef struct _CEOperationType {
    CEOperationExecute_f _Nonnull execute;
} CEOperationType_s;

struct _CEOperation {
    const CEOperationType_s * _Nonnull type;
    
    CEOperationParamRef _Nullable in;
    CEOperationParamRef _Nullable out;
};



//typedef struct _CEBlockStorage {
//    //    _Atomic(uintptr_t) execute;
//    //    _Atomic(uintptr_t) executeResult;
//    //    _Atomic(uintptr_t) dealloc;
//
//    CEClosureExecute_f _Nonnull execute;
//    CEClosureExecute_f _Nonnull executeResult;
//    CEClosureClearData_f _Nullable dealloc;
//    _Atomic(uint_fast32_t) contextSize;
//    _Atomic(uint_fast32_t) resultSize;
//    CEAtomicMemoryBlock_t context[0];
//} CEBlockStorage_s;

//typedef struct _CEClosureType {
//    CEClosureExecute_f _Nonnull execute;
//    CEClosureClearData_f _Nullable clearData;
//    uint32_t dataSize;
//    uint32_t paramsSize;
//    uint32_t resultSize;
//} CEClosureType_s;
//
////typedef struct _CEClosure {
////    CEThreadWaiter_s * _Nullable waiter;
////    CEClosureExecute_f _Nonnull execute;
////    CEClosureClearData_f _Nullable clearData;
////    uint32_t dataSize;
////    uint32_t paramsSize;
////    uint32_t resultSize;
////} CEClosure_s;
//
//typedef struct _CEClosure {
//    const CEClosureType_s * _Nonnull type;
//    const CEThreadWaiter_s * _Nullable waiter;
//    uint8_t content[];
//} CEClosure_s;



#endif /* CEOperation_h */
