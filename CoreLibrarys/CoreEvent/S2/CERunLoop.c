//
//  CELoop.c
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CERunLoop.h"
#include "CEMemory.h"
#include "CEConfig.h"
#include "CEBaseApi.h"
#include "CETimeEvent.h"
#include "CEFileEvent.h"
#include "CEThread.h"
#include "CEClosure.h"

#pragma mark - run loop progress

static const char * const _Nullable CERunLoopProgressName[9] = {
    NULL,
    "CERunLoopProgress.doBlock0",
    "CERunLoopProgress.doTimer0",
    "CERunLoopProgress.doCheckSource",
    "CERunLoopProgress.doBlock1",
    "CERunLoopProgress.doTimer1",
    "CERunLoopProgress.doSource",
    "CERunLoopProgress.doCheckSourceTimeout",
    "CERunLoopProgress.doSourceTimeout",
};

void CERunLoopProcessEventsBegin(CERunLoop_s * _Nonnull eventLoop) {
    CELogVerbose2(64, "CERunLoopProgress begin");
}
void CERunLoopProcessEventsEnd(CERunLoop_s * _Nonnull eventLoop) {
    CELogVerbose2(64, "CERunLoopProgress end");
}
static inline void CERunLoopProcessBeginNew(CERunLoop_s * _Nonnull eventLoop, CERunLoopProgress_t new) {
    eventLoop->progress = new;
    CELogVerbose2(0, "%s begin", CERunLoopProgressName[new]);
}
static inline void CERunLoopProcessFinishOld(CERunLoop_s * _Nonnull eventLoop) {
    CELogVerbose2(0, "%s end", CERunLoopProgressName[eventLoop->progress]);
}

#pragma mark - func statement

void CERunLoopReadTimeout(CERunLoop_s * _Nonnull eventLoop, uint32_t timerId);
void CERunLoopWriteTimeout(CERunLoop_s * _Nonnull eventLoop, uint32_t timerId);


extern CETimeEvent_s * _Nullable CETimeEventSourceInit(CETimeEventManager_s * _Nonnull manager,
                                                       uint64_t delay,
                                                       uint64_t leeway,
                                                       _Bool repeat,
                                                       _Bool repeatModeAfter,
                                                       uint64_t interval,
                                                       CETimeEventHandler_f _Nonnull execute,
                                                       void * _Nullable context,
                                                       CETimeEventClearContextHandler_f _Nullable clearContext);
extern void CETimeEventSourceExecute(CETimeEvent_s * _Nonnull te, CERunLoop_s * _Nonnull eventLoop);
extern void CETimeEventSourceDeinit(CETimeEvent_s * _Nonnull te, CERunLoop_s * _Nonnull eventLoop);
extern void CETimeEventSourceStop(CETimeEvent_s * _Nonnull te, CERunLoop_s * _Nonnull eventLoop);

extern CEFileEvent_s * _Nullable CERunLoopGetFileEvent(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);
extern CEFileEvent_s * _Nullable CERunLoopGetFileEventByIndex(CERunLoop_s * _Nonnull eventLoop, int index);

CEBlockQueuePage_s * _Nullable CEBlockQueueRemoveAll(CEBlockQueue_s * _Nonnull queue);
void CEBlockQueueAppend(CEBlockQueue_s * _Nonnull queue, uintptr_t block);

void CERunLoopBlockQueueLock(CERunLoop_s * _Nonnull eventLoop);
void CERunLoopBlockQueueUnlock(CERunLoop_s * _Nonnull eventLoop);

#pragma mark - fired event helper func

static inline void CERunLoopAppendFiredEvent(CERunLoop_s * _Nonnull eventLoop, CEFiredEvent_s fe) {
    if (fe.fd >= eventLoop->setsize || fe.fd < 0) {
        fprintf(stderr, "CERunLoopAppendFiredEvent error, fd: %d out setsize\n", fe.fd);
        fflush(stderr);
        abort();
    }
    
    uint32_t offset = (uint32_t)index;
    eventLoop->firedPages[offset >> 16][offset & 0xffff] = fe;
    eventLoop->firedCount += 1;
}

static inline CEFiredEvent_s * _Nullable CERunLoopGetFiredEventByIndex(CERunLoop_s * _Nonnull eventLoop, int64_t index) {
    if (index >= eventLoop->setsize || index < 0) {
        return NULL;
    }
    uint32_t offset = (uint32_t)index;
    return &(eventLoop->firedPages[offset >> 16][offset & 0xffff]);
}

#pragma mark - time

static const struct timeval CEApiPollDefaultTime = {0, 124000};

void CERunLoopTimeSlip(CERunLoop_s * _Nonnull eventLoop, _Bool fileTimeSlip) {
    uint64_t nowMicroseconds = CEGetMicrosecondsTime();
    eventLoop->microsecondsTime = nowMicroseconds;

    if (!fileTimeSlip) {
        return;
    }
    
    uint64_t seconds8 = nowMicroseconds / 125000;
    if (seconds8 > eventLoop->fileTimerSeconds8) {
        uint64_t interval = seconds8 - eventLoop->fileTimerSeconds8;
        eventLoop->fileTimerSeconds8 = seconds8;
        int count = 0;
        if (interval >= 0x4000) {
            count = 0x4000;
        } else {
            count = (int)interval;
        }
        uint32_t index = eventLoop->timerFiredIndex;
        for (; count>0; count--) {
            index = (index + 1) & 0x3fff;
            CERunLoopReadTimeout(eventLoop, index);
            CERunLoopWriteTimeout(eventLoop, index);
        }
    }
}

