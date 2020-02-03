//
//  CEPool.h
//  CoreEvent
//
//  Created by vector on 2018/12/14.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEPool_h
#define CEPool_h

#include <CoreEvent/CEBase.h>


//时间单位都是microseconds
CEResult_t CEPoolAddTimeEvent(CEPoolRef _Nonnull pool,
                              CETimeDescription_s * _Nullable tdPtr,
                              uint64_t delay,
                              uint64_t leeway,
                              _Bool repeat,
                              _Bool repeatModeAfter,
                              uint64_t interval,
                              CETimeEventHandler_f _Nonnull execute,
                              void * _Nullable context,
                              CETimeEventClearContextHandler_f _Nullable clearContext);


CEResult_t CEPoolRemoveTimeEvent(CEPoolRef _Nonnull pool, CETimeDescription_s td);


#pragma mark - file event api

CEResult_t CEPoolAddFileEventMask(CEPoolRef _Nonnull pool, CEFileDescription_s fd, CEFileEventMask_es mask);

CEResult_t CEPoolRemoveFileEventMask(CEPoolRef _Nonnull pool, CEFileDescription_s fd, CEFileEventMask_es mask);

//timeout  单位为毫妙 数值必须大于0 且 小于等于CEFileEventTimeoutMillisecondMax


CEResult_t CEPoolSetFileEventReadTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);
CEResult_t CEPoolSetFileEventWriteTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);

CEResult_t CEPoolCancelFileEventReadTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t CEPoolCancelFileEventWriteTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t CEPoolUpdateFileEventReadTime(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t CEPoolUpdateFileEventWriteTime(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t CEPoolAddFileEvent(CEPoolRef _Nonnull pool,
                              CEFileDescription_s * _Nonnull fdPtr,
                              int fd,
                              _Bool customTag,
                              uint32_t tag,
                              CEFileEventHandler_f _Nonnull handler,
                              void * _Nullable clientData);
CEResult_t CEPoolRemoveFileEvent(CEPoolRef _Nonnull pool, CEFileDescription_s fd);


#pragma mark - run loop async api

void CERunLoopAsync(CEPoolRef _Nonnull pool, CEClosureExecute_f _Nonnull execute, CEClosureClearData_f _Nullable clearData, const void * _Nullable data, size_t dataSize);
void CERunLoopSync(CEPoolRef _Nonnull pool, CEClosureExecute_f _Nonnull execute, CEClosureClearData_f _Nullable clearData, const void * _Nullable data, size_t dataSize, void * _Nullable resultReceiver, size_t resultSize);



CEFileEventMask_es CEWait(int fd, CEFileEventMask_es mask, int milliseconds);

char * _Nonnull CEPoolGetApiName(void);

uint32_t CEPoolGetSetSize(CEPoolRef _Nonnull pool);


#endif /* CEPool_h */
