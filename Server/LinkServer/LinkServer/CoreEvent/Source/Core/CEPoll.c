//
//  CEPoll.m
//  CoreEvent
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#include "CEPollInternal.h"

#include "CEMemory.h"
#include "CEConfig.h"
#include "CEBaseApi.h"
#include "CEFileEvent.h"
#include "CEBaseApi.h"

extern void _CETimeEventPerform(CETimeEventRef _Nonnull ref);

extern CETimerQueue_s * _Nonnull CETimeEventQueueCreate(void);
static inline CETimeEventRef _Nullable _CETimerQueueGetFirst(CETimerQueue_s * _Nonnull queue) {
    if (queue->count <= 0) {
        return NULL;
    }
    return queue->buffer[0];
}

extern CCBool _CETimerQueueRemove(CETimerQueue_s * _Nonnull queue, CETimeEventRef _Nonnull event);
extern CCBool _CETimerQueueInsert(CETimerQueue_s * _Nonnull queue, CETimeEventRef _Nonnull event);

extern CETimeEventRef _Nullable _CETimerQueueRemoveFirst(CETimerQueue_s * _Nonnull queue);
extern CETimeEventRef _Nonnull _CETimerQueueRemoveIndex(CETimerQueue_s * _Nonnull queue, uint32_t index);
extern void _CETimerQueueUpdate(CETimerQueue_s * _Nonnull queue, CETimeEventRef _Nonnull ref, CCMicrosecondTime time);

/*
 我测试过 getsockopt(m_socket, SOL_SOCKET, SO_ERROR,(char*)&optval, &optlen);
 和 getsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE,(char*)&optval, &optlen);
 3. getsockopt

 int optval, optlen = sizeof(int);

 getsockopt(sd, SOL_SOCKET, SO_ERROR,(char*) &optval, &optlen);

 switch(optval){

       case 0:

         //“处于连接状态“

         break;

       case ECONNREFUSED:

       case EHOSTNUMREACH:

 }
 
 4. 项目中使用的情况

 // 判断是否连接

 int optval, optlen = sizeof(int);

 int r = getsockopt(g_sockfd, SOL_SOCKET, SO_ERROR,(char*) &optval, &optlen);

 // send HELLO 包

 if ((optval == 0) && (r == 0) && (send(g_sockfd,(char*)&g_DAUCommand,sizeof(struct _DAUCommand),0) != SOCKET_ERROR ))

 {

 ServiceLog.write_log("[threadOperationWaitTrasferAgain]: try to recv from dau...");

          length = recv(g_sockfd,(char *)&command,sizeof(struct _DAUCommand),0);

 }

 else

 {

          if (connect(g_sockfd,(struct sockaddr*)&g_serv_addr, sizeof(g_serv_addr) ) == 0)

          {

                   ServiceLog.write_log("[threadOperationWaitTrasferAgain]:connect to DAU  success.");

                    ServiceLog.write_log("[threadOperationWaitTrasferAgain]: try to recv from dau...");

                    length = recv(g_sockfd,(char *)&command,sizeof(struct _DAUCommand),0);

          }

          else

          {

                    ServiceLog.write_log("[threadOperationWaitTransferAgain]:can't connect to DAU, try again ");

                    length = 0;

                    continue;

          }

 }

 */


#pragma mark - file event api

CCBool CEPollTimerQueueIsFull(CEPollPtr _Nonnull poll) {
    assert(poll);
    CEPoll_s * p = poll;
    return p->timerQueue->count < CETimerQueueBufferSize;
}

CETimeEventRef _Nonnull CEPollAddTimeEvent(CEPollPtr _Nonnull poll,
                                           uint32_t mode,
                                           CEMicrosecondTime deadline,
                                           CEMicrosecondTime repeating,
                                           CCClosureRef _Nonnull closure) {
    assert(poll);
    CEPoll_s * p = poll;
    assert(mode <= 2);//0, 1, 2 有效
    CETimeEventRef ref = CCRefAllocate(CCDomain, CERefTypeTimeEvent, sizeof(CETimeEvent_s));
    CETimeEvent_s * content = CCRefGetContentPtr(ref);
    CCRefRetain(closure);
    content->isCanceled = 0;
    content->closure = closure;
    content->when = deadline;
    content->mode = mode;
    content->isFinished = 0;
    content->qIndex = CETimeEventQueueIndexInvalid;
    content->duration = repeating;
    assert(_CETimerQueueInsert(p->timerQueue, ref));
    CCRefRetain(ref);
    return ref;
}

