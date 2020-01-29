//
//  CEPoll.m
//  CoreEvent
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#import "CEPoll.h"

#include "CEMemory.h"
#include "CEConfig.h"
#include "CEBaseApi.h"
#include "CETimeEvent.h"
#include "CEFileEvent.h"
#include "CEBaseApi.h"


typedef uint32_t CETimerTimeInterval;

#define CEPollFileTimerHashShift 2
#define CEPollFileTimerTableSize 0x10000
#define CEPollFileHandlerTableSize 0x1000

#pragma pack(push, 2)
typedef struct {
    uint16_t prev;
    uint16_t next;
} CEFileLink_s;


/*
 none
 checking
 timeout
 */
//typedef uint32_t CEFileTimerStatus_t;
//const CEFileTimerStatus_t CEFileTimerStatusNone = 0;
//const CEFileTimerStatus_t CEFileTimerStatusNormal = 1;
//const CEFileTimerStatus_t CEFileTimerStatusTimeout = 2;



/* File event structure 16B*/
typedef struct {
    uint32_t sequence: 16;
    uint32_t handler: 12;
    uint32_t isValid: 1;
    uint32_t isFired: 1;
    uint32_t mask: 2;
    
    uint32_t status: 2;
    uint32_t checkReadTimeout: 1;
    uint32_t checkWriteTimout: 1;
    uint32_t isReadTimeout: 1;
    uint32_t isWriteTimeout: 1;

    uint32_t hashOfReadTimer: 13;
    uint32_t hashOfWriteTimer: 13;

    CEFileLink_s readTimer;
    CEFileLink_s writeTimer;
} CEFile_s;

typedef struct {
    CEFileId id;
    uint32_t handler;
} CEFileFiredInfo;

typedef void (*CEPollTimeoutCallback_f)(void * _Nullable context, CEFileId * _Nonnull ids, uint32_t count);
typedef void (*CEPollFileEventCallback_f)(void * _Nullable context, CEFileId * _Nonnull ids, uint32_t count);

typedef struct {
    CEPollTimeoutCallback_f handleTimeout;
    CEPollFileEventCallback_f handleFileEvent;
    CCPtr _Nullable context;
} CEFileEventHandler_s;

//typedef struct {
//    CCRange32 range;
//    CEFileEventHandler_s handler;
//} CEFileHandler_s;

typedef struct {
    uint32_t handlerId;
    uint32_t count;
    CEFileId items[1023];
} CEFileEventBuffer_s;

typedef struct {
    uint16_t table[CEPollFileTimerTableSize];
} CEFileTimerMap_s;


typedef struct {
    pthread_t _Nullable thread;
    void * _Nullable api; /* This is used for polling API specific data */

#if __APPLE__
    os_unfair_lock blockQueueLock;
#else
    pthread_spinlock_t blockQueueLock;
#endif
    uint64_t fdSequence;

    uint32_t blockEvent;
    uint32_t timerTableOffset;//source timer 的 游标
    uint32_t tmpBufferSize;
    CEBlockQueue_s blockQueue;
    CCPtr _Nonnull tmpBuffer;
    CETimeEventManager_s timeEventManager;
    
    CCMicrosecondTime currentTime;//单位 微秒
    uint64_t fileTimerSeconds8;//单位为(1/8)秒
    
    uint32_t fileTableSize;
    uint32_t fileEventHandlerCount;

    CEFile_s * _Nonnull fileTable;

    
//    uint32_t observerBufferSize;
//    uint32_t observerBufferCount;
//
//    CERunLoopObserver_s * _Nullable * _Nonnull observers;
    
    CEFileEventHandler_s fileEventHandlers[CEPollFileHandlerTableSize];
    CCRange16 firedFileCounts[CEPollFileHandlerTableSize];

    CEFileTimerMap_s readTimerMap;
    CEFileTimerMap_s writeTimerMap;
} CEPoll_s;
#pragma pack(pop)

static inline CEFile_s * _Nonnull CEPollGetFile(CEPoll_s * p, int index) {
    assert(index >= 0);
    assert(index < 0xffff);
    CEFile_s * file = p->fileTable;
    file += index;
    return file;
}

static inline CEFileId CEFileIdMake(uint16_t fd, uint16_t seq) {
    CEFileId f = {
        .fd = fd,
        .sequence = seq,
    };
    return f;
}

typedef struct {
    CCByte16 deviceToken;
    CCByte16 userToken;
} Connect_s;

