//
//  LSEventLoop.c
//  LinkServer
//
//  Created by vector on 2020/2/12.
//  Copyright © 2020 vector. All rights reserved.
//

#include "LSEventLoop.h"
#include "LSConnnectionGroup.h"

#include <pthread.h>
#include <semaphore.h>
#include "LSManager.h"


#pragma mark - api callback

void _LSEventLoopApiPollCallback(void * _Nonnull api, void * _Nullable context, CEEvent_s event);


void _LSEventLoopApiPollCallback(void * _Nonnull api, void * _Nullable context, CEEvent_s event) {
    LSManager_s * manager = LSManagerShared();
    LSEventLoop_s * loop = context;
    LSEvent_s item = {};
    item.error = event.error;
    item.status = event.status;
    item.fd = event.fd;
    item.loopId = loop->id;
    LSFile_s * file = LSManagerGetFile(manager, event.fd);
    assert(file->isValid);
    assert(file->loopId = loop->id);
    uint32_t index = file->index;
    item.groupId = index >> 13;
    item.indexInGroup = index & 0x1fff;
    LSConnectionGroup_s * group = LSEventLoopGetGroup(loop, item.groupId);
    _LSConnnectionGroupSaveEvent(group, item);
}

#pragma mark - runloop

//int64_t _CEPollDoBlocks(CEPoll_s * _Nonnull p) {
//    CEBlockQueuePage_s * page = NULL;
//    page = CEBlockQueueRemoveAll(&(p->blockQueue));
//
//    int64_t count = 0;
//    while (NULL != page) {
//        CEBlockQueuePage_s * nextPage = page->next;
//        for (size_t i=0; i<page->used; i++) {
//            count += 1;
//
//            CCClosureRef closure = page->buffer[i];
//            CCClosurePerform(closure, NULL, NULL);
//            CCRefRelease(closure);
//        }
//        CEDeallocate(page);
//        page = nextPage;
//    }
//    return count;
//}

//void _LSEventLoopDoTimers(LSEventLoop_s * _Nonnull loop) {
//    int64_t processed = 0;
//
//    CETimerQueue_s * queue = p->timerQueue;
//    CETimeEventRef * ref = _CETimerQueueGetFirst(queue);
//    CEMicrosecondTime now = p->currentTime;
//    int64_t fired = 0;
//    while (NULL != ref) {
//        CETimeEvent_s * event = (CETimeEvent_s *)CCRefGetContentPtr(ref);
//        if (event->when <= now) {
//            if (event->isCanceled) {
//                if (_CETimerQueueRemove(queue, ref)) {
//                    CCRefRelease(ref);
//                }
//            } else {
//                assert(0 == event->isFinished);
//                _CETimeEventPerform(ref);
//                if (0 == event->isCanceled) {
//                    if (CETimeEventModeRepeatFixedRate == event->mode) {
//                        _CETimerQueueUpdate(queue, ref, event->when + event->duration);
//                    } else if (CETimeEventModeRepeatFixedDelay == event->mode) {
//                        _CETimerQueueUpdate(queue, ref, CEGetCurrentTime() + event->duration);
//                    } else if (CETimeEventModeDelay == event->mode) {
//                        event->isFinished = true;
//                    } else {
//                        CELogError("CETimeEventPerform error");
//                        abort();
//                    }
//                }
//                if (0 != event->isCanceled || 0 != event->isFinished) {
//                    if (_CETimerQueueRemove(queue, ref)) {
//                        CCRefRelease(ref);
//                    }
//                }
//                ref = _CETimerQueueGetFirst(queue);
//            }
//            fired += 1;
//        } else {
//            break;
//        }
//    }
//    return processed;
//}


//bufferSize 缓冲区长度， 不是个数
void _LSEventLoopDoCheckSource(LSEventLoop_s * _Nonnull loop) {

    CCBootMicrosecondTime timeout = 1000000;
    CCBootMicrosecondTime now = CCBootInterval();
    
//    if (now < p->currentTime + CEFrameIntervalDefault) {
//        CETimeEvent_s * ref = _CETimerQueueGetFirst(p->timerQueue);
//        if (NULL != ref) {
//            CETimeEvent_s * event = (CETimeEvent_s *)CCRefGetContentPtr(ref);
//            if (event->when > now) {
//                timeout = 1;
//            } else {
//                timeout = event->when - now;
//            }
//        } else {
//            timeout = CEFrameIntervalDefault - p->fileTableSize / 32 * CEFrameIntervalPer32;
//        }
//    }

    uint32_t count = loop->groupCount;
    for (uint32_t index=0; index<count; index++) {
        LSConnectionGroup_s * group = LSEventLoopGetGroup(loop, index);
        _LSConnnectionGroupClearEvents(group);
    }
    CEApiPoll(loop->api, timeout, loop, _LSEventLoopApiPollCallback);
}


void _LSEventLoopDoSource(LSEventLoop_s * _Nonnull loop) {
    CCBootMicrosecondTime now = CCBootInterval();
    uint32_t count = loop->groupCount;
    for (uint32_t index=0; index<count; index++) {
        LSConnectionGroup_s * group = LSEventLoopGetGroup(loop, index);
        _LSConnnectionGroupPerformEvents(group, now);
    }
    
    for (uint32_t index=0; index<count; index++) {
        LSConnectionGroup_s * group = LSEventLoopGetGroup(loop, index);
        _LSConnnectionGroupCheckTimeout(group, now);
    }
}

