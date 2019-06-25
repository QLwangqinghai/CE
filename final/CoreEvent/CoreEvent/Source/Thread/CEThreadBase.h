//
//  CEThreadBase.h
//  CoreEvent
//
//  Created by vector on 2019/5/7.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEThreadBase_h
#define CEThreadBase_h



#include <pthread.h>

#include <CoreEvent/CEBase.h>
#include <CoreEvent/CERuntime.h>
#include <CoreEvent/CETaskParam.h>


#if __APPLE__
typedef os_unfair_lock CESpinLock_t;
#else
typedef pthread_spinlock_t CESpinLock_t;
#endif

typedef CESpinLock_t * CESpinLockPtr;

typedef uint16_t CEQueuePriority_t;
static const CEQueuePriority_t CEQueuePriorityHigh = 192;
static const CEQueuePriority_t CEQueuePriorityDefault = 128;
static const CEQueuePriority_t CEQueuePriorityLow = 64;
static const CEQueuePriority_t CEQueuePriorityMain = 255;


typedef uint16_t CEQueueType_t;
static const CEQueueType_t CEQueueTypeMain = 1;
static const CEQueueType_t CEQueueTypeGolbal = 2;
static const CEQueueType_t CEQueueTypeSerial = 3;
static const CEQueueType_t CEQueueTypeConcurrent = 4;


struct _CESem;
typedef struct _CESem CESem_s;
typedef CESem_s * CESemPtr;

//typedef struct _CEConditionContext {
//
//} CEConditionContext_s;

typedef struct _CEConditionLock {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    CEPtr _Nonnull value;
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
//    CType_e * _Nullable paramTypes;
//    CEParamItemValue_u * _Nullable paramItems;
//    uint32_t paramCount;
//};

struct _CETaskExecuteObserver {
    CETaskExecuteObserverFinish_f _Nonnull finish;
    void * _Nullable context;
};

typedef struct _CEThreadConfig {
    char name[64];
    uint32_t stacksize;//0 use default
    float schedPriority;//[-1, 1]
} CEThreadConfig_s;



CESpinLockPtr _Nonnull CESpinLockCreate(void);
void CESpinLockDestroy(CESpinLockPtr _Nonnull lockPtr);

void CESpinLockInit(CESpinLockPtr _Nonnull lockPtr);
void CESpinLockDeinit(CESpinLockPtr _Nonnull lockPtr);

void CESpinLockLock(CESpinLockPtr _Nonnull lockPtr);
void CESpinLockUnlock(CESpinLockPtr _Nonnull lockPtr);



CESemPtr _Nonnull CESemCreate(unsigned int value);
void CESemDestroy(CESemPtr _Nonnull sem);

void CESemWait(CESemPtr _Nonnull sem);
void CESemSignal(CESemPtr _Nonnull sem);

#endif /* CEThreadBase_h */
