//
//  CEPoolPrivate.h
//  CoreEvent
//
//  Created by vector on 2018/12/14.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEPoolPrivate_h
#define CEPoolPrivate_h

#include "CEPoolInternal.h"


#pragma mark - time event api


//时间单位都是microseconds
CEResult_t __CEPoolAddTimeEvent(CEPoolRef _Nonnull pool,
                                CETimeDescription_s * _Nullable tdPtr,
                                uint64_t delay,
                                uint64_t leeway,
                                _Bool repeat,
                                _Bool repeatModeAfter,
                                uint64_t interval,
                                CETimeEventHandler_f _Nonnull execute,
                                void * _Nullable context,
                                CETimeEventClearContextHandler_f _Nullable clearContext);


CEResult_t __CEPoolRemoveTimeEvent(CEPoolRef _Nonnull pool, CETimeDescription_s td);


#pragma mark - file event api

CEResult_t __CEPoolAddFileEventMask(CEPoolRef _Nonnull pool, CEFileDescription_s fd, CEFileEventMask_es mask);

CEResult_t __CEPoolRemoveFileEventMask(CEPoolRef _Nonnull pool, CEFileDescription_s fd, CEFileEventMask_es mask);

//timeout  单位为毫妙 数值必须大于0 且 小于等于CEFileEventTimeoutMillisecondMax


CEResult_t __CEPoolSetFileEventReadTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);
CEResult_t __CEPoolSetFileEventWriteTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);

CEResult_t __CEPoolCancelFileEventReadTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t __CEPoolCancelFileEventWriteTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t __CEPoolUpdateFileEventReadTime(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t __CEPoolUpdateFileEventWriteTime(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t __CEPoolAddFileEvent(CEPoolRef _Nonnull pool,
                                CEFileDescription_s * _Nonnull fdPtr,
                                int fd,
                                _Bool customTag,
                                uint32_t tag,
                                CEFileEventHandler_f _Nonnull handler,
                                void * _Nullable clientData);
CEResult_t __CEPoolRemoveFileEvent(CEPoolRef _Nonnull pool, CEFileDescription_s fd);


#pragma mark - run loop async api

void CERunLoopAsync(CEPoolRef _Nonnull pool, CEClosureExecute_f _Nonnull execute, CEClosureClearData_f _Nullable clearData, const void * _Nullable data, size_t dataSize);
void CERunLoopSync(CEPoolRef _Nonnull pool, CEClosureExecute_f _Nonnull execute, CEClosureClearData_f _Nullable clearData, const void * _Nullable data, size_t dataSize, void * _Nullable resultReceiver, size_t resultSize);

#pragma mark - run loop api


/* return the number of processed file/time events */
int64_t __CEPoolProcessEvents(CEPoolRef _Nonnull pool);
void __CEPoolRun(CEPoolRef _Nonnull pool);



int __CEPoolResizeSetSize(CEPoolRef _Nonnull pool, uint32_t setsize);

static inline int __CEPoolGoodSize(int size) {
    if (size <= 0x10000) {
        return 0x10000;
    } else {
        uint32_t tmp = size;
        int count = 0x10000;
        int pageCount = (int)(tmp >> 16);
        
        if ((tmp & 0xffff) != 0) {
            pageCount += 1;
        }
        return count * pageCount;
    }
}


CEPoolRef _Nonnull __CEPoolCreate(unsigned int setsize);

void __CEPoolDeinit(CEPoolRef _Nonnull pool);

void __CEPoolStop(CEPoolRef _Nonnull pool);



#endif /* CEPoolPrivate_h */
