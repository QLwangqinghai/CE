//
//  CELoop.h
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CERunLoop_h
#define CERunLoop_h

#include <CoreEvent/CEBaseTypeInternal.h>


#pragma mark - time event api


//时间单位都是microseconds
CEResult_t CERunLoopAddTimeEvent(CEPool_s * _Nonnull eventLoop,
                                 CETimeDescription_s * _Nullable tdPtr,
                                 uint64_t delay,
                                 uint64_t leeway,
                                 _Bool repeat,
                                 _Bool repeatModeAfter,
                                 uint64_t interval,
                                 CETimeEventHandler_f _Nonnull execute,
                                 void * _Nullable context,
                                 CETimeEventClearContextHandler_f _Nullable clearContext);

CEResult_t CERunLoopAddRepeatTimer(CEPool_s * _Nonnull eventLoop,
                                   CETimeDescription_s * _Nullable tdPtr,
                                   uint64_t delay,
                                   uint64_t leeway,
                                   _Bool repeatModeAfter,
                                   uint64_t interval,
                                   CETimeEventHandler_f _Nonnull execute,
                                   void * _Nullable context,
                                   CETimeEventClearContextHandler_f _Nullable clearContext);

CEResult_t CERunLoopAddDelayTimer(CEPool_s * _Nonnull eventLoop,
                                  CETimeDescription_s * _Nullable tdPtr,
                                  uint64_t delay,
                                  uint64_t leeway,
                                  CETimeEventHandler_f _Nonnull execute,
                                  void * _Nullable context,
                                  CETimeEventClearContextHandler_f _Nullable clearContext);

CEResult_t CERunLoopRemoveTimeEvent(CEPool_s * _Nonnull eventLoop, CETimeDescription_s td);


#pragma mark - file event api

CEResult_t CERunLoopAddFileEventMask(CEPool_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventMask_es mask);

CEResult_t CERunLoopRemoveFileEventMask(CEPool_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventMask_es mask);

//timeout  单位为毫妙 数值必须大于0 且 小于等于CEFileEventTimeoutMillisecondMax


CEResult_t CERunLoopSetFileEventReadTimeout(CEPool_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);
CEResult_t CERunLoopSetFileEventWriteTimeout(CEPool_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);

CEResult_t CERunLoopCancelFileEventReadTimeout(CEPool_s * _Nonnull eventLoop, CEFileDescription_s fd);
CEResult_t CERunLoopCancelFileEventWriteTimeout(CEPool_s * _Nonnull eventLoop, CEFileDescription_s fd);
CEResult_t CERunLoopUpdateFileEventReadTime(CEPool_s * _Nonnull eventLoop, CEFileDescription_s fd);
CEResult_t CERunLoopUpdateFileEventWriteTime(CEPool_s * _Nonnull eventLoop, CEFileDescription_s fd);
CEResult_t CERunLoopAddFileEvent(CEPool_s * _Nonnull eventLoop,
                                 CEFileDescription_s * _Nonnull fdPtr,
                                 int fd,
                                 _Bool customTag,
                                 uint32_t tag,
                                 CEFileEventHandler_f _Nonnull handler,
                                 void * _Nullable clientData);
CEResult_t CERunLoopRemoveFileEvent(CEPool_s * _Nonnull eventLoop, CEFileDescription_s fd);


#pragma mark - run loop async api

void CERunLoopAsync(CEPool_s * _Nonnull eventLoop, CEClosureExecute_f _Nonnull execute, CEClosureClearData_f _Nullable clearData, const void * _Nullable data, size_t dataSize);
void CERunLoopSync(CEPool_s * _Nonnull eventLoop, CEClosureExecute_f _Nonnull execute, CEClosureClearData_f _Nullable clearData, const void * _Nullable data, size_t dataSize, void * _Nullable resultReceiver, size_t resultSize);

#pragma mark - run loop api


/* return the number of processed file/time events */
int64_t __CEPoolProcessEvents(CEPool_s * _Nonnull eventLoop);
CEFileEventMask_es CEWait(int fd, CEFileEventMask_es mask, int milliseconds);
void __CEPoolRun(CEPool_s * _Nonnull eventLoop);
char * _Nonnull CERunLoopGetApiName(void);

uint32_t CEPoolGetSetSize(CEPool_s * _Nonnull eventLoop);
int __CEPoolResizeSetSize(CEPool_s * _Nonnull eventLoop, uint32_t setsize);




#endif /* CELoop_h */