#pragma mark - thread key-specific

CERunLoop_s * _Nullable CEGetCurrentLoop(void) {
    return (CERunLoop_s *)pthread_getspecific(CERunLoopThreadKeyShared());

}
void CESetCurrentLoop(CERunLoop_s * _Nullable loop) {
    CERunLoop_s * old = CEGetCurrentLoop();
    if (NULL != old && NULL != loop & old != loop) {
        fprintf(stderr, "SetCurrentLoop error\n");
        fflush(stderr);
        abort();
    }
    int result = pthread_setspecific(CERunLoopThreadKeyShared(), loop);
    assert(result == 0);
}

#pragma mark - api callback


void CEApiFileEventCallback(void * _Nullable context, void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(context);
    if (mask == CEFileEventMaskNone) {
        return;
    }
    CERunLoop_s * eventLoop = (CERunLoop_s *)context;

    CEFileEvent_s * fe = CERunLoopGetFileEventByIndex(eventLoop, fd);
    assert(fe);
    assert(fe->handler);
    
    CEFileDescription_s efd = {.fd = fd, .tag = fe->fdTag};
    
    if (CEFileEventMaskReadWritable & mask) {
        if (0 == fe->readable && 0 == fe->writable) {
            CERunLoopAppendFiredEvent(eventLoop, efd);
        }
        if (CEFileEventMaskReadable & mask) {
            fe->readable = 1;
        }
        if (CEFileEventMaskWritable & mask) {
            fe->writable = 1;
        }
    }
}

void CEApiPipeEventCallback(void * _Nullable context, void * _Nonnull api) {
    assert(context);
    CERunLoop_s * eventLoop = (CERunLoop_s *)context;
    eventLoop->blockEvent = 1;
    CELogVerbose2(128, "CEApiPipeEventCallback");
}


#pragma mark - run loop life


CERunLoop_s * _Nonnull CERunLoopCreate(unsigned int setsize) {
    setsize = CERunLoopSize(setsize);
    
    void * api = CEApiCreate(setsize);
    if (NULL == api) {
        fprintf(stderr, "CEApiCreate failure with size: %u\n", setsize);
        fflush(stderr);
        abort();
    };

    CERunLoop_s * eventLoop = (CERunLoop_s *)CEAllocateClear(sizeof(CERunLoop_s));
    eventLoop->firedCount = 0;
    
    uint32_t pageCount = setsize >> 16;

    for (uint32_t index=0; index< pageCount; index++) {
        eventLoop->fileEventsPages[index] = (CEFileEvent_s *)CEAllocateClear(sizeof(CEFileEvent_s) * 0x10000);
        eventLoop->firedPages[index] = (CEFiredEvent_s *)CEAllocate(sizeof(CEFiredEvent_s) * 0x10000);
    }
    for (int index=0; index< 0x4000; index++) {
        eventLoop->readTimerPages[index] = CEFileDescriptionInvalid;
        eventLoop->writeTimerPages[index] = CEFileDescriptionInvalid;
    }
    
    eventLoop->setsize = setsize;
    eventLoop->microsecondsTime = CEGetMicrosecondsTime();

    atomic_store(&(eventLoop->runningStates), 0);
    eventLoop->maxfd = -1;

    atomic_store(&(eventLoop->runInThread), 0);

    
#if __APPLE__
    eventLoop->blockQueueLock = OS_UNFAIR_LOCK_INIT;
#else
    pthread_spin_init(&(eventLoop->blockQueueLock), PTHREAD_PROCESS_PRIVATE);
#endif
    
    eventLoop->api = api;

    CETimeEventQueueInit(&(eventLoop->timeEventManager.timerQueue));
    //pipe
    //https://www.cnblogs.com/kunhu/p/3608109.html
    
    //eventfd
    //https://www.cnblogs.com/zhengchunhao/p/5335914.html
    
    //timerfd 和 eventfd接口实现进程线程通信
    //http://www.cnblogs.com/meihao1203/p/9368398.html
    
    //https://wenku.baidu.com/view/ab78e41ff11dc281e53a580216fc700abb6852be.html
    
    //signalfd
    //https://www.aliyun.com/jiaocheng/132736.html?spm=5176.100033.2.5.56a42e7aP3DY2O
    
    return eventLoop;
}
void CERunLoopDeinit(CERunLoop_s *eventLoop) {
    CEApiFree(eventLoop);
    uint32_t pageCount = eventLoop->setsize >> 16;

    for (uint32_t index=pageCount; index<pageCount; index++) {
        CEFileEvent_s * eventPage = eventLoop->fileEventsPages[index];
        CEFiredEvent_s * firedPage = eventLoop->firedPages[index];
        CEDeallocate(eventPage);
        CEDeallocate(firedPage);
    }
    CEDeallocate(eventLoop);
}

