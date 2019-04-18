//
//  CELoop.h
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CERunLoop_h
#define CERunLoop_h

#include "CEBaseType.h"




CERunLoop_s * _Nonnull CERunLoopShared(void);


//时间单位都是microseconds
//CEResult_t CERunLoopAddTimeEvent(CERunLoop_s * _Nonnull eventLoop, uintptr_t event, uint64_t delay);


CEResult_t CERunLoopRemoveTimeEvent(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td);

/* return the number of processed file/time events */
int64_t CERunLoopProcessEvents(CERunLoop_s * _Nonnull eventLoop);
CEFileEventMask_es CEWait(int fd, CEFileEventMask_es mask, int milliseconds);
void CERunLoopRun(CERunLoop_s * _Nonnull eventLoop);
char * _Nonnull CERunLoopGetApiName(void);

uint32_t CERunLoopGetSetSize(CERunLoop_s * _Nonnull eventLoop);
int CERunLoopResizeSetSize(CERunLoop_s * _Nonnull eventLoop, uint32_t setsize);

void CERunLoopAsync(CERunLoop_s * _Nonnull eventLoop, CEBlockExecute_f _Nonnull execute, CEBlockDealloc_f _Nullable dealloc, void * _Nullable context);

void CERunLoopRunDefault(void);



#endif /* CELoop_h */