void CEPollCancelTimeEvent(CEPollPtr _Nonnull poll, CETimeEventRef _Nonnull ref) {
    assert(poll);
    assert(ref);
    CEPoll_s * p = poll;
    CETimeEvent_s * content = CCRefGetContentPtr(ref);
    if (0 != content->isFinished) {
        content->isCanceled = 1;
    }
    if (_CETimerQueueRemove(p->timerQueue, ref)) {
        CCRefRelease(ref);
    }
}
CCBool CEPollIsValidTimeEvent(CEPollPtr _Nonnull poll, CETimeEventRef _Nonnull ref) {
    assert(poll);
    assert(ref);
    CEPoll_s * p = poll;
    CETimeEvent_s * content = (CETimeEvent_s *)CCRefGetContentPtr(ref);
    uint32_t index = content->qIndex;
    return (index < p->timerQueue->count);
}


#pragma mark - file event api


CEResult_t CEPollAddFile(CEPollPtr _Nonnull poll, uint32_t handlerId, int fd, uint32_t context) {
    assert(poll);
    CEPoll_s * p = poll;
    if (fd < 0 || fd >= p->fileTableSize) {
        CELogWarning("CEPollAddFile error, unsupport with fd: %d\n", fd);
        return CEResultErrorParams;
    };
    
    if (handlerId >= p->handlerCount) {
        CELogWarning("CEPollAddFile error, unsupport with handleId: %d\n", handlerId);
        return CEResultErrorParams;
    };
    if (context > CEPollFileContextMax) {
        CELogWarning("CEPollAddFile error, unsupport with fidPtr: null\n");
        return CEResultErrorParams;
    };
    assert(poll);
    CEFile_t * file = __CEPollGetFileWithIndex(p, fd);
    if (file->handler != 0) {
        return CEResultErrorFileIdInvalid;
    }
    file->handler = handlerId;
    file->mask = 0;
    file->context = context;
    return CEResultSuccess;
}

static inline CEResult_t _CEPollAddFileEventMask(CEPoll_s * _Nonnull p, int fd, CEFileEventMask_es mask) {
    assert(p);
    CEFile_t * file = _CEPollGetFile(p, fd);
    if (NULL == file) {
        return CEResultErrorFileId;
    }
    CEApiAddEvent(p->api, fd, file->mask, mask);
    file->mask |= mask;
    return CEResultSuccess;
}

static inline CEResult_t _CEPollRemoveFileEventMask(CEPoll_s * _Nonnull p, int fd, CEFileEventMask_es mask) {
    assert(p);
    CEFile_t * file = _CEPollGetFile(p, fd);
    if (NULL == file) {
        return CEResultErrorFileId;
    }
    if (file->mask == CEFileEventMaskNone) {
        return CEResultSuccess;
    };
    file->mask = file->mask & (~mask);
    CEApiRemoveEvent(p->api, fd, mask);
    return CEResultSuccess;
}


CEResult_t CEPollRemoveFile(CEPollPtr _Nonnull poll, int fd) {
    assert(poll);
    CEPoll_s * p = poll;
    CEFile_t * file = _CEPollGetFile(p, fd);
    if (NULL == file) {
        return CEResultErrorFileId;
    }
    CEApiRemoveEvent(p->api, fd, CEFileEventMaskReadWritable);
    file->handler = CEFileHandlerInvalid;
    return CEResultSuccess;
}


CEResult_t CEPollAddFileEventMask(CEPollPtr _Nonnull poll, int fd, CEFileEventMask_es mask) {
    assert(poll);
    CEPoll_s * p = poll;
    return _CEPollAddFileEventMask(p, fd, mask);
}