void CERunLoopRun(CERunLoop_s *eventLoop) {// 0 is stopped , 1 running, 2 is will stopped
    uint32_t runningStates = atomic_load(&(eventLoop->runningStates));
    uint32_t newRunningStates = 1;
    
    do {
        runningStates = atomic_load(&(eventLoop->runningStates));
        if (0 != runningStates) {
            return;
        }
    } while (!atomic_compare_exchange_weak(&(eventLoop->runningStates), &runningStates, newRunningStates));
    
    
    CESetCurrentLoop(eventLoop);
    
    while (atomic_load(&(eventLoop->runningStates)) == 1) {
        CERunLoopProcessEvents(eventLoop);
    }
    
    CESetCurrentLoop(NULL);
}

void CERunLoopStop(CERunLoop_s *eventLoop) {
//    eventLoop->stop = 1;
}


#pragma mark - run loop size(setter & getter)

/* Return the current set size. */
uint32_t CERunLoopGetSetSize(CERunLoop_s *eventLoop) {
    return eventLoop->setsize;
}

/* Resize the maximum set size of the event loop.
 * If the requested set size is smaller than the current set size, but
 * there is already a file descriptor in use that is >= the requested
 * set size minus one, AE_ERR is returned and the operation is not
 * performed at all.
 *
 * Otherwise AE_OK is returned and the operation is successful. */
int CERunLoopResizeSetSize(CERunLoop_s *eventLoop, uint32_t setsize) {
    setsize = CERunLoopSize(setsize);
    
    //TODO: 校验状态
    
    if (setsize == eventLoop->setsize) {
        return CEResultSuccess;
    }
    if (eventLoop->maxfd >= setsize) {
        return CEResultFailureResize;
    }
    CEApiResize(eventLoop, setsize);
    
    uint32_t oldPageCount = eventLoop->setsize >> 16;
    uint32_t newPageCount = setsize >> 16;
    
    for (uint32_t index=newPageCount; index<oldPageCount; index++) {
        CEFileEvent_s * eventPage = eventLoop->fileEventsPages[index];
        CEFiredEvent_s * firedPage = eventLoop->firedPages[index];
        
        CEDeallocate(eventPage);
        CEDeallocate(firedPage);
        eventLoop->fileEventsPages[index] = NULL;
        eventLoop->firedPages[index] = NULL;
    }
    
    for (uint32_t index=oldPageCount; index<newPageCount; index++) {
        eventLoop->fileEventsPages[index] = (CEFileEvent_s *)CEAllocateClear(sizeof(CEFileEvent_s) * 0x10000);
        eventLoop->firedPages[index] = (CEFiredEvent_s *)CEAllocate(sizeof(CEFiredEvent_s) * 0x10000);
    }
    eventLoop->setsize = setsize;
    return CEResultSuccess;
}


#pragma mark - run loop time event manager api



#pragma mark - run loop time event api

CEResult_t CERunLoopCreateTimeEvent(CERunLoop_s * _Nonnull eventLoop,
                                    CETimeEvent_s * _Nullable * _Nonnull eventPtr,
                                    uint64_t delay,
                                    uint64_t leeway,
                                    _Bool repeat,
                                    _Bool repeatModeAfter,
                                    uint64_t interval,
                                    CETimeEventHandler_f _Nonnull execute,
                                    void * _Nullable context,
                                    CETimeEventClearContextHandler_f _Nullable clearContext) {
    
    if (interval > CETimeEventIntervalMax || NULL == execute || NULL == eventLoop) {
        return CEResultErrorParams;
    }
    if (repeat && interval == 0) {
        return CEResultErrorParams;
    }
    CETimeEventManager_s * tem = &(eventLoop->timeEventManager);
    CETimeEvent_s * te = CETimeEventSourceInit(tem, delay, leeway, repeat, repeatModeAfter, interval, execute, context, clearContext);
    
    *eventPtr = te;
    if (NULL == te) {
        return CEResultErrorTimeDescriptionNotEnough;
    } else {
        CETimeEventManager_s * tem = &(eventLoop->timeEventManager);
        CETimeEventQueue_s * teq = &(tem->timerQueue);
        CETimeEventQueueInsert(teq, te);
        return CEResultSuccess;
    }
}

CEResult_t CERunLoopAddTimeEvent(CERunLoop_s * _Nonnull eventLoop,
                                 CETimeDescription_s * _Nullable tdPtr,
                                 uint64_t delay,
                                 uint64_t leeway,
                                 _Bool repeat,
                                 _Bool repeatModeAfter,
                                 uint64_t interval,
                                 CETimeEventHandler_f _Nonnull execute,
                                 void * _Nullable context,
                                 CETimeEventClearContextHandler_f _Nullable clearContext) {
    CETimeEvent_s * te = NULL;
    
    CEResult_t result = CERunLoopCreateTimeEvent(eventLoop, &te, delay, leeway, repeat, repeatModeAfter, interval, execute, context, clearContext);
    return result;
}
CEResult_t CERunLoopAddRepeatTimer(CERunLoop_s * _Nonnull eventLoop,
                                   CETimeDescription_s * _Nullable tdPtr,
                                   uint64_t delay,
                                   uint64_t leeway,
                                   _Bool repeatModeAfter,
                                   uint64_t interval,
                                   CETimeEventHandler_f _Nonnull execute,
                                   void * _Nullable context,
                                   CETimeEventClearContextHandler_f _Nullable clearContext) {
    return CERunLoopAddTimeEvent(eventLoop, tdPtr, delay, leeway, 1, repeatModeAfter, interval, execute, context, clearContext);
}

