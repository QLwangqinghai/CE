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

extern void CETimeEventStop(CETimeEvent_s * _Nonnull te);

extern CEFileEvent_s * _Nullable CERunLoopGetFileEvent(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);
extern CEFileEvent_s * _Nullable CERunLoopGetFileEventByIndex(CERunLoop_s * _Nonnull eventLoop, int index);

void CERunLoopUpdateApiTimer(CERunLoop_s * _Nonnull eventLoop);

CEBlock_s * _Nullable CEBlockQueueRemoveAll(CEBlockQueue_s * _Nonnull queue);
CEBlock_s * _Nullable CEBlockQueueAppend(CEBlockQueue_s * _Nonnull queue, CEBlock_s * block);
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

static inline CEFiredEvent_s * _Nullable CERunLoopGetFiredEventByIndex(CERunLoop_s * _Nonnull eventLoop, int index) {
    if (index >= eventLoop->setsize || index < 0) {
        return NULL;
    }
    uint32_t offset = (uint32_t)index;
    return &(eventLoop->firedPages[offset >> 16][offset & 0xffff]);
}

#pragma mark - time

static uint64_t CEGetMicrosecondsTime(void) {
    return CESystemBootInterval();
}
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

static uint64_t CEAddMicrosecondsToNow(uint64_t milliseconds) {
    uint64_t time = CEGetMicrosecondsTime();
    return time + milliseconds;
}

#pragma mark - thread key-specific

void __CESharedValueDealloc(void * value) {
}

static pthread_key_t __CESharedKey = 0;
void __CESharedKeyOnceBlockFunc(void) {
    int result = pthread_key_create(&__CESharedKey, __CESharedValueDealloc);
    assert(result == 0);
}
pthread_key_t CESharedKey(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CESharedKeyOnceBlockFunc);
    return __CESharedKey;
}