CEResult_t CEPollRemoveFileEventMask(CEPollPtr _Nonnull poll, int fd, CEFileEventMask_es mask) {
    assert(poll);
    CEPoll_s * p = poll;
    return _CEPollRemoveFileEventMask(p, fd, mask);
}

#pragma mark - run loop async api


void CEPollWakeUp(CEPoll_s * _Nonnull p) {
    assert(p);
    CEApiWakeUp(p->api);
}
static inline void CEBlockQueueLock(CEBlockQueue_s * _Nonnull queue) {
    assert(queue);
#if __APPLE__
    os_unfair_lock_lock(&(queue->blockQueueLock));
#else
    pthread_spin_lock(&(queue->blockQueueLock));
#endif
}

static inline void CEBlockQueueUnlock(CEBlockQueue_s * _Nonnull queue) {
    assert(queue);
#if __APPLE__
    os_unfair_lock_unlock(&(queue->blockQueueLock));
#else
    pthread_spin_unlock(&(queue->blockQueueLock));
#endif
}

CEBlockQueuePage_s * _Nullable CEBlockQueueRemoveAll(CEBlockQueue_s * _Nonnull queue) {
    assert(queue);
    CEBlockQueueLock(queue);
    CEBlockQueuePage_s * page = queue->begin;
    queue->begin = NULL;
    queue->end = NULL;
    CEBlockQueueUnlock(queue);
    return page;
}

void CEBlockQueueAppend(CEBlockQueue_s * _Nonnull queue, CCClosureRef _Nonnull closure) {
    assert(queue);
    CEBlockQueueLock(queue);
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
    page->buffer[page->used] = closure;
    page->used ++;
    CEBlockQueueUnlock(queue);
}


void CEPollAsync(CEPollPtr _Nonnull poll, CCClosureRef _Nonnull closure) {
    assert(poll);
    assert(closure);
    CEPoll_s * p = poll;

    CCRefRetain(closure);
    CEBlockQueueAppend(&(p->blockQueue), closure);
    CEPollWakeUp(p);
}

#pragma mark - api callback

struct CEApiPollContext {
    CEPoll_s * _Nonnull poll;
    CCRange16 * ranges;
    uint32_t count;
};

_Bool CEApiFileEventMapperCallback(void * _Nullable context, void * _Nonnull api, CCPtr _Nonnull buffer, int fd, CEFileEventMask_es mask) {
    assert(context);
    if (mask == CEFileEventMaskNone) {
        return false;
    }
    CEPoll_s * p = ((struct CEApiPollContext *)context)->poll;
    CCRange16 * ranges = ((struct CEApiPollContext *)context)->ranges;
    assert(p);
    assert(ranges);
    assert(fd < p->fileTableSize);

    CEFile_t * file = __CEPollGetFileWithIndex(p, fd);
    assert(file->handler > 0);

    if (CEFileEventMaskReadWritable & mask) {
        uint32_t oldStatus = __CEPollGetStatus(p, fd);
        uint32_t newStatus = oldStatus;
        if (CEFileEventMaskReadable & mask) {
            newStatus |= CEFileEventMaskReadable;
        }
        if (CEFileEventMaskWritable & mask) {
            newStatus |= CEFileEventMaskWritable;
        }
        __CEPollSetStatus(p, fd, newStatus);
        if (0 == oldStatus && 0 != newStatus) {
            CEFile_t file = p->fileTable[fd];
            int * item = buffer;
            *item = fd;
            assert(file.handler < p->handlerCount);
            ranges[file.handler].length += 1;
            ((struct CEApiPollContext *)context)->count += 1;
        }
    }
    return true;
}

void CEApiPipeEventCallback(void * _Nullable context, void * _Nonnull api) {
    assert(context);
    CELogVerbose2(128, "CEApiPipeEventCallback");
}


#pragma mark - runloop



int64_t _CEPollDoBlocks(CEPoll_s * _Nonnull p) {
    CEBlockQueuePage_s * page = NULL;
    page = CEBlockQueueRemoveAll(&(p->blockQueue));
    
    int64_t count = 0;
    while (NULL != page) {
        CEBlockQueuePage_s * nextPage = page->next;
        for (size_t i=0; i<page->used; i++) {
            count += 1;
            
            CCClosureRef closure = page->buffer[i];
            CCClosurePerform(closure, NULL, NULL);
            CCRefRelease(closure);
        }
        CEDeallocate(page);
        page = nextPage;
    }
    return count;
}