CEResult_t CERunLoopAddDelayTimer(CERunLoop_s * _Nonnull eventLoop,
                                  CETimeDescription_s * _Nullable tdPtr,
                                  uint64_t delay,
                                  uint64_t leeway,
                                  CETimeEventHandler_f _Nonnull execute,
                                  void * _Nullable context,
                                  CETimeEventClearContextHandler_f _Nullable clearContext) {
    return CERunLoopAddTimeEvent(eventLoop, tdPtr, delay, leeway, 0, 0, 0, execute, context, clearContext);
}


CEResult_t CERunLoopRemoveTimeEvent(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td) {
    
    CETimeEventManager_s * tem = &(eventLoop->timeEventManager);
    CETimeEventQueue_s * teq = &(eventLoop->timeEventManager.timerQueue);
    
    CETimeEvent_s * te = CETimeEventManagerGetEvent(tem, td);

    if (NULL != te) {
        _Bool executing = CETimeEventStatesExecuting == te->states;
        
        if (executing) {
            CETimeEventSourceStop(te, eventLoop);
        } else {
            CETimeEventQueueRemove(teq, te);
            CETimeEventSourceDeinit(te, eventLoop);
        }
    } else {
        return CEResultErrorTimeDescriptionInvalid;
    }

    return CEResultSuccess;
}

#pragma mark - run loop file event api

CEResult_t CERunLoopAddFileEventMask(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventMask_es mask) {
    CEFileEvent_s *fe = CERunLoopGetFileEvent(eventLoop, fd);
    if (NULL == fe) {
        return -1;
    }
    
    if (CEApiAddEvent(eventLoop, fd.fd, fe->mask, mask) == -1) {
        return CEResultErrorSystemCall;
    }
    fe->mask |= mask;
    
    if (fd.fd > eventLoop->maxfd) {
        eventLoop->maxfd = fd.fd;
    }
    if (fd.fd > eventLoop->maxIndex) {
        eventLoop->maxIndex = fd.fd;
    }
    return CEResultSuccess;
}

CEResult_t CERunLoopRemoveFileEventMask(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventMask_es mask) {
    CEFileEvent_s *fe = CERunLoopGetFileEvent(eventLoop, fd);
    if (NULL == fe) {
        return CEResultErrorFileDescription;
    }
    if (fe->mask == CEFileEventMaskNone) {
        return CEResultSuccess;
    };
    
    CEApiRemoveEvent(eventLoop, fd.fd, mask);
    fe->mask = fe->mask & (~mask);
    
    if (fd.fd == eventLoop->maxfd && fe->mask == CEFileEventMaskNone) {
        /* Update the max fd */
        int j;
        
        for (j = eventLoop->maxfd-1; j >= 0; j--) {
            CEFileEvent_s * iter = CERunLoopGetFileEventByIndex(eventLoop, j);
            if (iter->mask != CEFileEventMaskNone) {
                break;
            }
        }
        eventLoop->maxfd = j;
    }
    
    
    if (fd.fd == eventLoop->maxIndex) {
        int j;
        for (j = eventLoop->maxIndex-1; j >= 0; j--) {
            CEFileEvent_s * iter = CERunLoopGetFileEventByIndex(eventLoop, j);
            if (NULL != iter->handler) {
                break;
            }
        }
        eventLoop->maxIndex = j;
    }
    return CEResultSuccess;
}


//timeout  单位为毫妙 数值必须大于0 且 小于等于CEFileEventTimeoutMillisecondMax
CEResult_t CERunLoopSetFileEventReadTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout) {
    return CEFileEventSourceSetReadTimeout(eventLoop, fd, initialTimeout, timeout);
}
CEResult_t CERunLoopSetFileEventWriteTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout) {
    return CEFileEventSourceSetWriteTimeout(eventLoop, fd, initialTimeout, timeout);
}

