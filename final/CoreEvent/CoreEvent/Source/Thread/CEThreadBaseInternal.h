//
//  CEThreadBaseInternal.h
//  CoreEvent
//
//  Created by vector on 2019/5/8.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEThreadBaseInternal_h
#define CEThreadBaseInternal_h

#include "CEThreadBase.h"

struct _CETaskSyncContext;
typedef struct _CETaskSyncContext CETaskSyncContext_t;
typedef CETaskSyncContext_t * CETaskSyncContextPtr;

typedef struct _CEQueueBase {
    char * _Nonnull label;
    size_t maxConcurrentCount;//[1, ...], 取值为1 时 为串行队列
} CEQueueBase_t;

typedef CEQueueBase_t * CEQueueBaseRef;

struct _CEThread {
#if __APPLE__
    pthread_t _Nullable pthread;
#else
    pthread_t pthread;
#endif
    size_t nameBufferLength;//==0时 为静态数据
    char * _Nonnull name;
    CEQueueRef _Nullable queue;
};

struct _CERunLoop {
    pthread_t _Nullable thread;
};

struct _CEThreadLoader {
    CEThreadRef _Nonnull thread;
    CEThreadStatus_t status;
    
    CERunLoopRef _Nonnull (* _Nullable loadRunLoop)(CEThreadRef _Nonnull);
    CERunLoopRef _Nonnull runLoop;
};

struct _CETaskWorker {
    CEThreadRef _Nonnull thread;
    CEQueueRef _Nonnull queue;
    void * owmerQueue;
    CESemRef _Nonnull sem;
};

struct _CEThreadSpecific {
    CEThread_s thread;
    CEThreadLoaderRef _Nullable loader;
    CETaskWorkerRef _Nullable worker;
    
    CEQueueBaseRef _Nullable queue;
    CETaskBase_t * _Nullable task;
    CETaskSyncContextPtr _Nullable syncContext;
    
};





typedef struct _CETaskSyncContextItem {
    void * _Nonnull task;
    CEQueueBaseRef _Nonnull queue;
} CETaskSyncContextItem_t;

typedef struct _CETaskSyncContext {
    void * _Nonnull owner;
    size_t blockingQueueCount;
    CETaskSyncContextItem_t blocking[16];
    CETaskSyncContextItem_t * _Nullable moreBlocking;
    CETaskSyncContextItem_t current;
} CETaskSyncContext_t;

typedef CETaskSyncContext_t * CETaskSyncContextPtr;


typedef struct _CETaskContext {
    struct _CETaskContext * _Nullable prev;


    CEQueueBaseRef _Nullable queue;
    CEThreadRef _Nonnull thread;

    CEParamRef _Nullable param;
    CEParamRef _Nullable resultReceiver;

} CETaskContext_s;
typedef CETaskContext_s * CETaskContextPtr;




extern CEThreadSpecificRef _Nonnull _CEThreadSpecificGetCurrent(char * _Nullable threadName, _Bool copyThreadName);

extern CEThreadSpecificRef _Nonnull CEThreadSpecificGetCurrent(void);

#endif /* CEThreadBaseInternal_h */