int64_t _CEPollDoTimers(CEPoll_s * _Nonnull p) {
    int64_t processed = 0;
    
    CETimerQueue_s * queue = p->timerQueue;
    CETimeEventRef * ref = _CETimerQueueGetFirst(queue);
    CEMicrosecondTime now = p->currentTime;
    int64_t fired = 0;
    while (NULL != ref) {
        CETimeEvent_s * event = (CETimeEvent_s *)CCRefGetContentPtr(ref);
        if (event->when <= now) {
            if (event->isCanceled) {
                if (_CETimerQueueRemove(queue, ref)) {
                    CCRefRelease(ref);
                }
            } else {
                assert(0 == event->isFinished);
                _CETimeEventPerform(ref);
                if (0 == event->isCanceled) {
                    if (CETimeEventModeRepeatFixedRate == event->mode) {
                        _CETimerQueueUpdate(queue, ref, event->when + event->duration);
                    } else if (CETimeEventModeRepeatFixedDelay == event->mode) {
                        _CETimerQueueUpdate(queue, ref, CEGetCurrentTime() + event->duration);
                    } else if (CETimeEventModeDelay == event->mode) {
                        event->isFinished = true;
                    } else {
                        CELogError("CETimeEventPerform error");
                        abort();
                    }
                }
                if (0 != event->isCanceled || 0 != event->isFinished) {
                    if (_CETimerQueueRemove(queue, ref)) {
                        CCRefRelease(ref);
                    }
                }
                ref = _CETimerQueueGetFirst(queue);
            }
            fired += 1;
        } else {
            break;
        }
    }
    return processed;
}


//bufferSize 缓冲区长度， 不是个数
int __CEPollDoCheckSource(CEPoll_s * _Nonnull p, struct CEApiPollContext * _Nonnull context, CEFileFiredInfo * _Nonnull buffer, uint32_t bufferSize) {
    static const CEApiPollCallback_s callback = {
        .filterMapper = CEApiFileEventMapperCallback,
        .pipeCallback = CEApiPipeEventCallback,
    };
    
    CEMicrosecondTime now = CEGetCurrentTime();
    CEMicrosecondTime timeout = 1;
    if (now < p->currentTime + CEFrameIntervalDefault) {
        CETimeEvent_s * ref = _CETimerQueueGetFirst(p->timerQueue);
        if (NULL != ref) {
            CETimeEvent_s * event = (CETimeEvent_s *)CCRefGetContentPtr(ref);
            if (event->when > now) {
                timeout = 1;
            } else {
                timeout = event->when - now;
            }
        } else {
            timeout = CEFrameIntervalDefault - p->fileTableSize / 32 * CEFrameIntervalPer32;
        }
    }
    memset(p->fileStatusTable, 0, p->fileStatusTableSize);
    return CEApiPoll(p->api, timeout, p->tmpBuffer, p->tmpBufferSize, sizeof(int), context, &callback);
}