CEResult_t CERunLoopCancelFileEventReadTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    return CEFileEventSourceCancelReadTimeout(eventLoop, fd);
}
CEResult_t CERunLoopCancelFileEventWriteTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    return CEFileEventSourceCancelWriteTimeout(eventLoop, fd);
}
CEResult_t CERunLoopUpdateFileEventReadTime(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    return CEFileEventSourceUpdateReadTime(eventLoop, fd);
}
CEResult_t CERunLoopUpdateFileEventWriteTime(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    return CEFileEventSourceUpdateWriteTime(eventLoop, fd);
}
CEResult_t CERunLoopAddFileEvent(CERunLoop_s * _Nonnull eventLoop,
                                 CEFileDescription_s * _Nonnull fdPtr,
                                 int fd,
                                 _Bool customTag,
                                 uint32_t tag,
                                 CEFileEventHandler_f _Nonnull handler,
                                 void * _Nullable clientData) {
    if (fd == INT_MAX) {
        CELogWarning("CEFileEventSourceInit error, unsupport with fd: %d\n", INT_MAX);
        return CEResultErrorParams;
    };
    
    if (NULL == handler || NULL == eventLoop || NULL == fdPtr) {
        return CEResultErrorParams;
    }
    
    if (fd >= eventLoop->setsize) {
        int result = CERunLoopResizeSetSize(eventLoop, fd + 1);
        if (0 != result) {
            return result;
        }
    }
    fdPtr->fd = fd;
    if (customTag) {
        fdPtr->tag = tag;
    }
    CEFileEvent_s * fe = CEFileEventSourceInit(eventLoop, *fdPtr, handler, clientData);
    if (NULL == fe) {
        return CEResultErrorSourceReinit;
    }
    if (!customTag) {
        uint32_t rtag = CERunLoopNextFdTag(eventLoop);
        fdPtr->tag = rtag;
        fe->fdTag = rtag;
    }
    
    if (fd > eventLoop->maxfd) {
        eventLoop->maxfd = fd;
    }
    return CEResultSuccess;
}
CEResult_t CERunLoopRemoveFileEvent(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    CEFileEvent_s * fe = CERunLoopGetFileEvent(eventLoop, fd);
    if (NULL == fe) {
        return CEResultErrorFileDescription;
    }
    return CEFileEventSourceDeinit(eventLoop, fe);
}

#pragma mark - run loop async api

//struct _CEBlock {
//    CEThreadWaiter_s * _Nullable waiter;
//    void * _Nullable resultReceive;
//    CEClosureExecute_f _Nonnull execute;
//    CEClosureClearData_f _Nullable dealloc;
//    size_t paramsSize;
//    size_t resultSize;
//    CEAtomicMemoryBlock_t context[0];
//};
void CERunLoopAsync(CERunLoop_s * _Nonnull eventLoop, CEClosureExecute_f _Nonnull execute, CEClosureClearData_f _Nullable clearData, const void * _Nullable data, size_t dataSize) {
    assert(eventLoop);
    assert(execute);
    size_t paramsSize = sizeof(void *);
    uintptr_t block = CEClosureInit(execute, clearData, data, dataSize, paramsSize, NULL, 0, NULL);
    assert((uintptr_t)NULL != block);

    CERunLoopBlockQueueLock(eventLoop);
    CEBlockQueueAppend(&(eventLoop->blockQueue), block);
    CERunLoopBlockQueueUnlock(eventLoop);
}

void CERunLoopSync(CERunLoop_s * _Nonnull eventLoop, CEClosureExecute_f _Nonnull execute, CEClosureClearData_f _Nullable clearData, const void * _Nullable data, size_t dataSize, void * _Nullable resultReceiver, size_t resultSize) {
    size_t paramsSize = sizeof(void *);

    CEThreadWaiter_s * waiter = CEThreadGetWaiter();
    assert(eventLoop);
    assert(execute);
    uintptr_t block = CEClosureInit(execute, clearData, data, dataSize, paramsSize, resultReceiver, resultSize, waiter);
    assert((uintptr_t)NULL != block);
    
    CERunLoopBlockQueueLock(eventLoop);
    uintptr_t whoWakeUp = atomic_load(&(waiter->whoWakeUp));
    assert((uintptr_t)(NULL) == whoWakeUp);
    atomic_store(&(waiter->whoWakeUp), (uintptr_t)block);
    CEBlockQueueAppend(&(eventLoop->blockQueue), block);
    CERunLoopBlockQueueUnlock(eventLoop);

    CEThreadWaiterWait(waiter);
    
//    CEAtomicMemoryLoad(resultPtr, resultReceiver, resultSize);
//    CEDeallocate(entityPtr);
}



#pragma mark - run loop api



/* Wait for milliseconds until the given file descriptor becomes
 * writable/readable/exception */
CEFileEventMask_es CEWait(int fd, CEFileEventMask_es mask, int milliseconds) {
    return CEApiWait(fd, mask, milliseconds);
}

char *CERunLoopGetApiName(void) {
    return CEApiName();
}


#pragma mark - run loop process events


int64_t CERunLoopDoBlocks(CERunLoop_s * _Nonnull eventLoop) {
    
    CEBlockQueuePage_s * page = NULL;
    
    CERunLoopBlockQueueLock(eventLoop);
    page = CEBlockQueueRemoveAll(&(eventLoop->blockQueue));
    CERunLoopBlockQueueUnlock(eventLoop);
    
    int64_t count = 0;
    
    while (NULL != page) {
        CEBlockQueuePage_s * nextPage = page->next;
        for (size_t i=0; i<page->used; i++) {
            count += 1;
            
            uintptr_t closurePtrValue = page->buffer[i];

            CEClosureInfo_s closureInfo = {};
            CEClosureGetInfo(closurePtrValue, &closureInfo);
            assert(closureInfo.paramsSize == sizeof(void *));
            
            closureInfo.execute(closurePtrValue, &eventLoop);
            if (closureInfo.clearData) {
                closureInfo.clearData(closurePtrValue, paramsPtr, entityPtr->paramsSize);
            }
            CEThreadWaiter_s * waiter = entityPtr->waiter;
            if (NULL != waiter) {
                uintptr_t whoWakeUp = atomic_load(&(waiter->whoWakeUp));
                assert((uintptr_t)(&(entityPtr->waiter)) == whoWakeUp);
                atomic_store(&(waiter->whoWakeUp), (uintptr_t)(NULL));
                CEThreadWaiterWakeUp(waiter);
            } else {
                CEDeallocate(entityPtr);
            }
        }
        CEDeallocate(page);

        page = nextPage;
    }
    
    return count;
}