typedef struct {
    uint16_t domain;
    uint16_t service;
    uint16_t time;
} PackageHeader_s;

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


CEResult_t CEPollAddFile(CEPollPtr _Nonnull poll, uint32_t handlerId, int fd, CEFileId * _Nonnull fdPtr) {
    assert(poll);
    CEPoll_s * p = poll;
    if (fd < 0 || fd >= p->fileTableSize) {
        CELogWarning("CEPollAddFile error, unsupport with fd: %d\n", fd);
        return CEResultErrorParams;
    };
    
    if (handlerId == 0 || handlerId >= p->fileEventHandlerCount) {
        CELogWarning("CEPollAddFile error, unsupport with handleId: %d\n", handlerId);
        return CEResultErrorParams;
    };
    if (NULL == fdPtr) {
        CELogWarning("CEPollAddFile error, unsupport with fdPtr: null\n");
        return CEResultErrorParams;
    };
    assert(poll);
    uint16_t index = (uint16_t)fd;
    uint16_t sequence = (uint16_t)p->fdSequence;
    p->fdSequence += 1;

    fdPtr->fd = index;
    fdPtr->sequence = sequence;

    CEFile_s * file = CEPollGetFile(p, index);
    file->sequence = sequence;
    file->handler = handlerId;
    file->isValid = 1;
    file->isFired = 0;
    file->status = 0;
    file->mask = 0;

    file->checkReadTimeout = 0;
    file->checkWriteTimout = 0;
    file->isReadTimeout = 0;
    file->isWriteTimeout = 0;

    return CEResultSuccess;
}
CEResult_t CEPollRemoveFile(CEPollPtr _Nonnull poll, CEFileId fd) {
    assert(poll);
    CEPoll_s * p = poll;

    if (fd.fd < 0 || fd.fd >= p->fileTableSize) {
        CELogWarning("CEPollRemoveFile error 1, fd error: (%d, %d)\n", fd.fd, fd.sequence);
        return CEResultErrorFileDescription;
    };
    
    CEFile_s * file = CEPollGetFile(p, fd.fd);
    
    if (0 == file->isValid) {
        CELogWarning("CEPollRemoveFile error 2, fd error: (%d, %d)\n", fd.fd, fd.sequence);
        return CEResultErrorFileDescription;
    };
    
    if (file->sequence == fd.sequence) {
        if (file->checkReadTimeout) {
            CEPollCancelFileEventReadTimeout(p, fd);
        }
        if (file->checkWriteTimout) {
            CEPollCancelFileEventWriteTimeout(p, fd);
        }
        file->isValid = 0;
        return CEResultSuccess;
    } else {
        CELogWarning("CEPollRemoveFile error, fd error: (%d, %d)\n", fd.fd, fd.sequence);
        return CEResultErrorParams;
    }
}


CEResult_t CEPollAddFileEventMask(CEPollPtr _Nonnull poll, CEFileId fd, CEFileEventMask_es mask) {
    assert(poll);
    CEPoll_s * p = poll;
    if (fd.fd < 0 || fd.fd >= p->fileTableSize) {
        CELogWarning("CEPollRemoveFile error 1, fd error: (%d, %d)\n", fd.fd, fd.sequence);
        return CEResultErrorFileDescription;
    };
    
    CEFile_s * file = CEPollGetFile(p, fd.fd);
    
    if (CEApiAddEvent(p->api, fd.fd, file->mask, mask) == -1) {
        return CEResultErrorSystemCall;
    }
    file->mask |= mask;
    return CEResultSuccess;
}

CEResult_t CEPollRemoveFileEventMask(CEPollPtr _Nonnull poll, CEFileId fd, CEFileEventMask_es mask) {
    assert(poll);
    CEPoll_s * p = poll;
    if (fd.fd < 0 || fd.fd >= p->fileTableSize) {
        CELogWarning("CEPollRemoveFile error 1, fd error: (%d, %d)\n", fd.fd, fd.sequence);
        return CEResultErrorFileDescription;
    };
    CEFile_s * file = CEPollGetFile(p, fd.fd);
    if (file->mask == CEFileEventMaskNone) {
        return CEResultSuccess;
    };
    file->mask = file->mask & (~mask);
    CEApiRemoveEvent(p->api, fd.fd, mask);
    return CEResultSuccess;
}

//timeout  单位为毫妙 数值必须大于0 且 小于等于CEFileEventTimeoutMillisecondMax