int64_t _CEPollDoSource(CEPoll_s * _Nonnull p) {
    assert(p);
    uint16_t counts[CEPollFileHandlerTableSize] = {};
    CCRange16 ranges[CEPollFileHandlerTableSize] = {};
    struct CEApiPollContext context = {
        .poll = p,
        .ranges = ranges,
        .count = 0,
    };

    CEFileFiredInfo * info = p->tmpBuffer;
    int processed = __CEPollDoCheckSource(p, &context, info, p->tmpBufferSize);
//    p->currentTime = CEGetCurrentTime();
//    assert(ranges[0].length == 0);
//    uint32_t offset = 0;
//    for (int i=1; i<p->handlerCount; i++) {
//        ranges[i].location = (uint16_t)offset;
//        offset += ranges[i].length;
//        assert(offset < 0x10000);
//    }
//    CEFileId * ids = (CEFileId *)(info + context.count);
//    
//    for (int j = 0; j < context.count; j++) {
//        CEFileFiredInfo * infoItem = info + j;
//        CEFileId * item = ids + ranges[infoItem->handler].location + counts[infoItem->handler];
//        *item = infoItem->id;
//        counts[infoItem->handler] += 1;
//    }
//    
//    assert(counts[0] == 0);
//    for (int i=1; i<p->handlerCount; i++) {
//        assert(counts[i] == ranges[i].length);
//        if (ranges[i].length > 0) {
//            CEFileId * item = ids + ranges[i].location;
//            printf("handlerId:%d, eventCount:%hu", i, ranges[i].length);
//            CEFileEventHandler_s * handler = &(p->fileEventHandlers[i]);
//            handler->handleFileEvent(handler->context, item, counts[i]);
//        }
//    }
//    
//    for (int j = 0; j < context.count; j++) {
//        CEFileId fid = ids[j];
////        CEFile_t * file = _CEPollGetFile(p, id);
////        file->read.isFired = 0;
////        file->write.isFired = 0;
//    }
//    
//    
//    
//    for (int j = 0; j < processed; j++) {
//        CEFileFiredInfo * item = info + j;
//
//        __CEPollGetFileWithIndex(p, item->id.in);
//
//        CEFileEvent_s *fe = CERunLoopGetFileEvent(eventLoop, fd);
//        if (NULL != fe) {
//            if (fe->readable || fe->writable) {
//                CEFileEventInfo_s info = {
//                    .readable = fe->readable,
//                    .writable = fe->writable,
//                    .isReadTimeout = 0,
//                    .isWriteTimeout = 0,
//                    .xxx = 0,
//                };
//                fe->readable = 0;
//                fe->writable = 0;
//                fe->handler(eventLoop, fd, fe->clientData, info);
//                processed++;
//            }
//        }
//    }
//    eventLoop->firedCount = 0;
    return processed;
}

static const char * const _Nullable CEPollProgressName[4] = {
    "CEPollProgress.doSource",
    "CEPollProgress.doBlock",
    "CEPollProgress.doTimer",
};
static inline void _CEPollProcessBeginNew(CEPoll_s * _Nonnull p, CEPollProgress_t new) {
    p->progress = new;
    CELogVerbose2(0, "%s begin", CEPollProgressName[new]);
}

int64_t CEPollProcessEvents(CEPoll_s * _Nonnull p) {
    int64_t processed = 0;
    
    _CEPollProcessBeginNew(p, CEPollProgressDoSource);
    processed += _CEPollDoSource(p);

    _CEPollProcessBeginNew(p, CEPollProgressDoBlock);
    processed += _CEPollDoBlocks(p);
    
    _CEPollProcessBeginNew(p, CEPollProgressDoTimer);
    processed += _CEPollDoTimers(p);

    return processed; /* return the number of processed file/time events */
}

#pragma mark - init

void * CEPollRunloopRun(void * args) {
    CEPoll_s * _Nonnull p = (CEPoll_s *)args;
    while (1) {
        int64_t count = CEPollProcessEvents(p);
        printf("CEPollProcessEvents count: %lld\n", count);
    }
    return NULL;
}

pthread_t CEPollCreateThread(CEPoll_s * _Nonnull p) {
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
//PTHREAD_STACK_MIN;
    size_t rStacksize = 1024 * 1024 * 4;
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
    
    result = pthread_create(&tid, &attr, CEPollRunloopRun, p);
    if (0 != result) {
        CELogError("pthread_create %s\n", strerror(result));
        return 0;
    }
    return tid;
}