int64_t _CERunLoopDoTimers(CERunLoop_s * _Nonnull eventLoop) {
    int64_t processed = 0;
    
    CETimeEventManager_s * tem = &(eventLoop->timeEventManager);
    CETimeEventQueue_s * teq = &(eventLoop->timeEventManager.timerQueue);
    
    CETimeEvent_s * te = CETimeEventQueueGetFirst(teq);
    
    uint64_t now = CEGetMicrosecondsTime();
    eventLoop->microsecondsTime = now;

    int64_t fired = 0;
    
    while (NULL != te) {
        if (te->when <= now) {
            tem->executingTimeEvent = te;
            CETimeEventSourceExecute(te, eventLoop);
            if (CETimeEventStatesWait == te->states) {
                CETimeEventQueueShiftDown(teq, te->qIndex);
            } else {
                CETimeEvent_s * removed = CETimeEventQueueRemoveFirst(teq);
                CETimeEventSourceDeinit(removed, eventLoop);
            }

            te = CETimeEventQueueGetFirst(teq);

            tem->executingTimeEvent = NULL;
            fired += 1;
        } else {
            break;
        }
    }
    tem->executingTimeEvent = NULL;
    
    return processed;
}

int64_t CERunLoopDoTimers(CERunLoop_s * _Nonnull eventLoop) {
    return _CERunLoopDoTimers(eventLoop);
}

void CERunLoopDoCheckSource(CERunLoop_s * _Nonnull eventLoop) {
    struct timeval tv = CEApiPollDefaultTime;
    struct timeval * tvp = &tv;
    
    uint64_t now = CEGetMicrosecondsTime();
    eventLoop->microsecondsTime = now;
    
    CETimeEvent_s * te = CETimeEventQueueGetFirst(&(eventLoop->timeEventManager.timerQueue));
    if (NULL != te) {
        if (te->when <= now) {
            tv.tv_sec = 0;
            tv.tv_usec = 1;
        } else {
            uint64_t tmp = te->when - now;
            if (tmp < tv.tv_usec) {
                tv.tv_sec = 0;
                tv.tv_usec = (int)tmp;
            }
        }
    }
    
    static const CEApiPoolCallback_s callback = {
        .fileEventCallback = CEApiFileEventCallback,
        .pipeCallback = CEApiPipeEventCallback,
    };
    CEApiPoll(eventLoop->api, tvp, eventLoop->setsize, eventLoop, &callback);
}

int64_t CERunLoopDoSource(CERunLoop_s * _Nonnull eventLoop) {
    int64_t processed = 0;
    
    for (uint64_t j = 0; j < eventLoop->firedCount; j++) {
        CEFileDescription_s fd = eventLoop->firedPages[eventLoop->firedCount >> 16][eventLoop->firedCount & 0xffff];
        CEFileEvent_s *fe = CERunLoopGetFileEvent(eventLoop, fd);
        if (NULL != fe) {
            if (fe->readable || fe->writable) {
                CEFileEventInfo_s info = {
                    .readable = fe->readable,
                    .writable = fe->writable,
                    .isReadTimeout = 0,
                    .isWriteTimeout = 0,
                    .xxx = 0,
                };
                fe->readable = 0;
                fe->writable = 0;
                fe->handler(eventLoop, fd, fe->clientData, info);
                processed++;
            }
        }
    }
    eventLoop->firedCount = 0;
    return processed;
}

void CERunLoopDoCheckSourceTimeout(CERunLoop_s * _Nonnull eventLoop) {
    CERunLoopTimeSlip(eventLoop, 1);
}

int64_t CERunLoopDoSourceTimeout(CERunLoop_s * _Nonnull eventLoop) {
    int64_t processed = 0;

    for (int j = 0; j < eventLoop->firedCount; j++) {
        CEFileDescription_s fd = *(CERunLoopGetFiredEventByIndex(eventLoop, j));
        
        CEFileEvent_s *fe = CERunLoopGetFileEvent(eventLoop, fd);
        if (NULL !=  fe) {
            if (fe->isReadTimeout || fe->isWriteTimeout) {
                CEFileEventInfo_s info = {
                    .readable = 0,
                    .writable = 0,
                    .isReadTimeout = fe->isReadTimeout,
                    .isWriteTimeout = fe->isWriteTimeout,
                    .xxx = 0,
                };
                fe->isReadTimeout = 0;
                fe->isWriteTimeout = 0;
                fe->handler(eventLoop, fd, fe->clientData, info);
                processed++;
            }
        }
    }
    eventLoop->firedCount = 0;
    return processed;
}



