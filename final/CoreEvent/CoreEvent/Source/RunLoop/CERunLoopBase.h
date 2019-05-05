//
//  CERunLoopBase.h
//  CoreEvent
//
//  Created by vector on 2019/5/6.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CERunLoopBase_h
#define CERunLoopBase_h

#include "CEBase.h"

struct _CERunLoop;
typedef struct _CERunLoop CERunLoop_s;
typedef CERunLoop_s * CERunLoopRef;

struct _CEThread;
typedef struct _CEThread CEThread_s;
typedef CEThread_s * CEThreadRef;

struct _CETaskWorker;
typedef struct _CETaskWorker CETaskWorker_s;
typedef CETaskWorker_s * CETaskWorkerRef;

struct _CESem;
typedef struct _CESem CESem_s;
typedef CESem_s * CESemRef;

struct _CEQueue;
typedef struct _CEQueue CEQueue_s;
typedef CEQueue_s * CEQueueRef;

struct _CESource;
typedef struct _CESource CESource_s;
typedef CESource_s * CESourceRef;



struct _CEThread {
#if __APPLE__
    pthread_t _Nullable pthread;
#else
    pthread_t pthread;
#endif
    CEThreadStatus_t status;
    
    CERunLoopRef _Nonnull (* _Nullable runLoopLoader)(CEThreadRef _Nonnull);
    
    CERunLoopRef _Nullable runLoop;
    
    
};




struct _CETaskWorker {
#if __APPLE__
    pthread_t _Nullable pthread;
#else
    pthread_t pthread;
#endif
    
    CESemRef _Nonnull sem;
};

struct _CERunLoop {
    pthread_t _Nullable thread;
};

struct _CEQueue {
    char * _Nonnull label;
    CESourceRef _Nonnull source;
    CEThreadRef _Nonnull thread;
    
};


//高1 位 表示执行中的count， 其余为任务个数 running = countInfos != 0

struct _CESource {
#if CEBuild64Bit
    //高14 位 表示执行中的count， 其余为任务个数
    _Atomic(uint_fast64_t) countInfos;// 0->1 do wake; 1->0 do wait
#else
    //高8 位 表示执行中的count， 其余为任务个数
    _Atomic(uint_fast32_t) countInfos;// 0->1 do wake; 1->0 do wait
#endif
    uint32_t concurrency: 7;//[1-127]
    uint32_t concurrencyBitCount: 3;//[1-255]

    
    
};



#endif /* CERunLoopBase_h */