static const CEFile_t CEFileInvalid = {
    .context = 0,
    .handler = CEFileHandlerInvalid,
    .mask = 0,
};
CEPoll_s * _Nonnull CEPollCreate(uint32_t setsize) {
    if (setsize < 64) {
        setsize = 64;
    }
    struct rlimit limit;
    if(getrlimit(RLIMIT_NOFILE, &limit) != 0) {
        CELogError("getrlimit RLIMIT_NOFILE error %s; \n", strerror(errno))
        abort();
    }
    CELogVerbose("getrlimit RLIMIT_NOFILE max: %llu, current: %llu\n", limit.rlim_max, limit.rlim_cur)
    
//    int mib[2] = {CTL_KERN, KERN_MAXFILESPERPROC};
//    int maxFilesPerProc = 0;
//    size_t size = sizeof(maxFilesPerProc);
//    if (sysctl(mib, 2, &maxFilesPerProc, &size, NULL, 0) != 0) {
//        CELogError("sysctl KERN_MAXFILESPERPROC error %s; \n", strerror(errno));
//        abort();
//    }
//    printf("maxFilesPerProc: %ld\n", maxFilesPerProc);

    if (setsize > limit.rlim_max) {
        assert(limit.rlim_max >= 64);
        setsize = limit.rlim_max < 0xffff ? (uint32_t)limit.rlim_max : 0xffff;
    }
    limit.rlim_cur = setsize;
    if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
        CELogError("setrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
        abort();
    } else {
        CELogInfo("success to setrlimit rlim_cur %lld; \n", limit.rlim_cur);
    }
    
    void * api = CEApiCreate();

    CEPoll_s * poll = (CEPoll_s *)CEAllocateClear(sizeof(CEPoll_s));
    poll->fileTableSize = setsize;
    poll->fileTable = CEAllocateClear(sizeof(CEFile_t) * setsize);
    
    poll->fileStatusTableSize = sizeof(CCUInt8) * ((setsize + 3) / 4);
    poll->fileStatusTable = CEAllocateClear(poll->fileStatusTableSize);
    poll->tmpBufferSize = CEApiGetEventItemSize() * setsize;
    poll->tmpBuffer = CEAllocateClear(poll->tmpBufferSize);
    
    CEFile_t * iter = poll->fileTable;
    for (uint32_t index=0; index<poll->fileTableSize; index++) {
        *iter = CEFileInvalid;
        iter += 1;
    }
    poll->fileTableSize = setsize;
    poll->currentTime = CEGetCurrentTime();

    poll->blockQueue.begin = NULL;
    poll->blockQueue.end = NULL;
#if __APPLE__
    poll->blockQueue.blockQueueLock = OS_UNFAIR_LOCK_INIT;
#else
    pthread_spin_init(&(poll->blockQueue.blockQueueLock), PTHREAD_PROCESS_PRIVATE);
#endif
    
    poll->api = api;

    poll->timerQueue = CETimeEventQueueCreate();
    //pipe
    //https://www.cnblogs.com/kunhu/p/3608109.html
    
    //eventfd
    //https://www.cnblogs.com/zhengchunhao/p/5335914.html
    
    //timerfd 和 eventfd接口实现进程线程通信
    //http://www.cnblogs.com/meihao1203/p/9368398.html
    
    //https://wenku.baidu.com/view/ab78e41ff11dc281e53a580216fc700abb6852be.html
    
    //signalfd
    //https://www.aliyun.com/jiaocheng/132736.html?spm=5176.100033.2.5.56a42e7aP3DY2O
    poll->thread = CEPollCreateThread(poll);
    return poll;
}



#pragma mark - share

uint32_t CEPollGetSetSize(CEPollPtr _Nonnull poll);

static uint32_t __CEPollSharedSetSize = 256;

void CEPollUpdateSetSize(uint32_t setsize) {
    __CEPollSharedSetSize = setsize;
}

static CEPoll_s * _Nonnull __CEPollShared = NULL;
void CEPollSharedCreate(void) {
    __CEPollShared = CEPollCreate(__CEPollSharedSetSize);
}
CEPollPtr _Nonnull CEPollShared(void) {
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, CEPollSharedCreate);
    return __CEPollShared;
}

uint32_t CEPollRegisterHandler(CEPollPtr _Nonnull poll, CCPtr _Nullable context, CEPollFileEventCallback_f _Nonnull handleFileEvent) {
    CEPoll_s * p = poll;
    if (p->handlerCount < CEPollFileHandlerTableSize-1) {
        uint32_t id = p->handlerCount;
        CEFileEventHandler_s * handler = &(p->fileEventHandlers[id]);
        handler->context = context;
        handler->handleFileEvent = handleFileEvent;
        p->handlerCount += 1;
        return id;
    } else {
        return CEFileHandlerInvalid;
    }
}