/* Process every pending time event, then every pending file event
 * (that may be registered by time event callbacks just processed).
 * The function returns the number of events processed. */
int64_t CERunLoopProcessEvents(CERunLoop_s * _Nonnull eventLoop) {
    
//    CERunLoopUpdateProcess(eventLoop, CERunLoopProgressOnBegin);
    CERunLoopProcessEventsBegin(eventLoop);
    
    int64_t processed = 0;
    
    CERunLoopProcessBeginNew(eventLoop, CERunLoopProgressDoBlock0);
    processed += CERunLoopDoBlocks(eventLoop);
    CERunLoopProcessFinishOld(eventLoop);
    
    CERunLoopProcessBeginNew(eventLoop, CERunLoopProgressDoTimer0);
    processed += CERunLoopDoTimers(eventLoop);
    CERunLoopProcessFinishOld(eventLoop);

    CERunLoopProcessBeginNew(eventLoop, CERunLoopProgressDoCheckSource);
#if DEBUG
    uint64_t checkBegin = CESystemBootInterval();
    CERunLoopDoCheckSource(eventLoop);
    uint64_t checkEnd = CESystemBootInterval();
    uint64_t checkUsedTime = checkEnd - checkBegin;
    CELogVerbose2(126, "CERunLoopDoCheckSource used time: %llu", checkUsedTime);
#else
    CERunLoopDoCheckSource(eventLoop);
#endif
    CERunLoopProcessFinishOld(eventLoop);

    CERunLoopProcessBeginNew(eventLoop, CERunLoopProgressDoBlock1);
    processed += CERunLoopDoBlocks(eventLoop);
    CERunLoopProcessFinishOld(eventLoop);

    CERunLoopProcessBeginNew(eventLoop, CERunLoopProgressDoTimer1);
    processed += CERunLoopDoTimers(eventLoop);
    CERunLoopProcessFinishOld(eventLoop);

    CERunLoopProcessBeginNew(eventLoop, CERunLoopProgressDoSource);
    processed += CERunLoopDoSource(eventLoop);
    CERunLoopProcessFinishOld(eventLoop);

    CERunLoopProcessBeginNew(eventLoop, CERunLoopProgressDoCheckSourceTimeout);
    CERunLoopDoCheckSourceTimeout(eventLoop);
    CERunLoopProcessFinishOld(eventLoop);

    CERunLoopProcessBeginNew(eventLoop, CERunLoopProgressDoSourceTimeout);
    CERunLoopDoSourceTimeout(eventLoop);
    CERunLoopProcessFinishOld(eventLoop);

//    CERunLoopUpdateProcess(eventLoop, CERunLoopProgressOnEnd);
    CERunLoopProcessEventsEnd(eventLoop);

    return processed; /* return the number of processed file/time events */
}


#pragma mark - run loop private method


void CERunLoopBlockQueueLock(CERunLoop_s * _Nonnull eventLoop) {
    assert(eventLoop);
#if __APPLE__
    os_unfair_lock_lock(&(eventLoop->blockQueueLock));
#else
    pthread_spin_lock(&(eventLoop->blockQueueLock));
#endif
}

void CERunLoopBlockQueueUnlock(CERunLoop_s * _Nonnull eventLoop) {
    assert(eventLoop);
#if __APPLE__
    os_unfair_lock_unlock(&(eventLoop->blockQueueLock));
#else
    pthread_spin_unlock(&(eventLoop->blockQueueLock));
#endif
}


CEBlockQueuePage_s * _Nullable CEBlockQueueRemoveAll(CEBlockQueue_s * _Nonnull queue) {
    CEBlockQueuePage_s * page = queue->begin;
    queue->begin = NULL;
    queue->end = NULL;
    return page;
}

void CEBlockQueueAppend(CEBlockQueue_s * _Nonnull queue, uintptr_t block) {
    CEBlockQueuePage_s * page = queue->end;
    
    if (NULL == page) {
        assert(NULL == queue->begin);
        page = CEAllocateClear(sizeof(CEBlockQueuePage_s));
        queue->begin = page;
        queue->end = page;
    } else {
        assert(NULL != queue->begin);
        if (page->used == CEBlockQueuePageSize) {
            page = CEAllocateClear(sizeof(CEBlockQueuePage_s));
            queue->end->next = page;
            queue->end = page;
        }
    }
    assert(page->used < CEBlockQueuePageSize);
    page->buffer[page->used] = block;
    page->used ++;
}


void * CERunLoopRunThread(void * args) {
    CERunLoop_s * eventLoop = (CERunLoop_s *)args;

    CERunLoopRun(eventLoop);
    return NULL;
}

