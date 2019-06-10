//
//  CEThreadBase.h
//  CoreEvent
//
//  Created by vector on 2019/5/7.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEThreadBase_h
#define CEThreadBase_h

#include "CEBase.h"
#include <pthread.h>
#include "CERuntime.h"
#include "CEParam.h"

#if __APPLE__
typedef os_unfair_lock CESpinLock_t;
#else
typedef pthread_spinlock_t CESpinLock_t;
#endif

typedef uint8_t CEQueuePriority_t;
static const CEQueuePriority_t CEQueuePriorityHigh = 192;
static const CEQueuePriority_t CEQueuePriorityDefault = 128;
static const CEQueuePriority_t CEQueuePriorityLow = 64;

typedef CERef CESyncWaiterRef;

struct _CESem;
typedef struct _CESem CESem_s;
typedef CESem_s * CESemPtr;

//typedef struct _CEConditionContext {
//
//} CEConditionContext_s;

typedef struct _CEConditionLock {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    long value;
} CEConditionLock_s;
typedef CEConditionLock_s * CEConditionLockPtr;

typedef struct _CECondition {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} CECondition_s;
typedef CECondition_s * CEConditionPtr;


typedef CERef CEThreadRef;

struct _CERunLoop;
typedef struct _CERunLoop CERunLoop_s;
typedef CERunLoop_s * CERunLoopRef;

struct _CETaskWorker;
typedef struct _CETaskWorker CETaskWorker_s;
typedef CETaskWorker_s * CETaskWorkerRef;

typedef CERef CEQueueRef;

struct _CESource;
typedef struct _CESource CESource_s;
typedef CESource_s * CESourceRef;


struct _CETaskExecuteContext;
typedef struct _CETaskExecuteContext CETaskExecuteContext_s;
typedef CETaskExecuteContext_s * CETaskExecuteContextRef;

struct _CETaskExecuteContext {
    CEParamRef _Nullable * _Nullable resultReceiver;
    CEParamType_e * _Nullable paramTypes;
    CEParamItemValue_u * _Nullable paramItems;
    uint32_t paramCount;
};

typedef uint64_t CETaskType_e;
static const CETaskType_e CETaskTypeAsync = 0;
static const CETaskType_e CETaskTypeSync = 1;

//Source 状态
struct _CETaskExecuteObserver;
typedef struct _CETaskExecuteObserver CETaskExecuteObserver_s;
typedef CETaskExecuteObserver_s * CETaskExecuteObserverRef;


typedef void (*CETaskExecuteObserverFinish_f)(CETaskExecuteObserverRef _Nonnull observer);

struct _CETaskResult {
    uint32_t paramCount;
    
};

//struct _CETaskContext {
//    CEParamRef _Nullable * _Nullable resultReceiver;
//    CEParamType_e * _Nullable paramTypes;
//    CEParamItemValue_u * _Nullable paramItems;
//    uint32_t paramCount;
//};

struct _CETaskExecuteObserver {
    CETaskExecuteObserverFinish_f _Nonnull finish;
    void * _Nullable context;
};


struct _CETaskContext;
typedef struct _CETaskContext CETaskContext_s;
typedef CETaskContext_s * CETaskContextPtr;


/*
 CEQueueRef _Nonnull targetQueue;
 CETaskExecuteObserverRef _Nullable observer;
*/
struct _CESyncTask {
    CERuntimeBase_s runtime;
//    CETaskBase_t base;
    CETaskContextPtr _Nonnull context;
    CEQueueRef _Nullable sourceQueue;
};




typedef struct _CEThreadConfig {
    char name[64];
    uint32_t stacksize;//0 use default
    float schedPriority;//[-1, 1]
} CEThreadConfig_s;


void CESpinLockInit(CESpinLock_t * _Nonnull lockPtr);
void CESpinLockDeinit(CESpinLock_t * _Nonnull lockPtr);

void CESpinLockLock(CESpinLock_t * _Nonnull lockPtr);
void CESpinLockUnlock(CESpinLock_t * _Nonnull lockPtr);



#endif /* CEThreadBase_h */
