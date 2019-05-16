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

#define CETaskParamItemBufferSize 16


struct _CESem;
typedef struct _CESem CESem_s;
typedef CESem_s * CESemRef;


struct _CEThread;
typedef struct _CEThread CEThread_s;
typedef CEThread_s * CEThreadRef;

struct _CERunLoop;
typedef struct _CERunLoop CERunLoop_s;
typedef CERunLoop_s * CERunLoopRef;


struct _CEThreadLoader;
typedef struct _CEThreadLoader CEThreadLoader_s;
typedef CEThreadLoader_s * CEThreadLoaderRef;

struct _CETaskWorker;
typedef struct _CETaskWorker CETaskWorker_s;
typedef CETaskWorker_s * CETaskWorkerRef;



struct _CEQueue;
typedef struct _CEQueue CEQueue_s;
typedef CEQueue_s * CEQueueRef;

struct _CESource;
typedef struct _CESource CESource_s;
typedef CESource_s * CESourceRef;


struct _CEThreadSpecific;
typedef struct _CEThreadSpecific CEThreadSpecific_s;
typedef CEThreadSpecific_s * CEThreadSpecificRef;



struct _CETaskContext;
typedef struct _CETaskContext CETaskContext_s;
typedef CETaskContext_s * CETaskContextRef;



struct _CETaskExecuteContext;
typedef struct _CETaskExecuteContext CETaskExecuteContext_s;
typedef CETaskExecuteContext_s * CETaskExecuteContextRef;

struct _CETaskExecuteContext {
    CEParamRef _Nullable * _Nullable resultReceiver;
    CEParamType_e * _Nullable paramTypes;
    CEParamItemValue_u * _Nullable paramItems;
    uint32_t paramCount;
};

struct _CETask;
typedef struct _CETask CETask_s;
typedef CETask_s * CETaskRef;
typedef CETaskRef CEEscapingTaskRef;

struct _CESyncTask;
typedef struct _CESyncTask CENoescapingTask_s;
typedef CENoescapingTask_s * CENoescapingTaskRef;

typedef void (*CETaskExecute_f)(CETaskExecuteContextRef _Nonnull context);
typedef void (*CETaskRelease_f)(CETaskRef _Nonnull task);


typedef uint64_t CETaskType_e;
static const CETaskType_e CETaskTypeAsync = 0;
static const CETaskType_e CETaskTypeSync = 1;

//Source 状态
struct _CETaskExecuteObserver;
typedef struct _CETaskExecuteObserver CETaskExecuteObserver_s;
typedef CETaskExecuteObserver_s * CETaskExecuteObserverRef;


typedef CEParamRef _Nullable * _Nullable (*CETaskExecuteObserverGetResultReceiver_f)(CETaskExecuteObserverRef _Nonnull observer);
typedef void (*CETaskExecuteObserverFinish_f)(CETaskExecuteObserverRef _Nonnull observer);

struct _CETaskResult {
    uint32_t paramCount;
    
};

struct _CETaskContext {
    CEParamRef _Nullable * _Nullable resultReceiver;
    CEParamType_e * _Nullable paramTypes;
    CEParamItemValue_u * _Nullable paramItems;
    uint32_t paramCount;
};

struct _CETaskExecuteObserver {
    CETaskExecuteObserverGetResultReceiver_f _Nullable getResultReceiver;
    CETaskExecuteObserverFinish_f _Nullable finish;
};

typedef struct _CETaskBase {
    uint64_t tid: 56;
    uint64_t type: 3;
    uint64_t paramItemCount: 5;//入参个数
    void * _Nullable relatedTask;//CENoescapingTaskRef, 同步任务的上下文中用到
    
    CETaskRelease_f _Nonnull release;
    CETaskExecute_f _Nonnull execute;
    CETaskExecuteObserverRef _Nullable observer;
} CETaskBase_t;


struct _CESyncTask {
//    CEObjectRuntimeBase_t runtime;
    CETaskBase_t base;
    CETaskContextRef _Nonnull context;
    CEQueueRef _Nullable sourceQueue;
    CEParamType_e paramTypes[CETaskParamItemBufferSize];
    CEParamItemValue_u paramItems[CETaskParamItemBufferSize];
};


#if __APPLE__
typedef os_unfair_lock CESpinLock_t;
#else
typedef pthread_spinlock_t CESpinLock_t;
#endif

void CESpinLockInit(CESpinLock_t * _Nonnull lockPtr);
void CESpinLockDeinit(CESpinLock_t * _Nonnull lockPtr);

void CESpinLockLock(CESpinLock_t * _Nonnull lockPtr);
void CESpinLockUnlock(CESpinLock_t * _Nonnull lockPtr);



#endif /* CEThreadBase_h */
