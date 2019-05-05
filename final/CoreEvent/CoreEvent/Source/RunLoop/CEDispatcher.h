//
//  CEDispatcher.h
//  CoreEvent
//
//  Created by vector on 2019/5/3.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEDispatcher_h
#define CEDispatcher_h

#include "CERunLoopBase.h"



///* State of an event based program */
//struct _CEDispatcher {
//    _Atomic(uintptr_t) runInThread;
//    _Atomic(uint_fast32_t) runningStates;// 0 is stopped , 1 running, 2 is will stopped
//    CERunLoopProgress_t progress;//
//    CEThread_s thread;
//    
//    int maxfd;   /* highest file descriptor currently registered */
//    int maxIndex;
//    unsigned int setsize; /* max number of file descriptors tracked */
//    int firedCount;
//    
//#if __APPLE__
//    os_unfair_lock blockQueueLock;
//#else
//    pthread_spinlock_t blockQueueLock;
//#endif
//    uint32_t blockEvent;
//    uint32_t timerFiredIndex;//source timer 的 游标
//    uint32_t fdTagSequence;
//    uint32_t xxxx;
//    CEBlockQueue_s blockQueue;
//    
//    CETimeEventManager_s timeEventManager;
//    
//    uint64_t microsecondsTime;//单位 微秒
//    uint64_t fileTimerSeconds8;//单位为(1/8)秒
//    
//    CEFileEvent_s * _Nullable fileEventsPages[32768]; /* Registered events, 0x10000 count peer page */
//    CEFiredEvent_s * _Nullable firedPages[32768]; /* Fired events, 0x10000 count peer page  */
//    
//    int readTimerPages[CERunLoopFileTimerPageSize];
//    int writeTimerPages[CERunLoopFileTimerPageSize];
//    
//    void * _Nullable api; /* This is used for polling API specific data */
//    uint32_t observerBufferSize;
//    uint32_t observerBufferCount;
//    
//    CERunLoopObserver_s * _Nullable * _Nonnull observers;
//};


#endif /* CEDispatcher_h */