_Bool CERunLoopMakeThreadRunDefault(CERunLoop_s * _Nonnull eventLoop) {
    pthread_t tid;
    pthread_attr_t attr;
    struct sched_param param;
    //初始化线程属性
    pthread_attr_init(&attr);
    int policy;
    int result = 0;
    
    result = pthread_getschedparam(pthread_self(), &policy, &param);
    if (0 != result) {
        CELogError("pthread_getschedparam %s\n", strerror(result));
        return 0;
    }
    result = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    if (0 != result) {
        CELogError("pthread_attr_setscope PTHREAD_SCOPE_SYSTEM %s\n", strerror(result));
        return 0;
    }
    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (0 != result) {
        CELogError("pthread_attr_setdetachstate PTHREAD_CREATE_DETACHED %s\n", strerror(result));
        return 0;
    }
    
    size_t stacksize = 0;
    result = pthread_attr_getstacksize(&attr, &stacksize);
    
    if (0 != result) {
        CELogError("pthread_attr_getstacksize error %s\n", strerror(result));
        return 0;
    }
    
    size_t rStacksize = 1024 * 1024 * 2;
    if (stacksize < rStacksize) {
        result = pthread_attr_setstacksize(&attr, rStacksize);//设置线程堆栈大小
        if (0 != result) {
            CELogError("pthread_attr_setstacksize error %s\n", strerror(result));
            return 0;
        }
    }
    
    
    int priority = sched_get_priority_max(policy);
    if (-1 == priority) {
        CELogError("sched_get_priority_max error %s\n", strerror(result));
        return 0;
    }
    
    param.sched_priority = priority;
    
    result = pthread_attr_setschedparam(&attr, &param);
    if (0 != result) {
        CELogError("pthread_attr_setschedparam %s\n", strerror(result));
        return 0;
    }
    
    result = pthread_create(&tid, &attr, CERunLoopRunThread, eventLoop);
    if (0 != result) {
        CELogError("pthread_create %s\n", strerror(result));
        return 0;
    }
    return 1;
}


void CERunLoopRunDefault(void) {
    CERunLoop_s * eventLoop = CERunLoopShared();

    uint32_t runningStates = atomic_load(&(eventLoop->runningStates));
    if (0 != runningStates) {
        return;
    }
    assert(CERunLoopMakeThreadRunDefault(eventLoop));
}
void CERunLoopWakeUp(CERunLoop_s * _Nonnull eventLoop) {
    assert(eventLoop);    
    CEApiWakeUp(eventLoop->api);
}


void CERunLoopReadTimeout(CERunLoop_s * _Nonnull eventLoop, uint32_t timerId) {
    int fd = eventLoop->readTimerPages[timerId];
    eventLoop->readTimerPages[timerId] = CEFileDescriptionInvalid;
    while (CEFileDescriptionInvalid != fd) {
        if (fd >= 0) {
            CEFileEvent_s * fe = CERunLoopGetFileEventByIndex(eventLoop, fd);
            if (NULL == fe || NULL == fe->handler) {
                fprintf(stderr, "CERunLoopReadTimeout fd get error, fd=%d\n", fd);
                fflush(stderr);
                abort();
            }
            fe->readTimer.prev = CEFileDescriptionInvalid;
            fd = fe->readTimer.next;
            fe->readTimer.next = CEFileDescriptionInvalid;

            CEFileDescription_s efd = {.fd = fd, .tag = fe->fdTag};
            
            if (0 == fe->isReadTimeout && 0 == fe->isWriteTimeout) {
                CERunLoopAppendFiredEvent(eventLoop, efd);
            }
            fe->isReadTimeout = 1;
        } else {
            fprintf(stderr, "CERunLoopReadTimeout fd error, fd=%d\n", fd);
            fflush(stderr);
            abort();
        }
    }
}

void CERunLoopWriteTimeout(CERunLoop_s * _Nonnull eventLoop, uint32_t timerId) {
    int fd = eventLoop->writeTimerPages[timerId];
    eventLoop->writeTimerPages[timerId] = CEFileDescriptionInvalid;

    while (CEFileDescriptionInvalid != fd) {
        if (fd >= 0) {
            CEFileEvent_s * fe = CERunLoopGetFileEventByIndex(eventLoop, fd);
            if (NULL == fe || NULL == fe->handler) {
                fprintf(stderr, "CERunLoopWriteTimeout fd get error, fd=%d\n", fd);
                fflush(stderr);
                abort();
            }
            fe->writeTimer.prev = CEFileDescriptionInvalid;
            fd = fe->writeTimer.next;
            fe->writeTimer.next = CEFileDescriptionInvalid;
            
            CEFileDescription_s efd = {.fd = fd, .tag = fe->fdTag};
            
            if (0 == fe->isReadTimeout && 0 == fe->isWriteTimeout) {
                CERunLoopAppendFiredEvent(eventLoop, efd);
            }
            fe->isWriteTimeout = 1;
        } else {
            fprintf(stderr, "CERunLoopWriteTimeout fd error, fd=%d\n", fd);
            fflush(stderr);
            abort();
        }
    }
}



#pragma mark - share

static CERunLoop_s * _Nonnull __CERunLoopShared = NULL;
void CERunLoopSharedCreate(void) {
    __CERunLoopShared = CERunLoopCreate(1024);
}
CERunLoop_s * _Nonnull CERunLoopShared(void) {
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, CERunLoopSharedCreate);
    return __CERunLoopShared;
}
