//
//  CETaskExecuteObserver.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CETaskExecuteObserver_h
#define CETaskExecuteObserver_h

#include <stdio.h>

#include "CEBase.h"
#include "CESem.h"
#include "CEParam.h"

struct _CETaskExecuteObserver;
typedef struct _CETaskExecuteObserver CETaskExecuteObserver_s;
typedef CETaskExecuteObserver_s * CETaskExecuteObserverRef;


typedef CEParamRef _Nullable * _Nullable (*CETaskExecuteObserverGetResultReceiver_f)(CETaskExecuteObserverRef _Nonnull observer);
typedef void (*CETaskExecuteObserverFinish_f)(CETaskExecuteObserverRef _Nonnull observer);


struct _CETaskExecuteObserver {
    CETaskExecuteObserverGetResultReceiver_f _Nullable getResultReceiver;
    CETaskExecuteObserverFinish_f _Nullable finish;
};


//struct _CETaskExecuteObserver {
//    CETaskExecuteObserverGetResultReceiver_f _Nullable getResultReceiver;
//    CETaskExecuteObserverFinish_f _Nullable finish;
//
//    CEParamRef _Nullable * _Nullable resultReceiver;
//    CESemRef _Nullable syncSem;//同步信号量
//};

#endif /* CETaskExecuteObserver_h */