CERunLoop_s * _Nullable CEGetCurrentLoop(void) {
    return (CERunLoop_s *)pthread_getspecific(CESharedKey());

}
void CESetCurrentLoop(CERunLoop_s * _Nullable loop) {
    CERunLoop_s * old = CEGetCurrentLoop();
    if (NULL != old && NULL != loop & old != loop) {
        fprintf(stderr, "SetCurrentLoop error\n");
        fflush(stderr);
        abort();
    }
    int result = pthread_setspecific(CESharedKey(), loop);
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
void CEApiTimerEventCallback(void * _Nullable context, void * _Nonnull api) {
    assert(context);
    CERunLoop_s * eventLoop = (CERunLoop_s *)context;
    eventLoop->timerEvent = 1;
    CELogVerbose2(128, "CEApiTimerEventCallback");
}

void CEApiPipeEventCallback(void * _Nullable context, void * _Nonnull api) {
    assert(context);
    CERunLoop_s * eventLoop = (CERunLoop_s *)context;
    eventLoop->blockEvent = 1;
    CELogVerbose2(128, "CEApiPipeEventCallback");
}


#pragma mark - run loop life


CERunLoop_s * _Nonnull CERunLoopCreate(int setsize) {
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
        if (eventLoop->beforesleep != NULL) {
            eventLoop->beforesleep(eventLoop);
        }
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


#pragma mark - run loop api

CEResult_t CERunLoopAddTimeEvent(CERunLoop_s * _Nonnull eventLoop, uintptr_t event, uint64_t delay) {
    CETimeEvent_s * te = (CETimeEvent_s *)event;
    
    if (NULL == eventLoop || NULL == te) {
        return CEResultErrorParams;
    }
    
    if (1 == te->running || te->owner != (uintptr_t)eventLoop) {
        return CEResultErrorParams;
    }
    
    te->running = 1;
    te->when = CEAddMicrosecondsToNow(delay);

    CETimeEventQueue_s * queue = &(eventLoop->timeEventQueue);
    
    CETimeEvent_s * iter = NULL;
    if (CETimeEventQueueProgressExecute == queue->progress) {
        iter = queue->iter->next;
        if (NULL == iter) {
            iter = queue->first;
        }
    } else {
        iter = queue->first;
    }

    if (iter) {
        if (iter->when > te->when) {
            CETimeEventQueueInsertAt(queue, iter, te);
        } else {
            while (NULL != iter) {
                if (iter->when <= te->when) {
                    CETimeEventQueueAppendAt(queue, iter, te);
                    break;
                }
                iter = iter->next;
            }
        }
    } else {
        CETimeEventQueueAppend(queue, te);
    }
    
    if (CETimeEventQueueProgressNone == queue->progress) {
        CERunLoopUpdateApiTimer(eventLoop);
    }
    return CEResultSuccess;
}

CEResult_t CERunLoopRemoveTimeEvent(CERunLoop_s * _Nonnull eventLoop, uintptr_t event) {
    CETimeEvent_s * te = (CETimeEvent_s *)event;
    if (NULL == eventLoop || NULL == te) {
        return CEResultErrorParams;
    }
    if (te->owner != (uintptr_t)eventLoop) {
        return CEResultErrorParams;
    }
    if (0 == te->running) {
        return CEResultSuccess;
    }
    _Bool needDeinit = CETimeEventStatesExecuting != te->states;
    CETimeEventStop(te);
    
    if (needDeinit) {
        CETimeEventQueueRemove(&(eventLoop->timeEventQueue), te);
        te->running = 0;
        
        CEResult_t result = CETimeEventDeinit((uintptr_t)te);
        assert(result == CEResultSuccess);
    }

    return CEResultSuccess;
}

/* Wait for milliseconds until the given file descriptor becomes
 * writable/readable/exception */
CEFileEventMask_es CEWait(int fd, CEFileEventMask_es mask, int milliseconds) {
    return CEApiWait(fd, mask, milliseconds);
}

char *CERunLoopGetApiName(void) {
    return CEApiName();
}


#pragma mark - run loop process events

/* Search the first timer to fire.
 * This operation is useful to know how many time the select can be
 * put in sleep without to delay any event.
 * If there are no timers NULL is returned.
 *
 * Note that's O(N) since time events are unsorted.
 * Possible optimizations (not needed by Redis so far, but...):
 * 1) Insert the event in order, so that the nearest is just the head.
 *    Much better but still insertion or deletion of timers is O(N).
 * 2) Use a skiplist to have this operation as O(1) and insertion as O(log(N)).
 */
static CETimeEvent_s *aeSearchNearestTimer(CERunLoop_s *eventLoop) {
    CETimeEvent_s *te = eventLoop->timeEventQueue.first;
    return te;
}

int64_t CERunLoopDoBlocks(CERunLoop_s *eventLoop) {
    
    CEBlock_s * block = NULL;
    
    CERunLoopBlockQueueLock(eventLoop);
    block = CEBlockQueueRemoveAll(&(eventLoop->blockQueue));
    CERunLoopBlockQueueUnlock(eventLoop);
    
    int64_t count = 0;
    
    CEBlock_s * tmp = block;

    while (NULL != tmp) {
        count += 1;
        tmp->execute(eventLoop, tmp->context);
        tmp = tmp->next;
    }
    
    tmp = block;
    while (NULL != tmp) {
        if (tmp->dealloc) {
            tmp->dealloc(eventLoop, tmp->context);
        }
        tmp = tmp->next;
    }
    
    return count;
}

int64_t _CERunLoopDoTimers(CERunLoop_s * _Nonnull eventLoop) {
    int64_t processed = 0;
    CETimeEvent_s * firedLastTimer = NULL;
    uint64_t now = CEGetMicrosecondsTime();
    
//    printf("now: %llu\n", now);
    
    eventLoop->microsecondsTime = now;
    
    CETimeEventQueue_s * queue = &(eventLoop->timeEventQueue);

    
    queue->iter = queue->first;
    
    if (NULL != queue->iter) {
        queue->progress = CETimeEventQueueProgressExecute;
    }
    
    int64_t fired = 0;
    while (NULL != queue->iter) {
//        printf("when:%llu  now: %llu %ld \n  ", iter->when, now, iter->when <= now ? 1 : 0);
        if (queue->iter->when <= now) {
//            printf("CETimeEventExecute\n");
            
            if (CETimeEventExecute((uintptr_t)queue->iter)) {
                processed += 1;
            }
            firedLastTimer = queue->iter;
            queue->iter = queue->iter->next;
            fired += 1;
        } else {
            break;
        }
    }
    
    queue->progress = CETimeEventQueueProgressDeinitInvalid;

    
    CETimeEventQueue_s invalidQueue = {};
    CETimeEventQueue_s queueA = {};
    CETimeEventQueue_s queueB = {};
    
    //拆分
    if (NULL != firedLastTimer) {
        queueA.first = queue->first;
        queueA.last = firedLastTimer;

        if (NULL != firedLastTimer->next) {
            queueB.first = firedLastTimer->next;
            queueB.last = queue->last;
            CETimeEventUnlink(firedLastTimer, firedLastTimer->next);
        }
    } else {
        queueB.first = queue->first;
        queueB.last = queue->last;
    }
    queue->first = NULL;
    queue->last = NULL;
    
    
    //剔除无效的timer, 重设触发时间
    queueA.iter = queueA.first;
    while (NULL != queueA.iter) {
        CETimeEvent_s * item = queueA.iter;
        queueA.iter = queueA.iter->next;

        if (CETimeEventStatesWait != item->states) {
            CETimeEventQueueRemove(&queueA, item);
            item->running = 0;
            CETimeEventQueueAppend(&invalidQueue, item);
        } else {
            if (item->repeatMode == CETimeEventRepeatModeAfter) {
                item->when = now + item->interval;
            } else {
                item->when += item->interval;
            }
        }
    }
    
    CETimeEventQueue_s sortedQueueA = {};

    //firedList 排序
    if (NULL != queueA.first) {
        while (NULL != queueA.first) {
            queueA.iter = queueA.first;
            CETimeEvent_s * selected = queueA.iter;

            while (NULL != queueA.iter) {
                if (queueA.iter->when < selected->when) {
                    selected = queueA.iter;
                }
                queueA.iter = queueA.iter->next;
            }
            CETimeEventQueueRemove(&queueA, selected);
            CETimeEventQueueAppend(&sortedQueueA, selected);
        };
    }
    
    
    //合并链表
    sortedQueueA.iter = sortedQueueA.first;
    queueB.iter = queueB.first;

    while (NULL != sortedQueueA.iter || NULL != queueB.iter) {
        CETimeEvent_s * item = NULL;
        if (NULL != sortedQueueA.iter && NULL != queueB.iter) {
            if (sortedQueueA.iter->when < queueB.iter->when) {
                item = sortedQueueA.iter;
                sortedQueueA.iter = sortedQueueA.iter->next;
                CETimeEventQueueRemove(&sortedQueueA, item);
            } else {
                item = queueB.iter;
                queueB.iter = queueB.iter->next;
                CETimeEventQueueRemove(&queueB, item);
            }
        } else if (NULL != sortedQueueA.iter) {
            item = sortedQueueA.iter;
            sortedQueueA.iter = sortedQueueA.iter->next;
            CETimeEventQueueRemove(&sortedQueueA, item);
        } else {//NULL != iterB
            item = queueB.iter;
            queueB.iter = queueB.iter->next;
            CETimeEventQueueRemove(&queueB, item);
        }
        CETimeEventQueueAppend(queue, item);
    }

    
    //剔除无效的timer, 重设触发时间
    invalidQueue.iter = invalidQueue.first;
    while (NULL != invalidQueue.iter) {
        CETimeEvent_s * item = invalidQueue.iter;
        invalidQueue.iter = invalidQueue.iter->next;
        CETimeEventDeinit((uintptr_t)item);
    }
    
    queue->progress = CETimeEventQueueProgressNone;

    if (fired > 0) {
        CERunLoopUpdateApiTimer(eventLoop);
    }
    
    return processed;
}

int64_t CERunLoopDoTimers(CERunLoop_s * _Nonnull eventLoop) {
    return _CERunLoopDoTimers(eventLoop);
}

void CERunLoopDoCheckSource(CERunLoop_s * _Nonnull eventLoop) {
    CETimeEvent_s *shortest = NULL;
    struct timeval tv;
    struct timeval * tvp;
    
    shortest = aeSearchNearestTimer(eventLoop);
    tv = CEApiPollDefaultTime;
    tvp = &tv;
    
    static const CEApiPoolCallback_s callback = {
        .fileEventCallback = CEApiFileEventCallback,
        .pipeCallback = CEApiPipeEventCallback,
        .timerCallback = CEApiTimerEventCallback,
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
 * Without special flags the function sleeps until some file event
 * fires, or when the next time event occurs (if any).
 *
 * If flags is 0, the function does nothing and returns.
 * if flags has AE_ALL_EVENTS set, all the kind of events are processed.
 * if flags has AE_FILE_EVENTS set, file events are processed.
 * if flags has AE_TIME_EVENTS set, time events are processed.
 * if flags has AE_DONT_WAIT set the function returns ASAP until all
 * if flags has AE_CALL_AFTER_SLEEP set, the aftersleep callback is called.
 * the events that's possible to process without to wait are processed.
 *
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
    uint64_t checkBegin = CESystemBootInterval();
    CERunLoopDoCheckSource(eventLoop);
    uint64_t checkEnd = CESystemBootInterval();
    CELogVerbose2(64, "CERunLoopDoCheckSource used time: %llu", checkEnd - checkBegin);
    
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

void CERunLoopUpdateApiTimer(CERunLoop_s * _Nonnull eventLoop) {
    CETimeEventQueue_s timeEventQueue = eventLoop->timeEventQueue;

    if (NULL == timeEventQueue.first) {
        return;
    }
    
    timeEventQueue.iter = eventLoop->timeEventQueue.first;
    
    uint64_t leeway = timeEventQueue.iter->leeway;
    uint64_t when = timeEventQueue.iter->when;
    timeEventQueue.iter = timeEventQueue.iter->next;

    while (timeEventQueue.iter) {
        if (timeEventQueue.iter->when + timeEventQueue.iter->leeway < when + leeway) {
            leeway = timeEventQueue.iter->leeway;
            when = timeEventQueue.iter->when;
        }
        if (timeEventQueue.iter->when > when + leeway) {
            break;
        }
        
        timeEventQueue.iter = timeEventQueue.iter->next;
    }
    CEApiResult_t apiResult = CEApiUpdateTimer(eventLoop->api, when + leeway);
    if (CEApiResultSuccess != apiResult) {
        CELogWarning("CEApiUpdateTimer failure %s;", strerror(errno));
    }
}


void CESetBeforeSleepProc(CERunLoop_s * _Nonnull eventLoop, CEStateChangeHandler_f _Nullable beforesleep) {
    eventLoop->beforesleep = beforesleep;
}

void CESetAfterSleepProc(CERunLoop_s * _Nonnull eventLoop, CEStateChangeHandler_f _Nullable aftersleep) {
    eventLoop->aftersleep = aftersleep;
}



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

void CERunLoopAsync(CERunLoop_s * _Nonnull eventLoop, CEBlockExecute_f _Nonnull execute, CEBlockDealloc_f _Nullable dealloc, void * _Nullable context) {
    assert(eventLoop);
    assert(execute);

    CERunLoopBlockQueueLock(eventLoop);
    
    CEBlock_s * block = CEAllocate(sizeof(CEBlock_s));
    block->next = NULL;
    block->execute = execute;
    block->dealloc = dealloc;
    block->context = context;
    
    CEBlockQueueAppend(&(eventLoop->blockQueue), block);
    CERunLoopBlockQueueUnlock(eventLoop);
}



CEBlock_s * _Nullable CEBlockQueueRemoveAll(CEBlockQueue_s * _Nonnull queue) {
    CEBlock_s * block = queue->begin;
    queue->begin = NULL;
    queue->end = NULL;
    return block;
}

CEBlock_s * _Nullable CEBlockQueueAppend(CEBlockQueue_s * _Nonnull queue, CEBlock_s * block) {
    if (NULL == queue->begin) {
        assert(NULL == queue->end);
        queue->begin = block;
        queue->end = block;
    } else {
        assert(NULL != queue->end);

        queue->end->next = block;
        queue->end = block;
    }
    return block;
}


void * CERunLoopRunThread(void * args) {
    CERunLoop_s * eventLoop = (CERunLoop_s *)args;

    CERunLoopRun(eventLoop);
    return NULL;
}

int CERunLoopMakeThreadRunDefault(CERunLoop_s * _Nonnull eventLoop) {
    pthread_t tid;
    pthread_attr_t attr;
    struct sched_param param;
    //初始化线程属性
    pthread_attr_init(&attr);
    int policy;
    int result = 0;
    
    result = pthread_getschedparam(pthread_self(), &policy, &param);
    if (0 != result) {
        printf("pthread_getschedparam %s\n", strerror(result));
        return result;
    }
    result = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    if (0 != result) {
        printf("pthread_attr_setscope PTHREAD_SCOPE_SYSTEM %s\n", strerror(result));
        return result;
    }
    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (0 != result) {
        printf("pthread_attr_setdetachstate PTHREAD_CREATE_DETACHED %s\n", strerror(result));
        return result;
    }
    
    int priority = sched_get_priority_max(policy);
    assert (priority != -1);
    param.sched_priority = priority;
    
    result = pthread_attr_setschedparam(&attr, &param);
    if (0 != result) {
        printf("pthread_attr_setschedparam %s\n", strerror(result));
        return result;
    }
    
    result = pthread_create(&tid, &attr, CERunLoopRunThread, eventLoop);
    if (0 != result) {
        printf("pthread_create %s\n", strerror(result));
    }
    return result;
}


void CERunLoopRunDefault(void) {
    CERunLoop_s * eventLoop = CERunLoopShared();

    uint32_t runningStates = atomic_load(&(eventLoop->runningStates));
    if (0 != runningStates) {
        return;
    }
    CERunLoopMakeThreadRunDefault(eventLoop);
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