//static const char * const _Nullable CEPollProgressName[4] = {
//    "CEPollProgress.doSource",
//    "CEPollProgress.doBlock",
//    "CEPollProgress.doTimer",
//};
//static inline void _CEPollProcessBeginNew(CEPoll_s * _Nonnull p, CEPollProgress_t new) {
//    p->progress = new;
//    CELogVerbose2(0, "%s begin", CEPollProgressName[new]);
//}

void LSEventLoopProcessEvents(LSEventLoop_s * _Nonnull loop) {
    _LSEventLoopDoCheckSource(loop);
    _LSEventLoopDoSource(loop);

//    _CEPollProcessBeginNew(p, CEPollProgressDoBlock);
//    processed += _CEPollDoBlocks(loop);
//
//    _CEPollProcessBeginNew(p, CEPollProgressDoTimer);
//    processed += _CEPollDoTimers(loop);

}


void * LSEventLoopRun(void * args) {
    LSEventLoop_s * _Nonnull p = (LSEventLoop_s *)args;
    while (1) {
        LSEventLoopProcessEvents(p);
    }
    return NULL;
}

pthread_t LSEventLoopCreateThread(LSEventLoop_s * _Nonnull loop) {
    pthread_t tid;
    pthread_attr_t attr;
    //初始化线程属性
    pthread_attr_init(&attr);

    int result = 0;
    
//    result = pthread_getschedparam(pthread_self(), &policy, &param);
//    if (0 != result) {
//        CELogError("pthread_getschedparam %s\n", strerror(result));
//        return 0;
//    }
    
    /*
     某个线程固定的"绑"在一个轻进程之上。被绑定的线程具有较高的响应速度，这是因为CPU时间片的调度是面向轻进程的，绑定的线程可以保证在需要的时候它总有一个轻进程可用。通过设置被绑定的轻进程的优先级和调度级可以使得绑定的线程满足诸如实时反应之类的要求。
     设置线程绑定状态的函数为pthread_attr_setscope，它有两个参数，第一个是指向属性结构的指针，第二个是绑定类型，它有两个取值：PTHREAD_SCOPE_SYSTEM（绑定的）和PTHREAD_SCOPE_PROCESS（非绑定的）
     */
    
    result = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    if (0 != result) {
        CCLogError("pthread_attr_setscope PTHREAD_SCOPE_SYSTEM %s\n", strerror(result));
        return 0;
    }
    
    //线程结束自动释放资源
    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (0 != result) {
        CCLogError("pthread_attr_setdetachstate PTHREAD_CREATE_DETACHED %s\n", strerror(result));
        return 0;
    }
    
    size_t stacksize = 0;
    result = pthread_attr_getstacksize(&attr, &stacksize);
    
    if (0 != result) {
        CCLogError("pthread_attr_getstacksize error %s\n", strerror(result));
        return 0;
    }
//PTHREAD_STACK_MIN;
    size_t rStacksize = 1024 * 1024 * 4;
    if (stacksize < rStacksize) {
        result = pthread_attr_setstacksize(&attr, rStacksize);//设置线程堆栈大小
        if (0 != result) {
            CCLogError("pthread_attr_setstacksize error %s\n", strerror(result));
            return 0;
        }
    }
    
    
    //优先级调整
//    struct sched_param param;
//    int policy = get_thread_policy(&attr);
//    int priority = sched_get_priority_max(policy);
//    if (-1 == priority) {
//        CELogError("sched_get_priority_max error %s\n", strerror(result));
//        return 0;
//    }
//    pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED); //有这行，设置优先级才会生效
//    pthread_attr_setschedpolicy(&thread_attr,SCHED_RR);
//    pthread_attr_setschedparam(&thread_attr, &schedule_param);
//    param.sched_priority = priority;
//
//    result = pthread_attr_setschedparam(&attr, &param);
//    if (0 != result) {
//        CELogError("pthread_attr_setschedparam %s\n", strerror(result));
//        return 0;
//    }
    
    result = pthread_create(&tid, &attr, LSEventLoopRun, loop);
    if (0 != result) {
        CCLogError("pthread_create %s\n", strerror(result));
        return 0;
    }
    return tid;
}

LSEventLoop_s * _Nonnull LSEventLoopCreate(uint32_t id, uint32_t groupCount) {
    assert(groupCount > 0);
    uint32_t connectionCapacity = groupCount * 4096;
    LSEventLoop_s * loop = CCAllocate(sizeof(LSEventLoop_s) + sizeof(LSConnectionGroup_s) * groupCount);
    loop->connectionCapacity = connectionCapacity;
    loop->groupCount = groupCount;
    loop->id = id;
    for (uint32_t i=0; i<groupCount; i++) {
        LSConnnectionGroupInit(&(loop->groups[i]), id);
    }
    loop->thread = LSEventLoopCreateThread(loop);
    return loop;
}