CEResult_t CEPollSetFileEventReadTimeout(CEPollPtr _Nonnull poll, CEFileId fd, CEMicrosecondTime timeout) {
    //todo

    return CEResultSuccess;
}
CEResult_t CEPollSetFileEventWriteTimeout(CEPollPtr _Nonnull poll, CEFileId fd, CEMicrosecondTime timeout) {
    //todo

    return CEResultSuccess;
}

CEResult_t CEPollCancelFileEventReadTimeout(CEPollPtr _Nonnull poll, CEFileId fd) {
    //todo

    return CEResultSuccess;

}
CEResult_t CEPollCancelFileEventWriteTimeout(CEPollPtr _Nonnull poll, CEFileId fd) {
    //todo

    return CEResultSuccess;

}



#pragma mark - run loop async api


void CEPollWakeUp(CEPoll_s * _Nonnull p) {
    assert(p);
    CEApiWakeUp(p->api);
}
static inline void CEPollBlockQueueLock(CEPoll_s * _Nonnull poll) {
    assert(poll);
#if __APPLE__
    os_unfair_lock_lock(&(poll->blockQueueLock));
#else
    pthread_spin_lock(&(poll->blockQueueLock));
#endif
}

static inline void CEPollBlockQueueUnlock(CEPoll_s * _Nonnull poll) {
    assert(poll);
#if __APPLE__
    os_unfair_lock_unlock(&(poll->blockQueueLock));
#else
    pthread_spin_unlock(&(poll->blockQueueLock));
#endif
}

CEBlockQueuePage_s * _Nullable CEBlockQueueRemoveAll(CEBlockQueue_s * _Nonnull queue) {
    CEBlockQueuePage_s * page = queue->begin;
    queue->begin = NULL;
    queue->end = NULL;
    return page;
}

void CEBlockQueueAppend(CEBlockQueue_s * _Nonnull queue, CCClosureRef _Nonnull closure) {
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
}


void CEPollAsync(CEPollPtr _Nonnull poll, CCClosureRef _Nonnull closure) {
    assert(poll);
    assert(closure);
    CEPoll_s * p = poll;

    CCClosureRetain(closure);
    CEPollBlockQueueLock(p);
    CEBlockQueueAppend(&(p->blockQueue), closure);
    CEPollBlockQueueUnlock(p);
    CEPollWakeUp(p);
}


#pragma mark - api callback


void CEApiFileEventMapperCallback(void * _Nullable context, void * _Nonnull api, CCPtr _Nonnull buffer, int fd, CEFileEventMask_es mask) {
    assert(context);
    if (mask == CEFileEventMaskNone) {
        return;
    }
    CEPoll_s * p = (CEPoll_s *)context;

    CEFile_s * file = CEPollGetFile(p, fd);

    assert(file->isValid);
    assert(file->handler > 0);
    
    if (CEFileEventMaskReadWritable & mask) {
        file->status = file->status | mask;
        
        if (0 == file->isFired) {
            CEFileFiredInfo info = {
                .id = CEFileIdMake(fd, file->sequence),
                .handler = file->handler,
            };
            CEFileFiredInfo * item = buffer;
            *item = info;
            assert(file->handler > 0);
            assert(file->handler < p->fileEventHandlerCount);

            file->isFired = 1;
        }
    }
}

void CEApiPipeEventCallback(void * _Nullable context, void * _Nonnull api) {
    assert(context);
    CERunLoop_s * eventLoop = (CERunLoop_s *)context;
    eventLoop->blockEvent = 1;
    CELogVerbose2(128, "CEApiPipeEventCallback");
}


#pragma mark - runloop



int64_t _CEPollDoBlocks(CEPoll_s * _Nonnull p) {
    CEBlockQueuePage_s * page = NULL;
    CEPollBlockQueueLock(p);
    page = CEBlockQueueRemoveAll(&(p->blockQueue));
    CEPollBlockQueueUnlock(p);
    
    int64_t count = 0;
    while (NULL != page) {
        CEBlockQueuePage_s * nextPage = page->next;
        for (size_t i=0; i<page->used; i++) {
            count += 1;
            
            CCClosureRef closure = page->buffer[i];
            CCClosurePerform(closure, NULL, NULL);
            CCClosureRelease(closure);
        }
        CEDeallocate(page);
        page = nextPage;
    }
    return count;
}

int64_t _CEPollDoTimers(CEPoll_s * _Nonnull p) {
    int64_t processed = 0;
    
    CETimeEventManager_s * tem = &(p->timeEventManager);
    CETimeEventQueue_s * teq = &(p->timeEventManager.timerQueue);
    
    CETimeEvent_s * te = CETimeEventQueueGetFirst(teq);
    CEMicrosecondTime now = p->currentTime;
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


static const struct timeval CEApiPollDefaultTime = {0, 124000};
int __CEPollDoCheckSource(CEPoll_s * _Nonnull p) {
    static const CEApiPollCallback_s callback = {
        .mapper = CEApiFileEventMapperCallback,
        .pipeCallback = CEApiPipeEventCallback,
    };
    
    CEMicrosecondTime now = CEGetCurrentTime();
    if (now >= p->currentTime + CEFrameIntervalDefault) {
        return CEApiPoll(p->api, NULL, p->tmpBuffer, p->tmpBufferSize, sizeof(CEFileFiredInfo), p, &callback);
    } else {
        struct timeval tv = {
            .tv_sec = 0,
            .tv_usec = CEFrameIntervalDefault - p->fileTableSize / 32 * CEFrameIntervalPer32,
        };
        struct timeval * tvp = &tv;
        CETimeEvent_s * te = CETimeEventQueueGetFirst(&(p->timeEventManager.timerQueue));
        if (NULL != te) {
            if (te->when > now) {
                CEMicrosecondTime tmp = te->when - now;
                if (tmp < tv.tv_usec) {
                    tv.tv_sec = 0;
                    tv.tv_usec = (int)tmp;
                }
            }
        }
        return CEApiPoll(p->api, tvp, p->tmpBuffer, p->tmpBufferSize, sizeof(CEFileFiredInfo), p, &callback);
    }
}

int64_t _CEPollDoSource(CEPoll_s * _Nonnull p) {
    assert(p);
    int processed = __CEPollDoCheckSource(p);
    for (int j = 0; j < processed; j++) {
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

void __CEPollDoCheckSourceTimeout(CEPoll_s * _Nonnull p) {
    CERunLoopTimeSlip(eventLoop, 1);
}

int64_t _CEPollDoSourceTimeout(CEPoll_s * _Nonnull p) {
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


static const char * const _Nullable CEPollProgressName[4] = {
    "CEPollProgress.doSource",
    "CEPollProgress.doSourceTimeout",
    "CEPollProgress.doBlock",
    "CEPollProgress.doTimer",
};
static inline void _CEPollProcessBeginNew(CEPoll_s * _Nonnull p, CEPollProgress_t new) {
//    p->progress = new;
    CELogVerbose2(0, "%s begin", CEPollProgressName[new]);
}



int64_t CEPollProcessEvents(CEPoll_s * _Nonnull p) {
    int64_t processed = 0;
    
    _CEPollProcessBeginNew(p, CEPollProgressDoSource);
    processed += _CEPollDoSource(p);
    _CEPollProcessBeginNew(p, CEPollProgressDoSourceTimeout);
    processed += _CEPollDoSourceTimeout(p);

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
        CEPollProcessEvents(p);
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
    
    result = pthread_create(&tid, &attr, CEPollRunloopRun, p);
    if (0 != result) {
        CELogError("pthread_create %s\n", strerror(result));
        return 0;
    }
    return tid;
}


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
    poll->fileTable = CEAllocateClear(sizeof(CEFile_s) * setsize);
    
    
    
    uint32_t pageCount = setsize >> 16;

//    for (uint32_t index=0; index< pageCount; index++) {
//        poll->fileEventsPages[index] = (CEFileEvent_s *)CEAllocateClear(sizeof(CEFileEvent_s) * 0x10000);
//        poll->firedPages[index] = (CEFiredEvent_s *)CEAllocate(sizeof(CEFiredEvent_s) * 0x10000);
//    }
//    for (int index=0; index< 0x4000; index++) {
//        poll->readTimerPages[index] = CEFileDescriptionInvalid;
//        poll->writeTimerPages[index] = CEFileDescriptionInvalid;
//    }
//    
    poll->fileTableSize = setsize;
    poll->currentTime = CEGetCurrentTime();

    
#if __APPLE__
    poll->blockQueueLock = OS_UNFAIR_LOCK_INIT;
#else
    pthread_spin_init(&(poll->blockQueueLock), PTHREAD_PROCESS_PRIVATE);
#endif
    
    poll->api = api;

    CETimeEventQueueInit(&(poll->timeEventManager.timerQueue));
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



