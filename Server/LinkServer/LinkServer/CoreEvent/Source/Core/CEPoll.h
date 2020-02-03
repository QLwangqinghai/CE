//
//  CEPoll.h
//  CoreEvent
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#ifndef CEPoll_h
#define CEPoll_h

#include "CEBase.h"
#include <CCFoundation/CCClosure.h>


typedef void (*CEPollTimeoutCallback_f)(void * _Nullable context, CEFileId * _Nonnull ids, uint32_t count);
typedef void (*CEPollFileEventCallback_f)(void * _Nullable context, CEFileId * _Nonnull ids, uint32_t count);

typedef struct {
    CEPollTimeoutCallback_f _Nonnull handleTimeout;
    CEPollFileEventCallback_f _Nonnull handleFileEvent;
    CCPtr _Nullable context;
} CEFileEventHandler_s;


#pragma mark - share
CEPollPtr _Nonnull CEPollShared(void);

uint32_t CEPollRegisterHandler(CEPollPtr _Nonnull poll, CCPtr _Nullable context, CEPollFileEventCallback_f _Nonnull handleFileEvent, CEPollTimeoutCallback_f _Nonnull handleTimeout);

CEFileEventMask_es CEPollGetFileStatus(CEPollPtr _Nonnull poll, CEFileId id);


#pragma mark - time event api


CCBool CEPollTimerQueueIfFull(CEPollPtr _Nonnull poll);

/**
 * mode CETimeEventModeDelay、CETimeEventModeRepeatFixedRate、CETimeEventModeRepeatFixedDelay
 * deadline 第一次触发时间
 * repeating 间隔时间 (mode 为  CETimeEventModeRepeatFixedRate、CETimeEventModeRepeatFixedDelay 时有效)
 * closure timer 会作为input传入
 * timerQueue 满了 时会崩溃
 * 返回一个retain的对象
 */
CETimeEventRef _Nonnull CEPollAddTimeEvent(CEPollPtr _Nonnull poll,
                                           uint32_t mode,
                                           CEMicrosecondTime deadline,
                                           CEMicrosecondTime repeating,
                                           CCClosureRef _Nonnull closure);

void CEPollCancelTimeEvent(CEPollPtr _Nonnull poll, CETimeEventRef _Nonnull ref);
CCBool CEPollIsValidTimeEvent(CEPollPtr _Nonnull poll, CETimeEventRef _Nonnull ref);


#pragma mark - file event api

CEResult_t CEPollAddFile(CEPollPtr _Nonnull poll, uint32_t handleId, int fd, CEFileId * _Nonnull fdPtr);
CEResult_t CEPollRemoveFile(CEPollPtr _Nonnull poll, CEFileId fd);

CEResult_t CEPollAddFileEventMask(CEPollPtr _Nonnull poll, CEFileId fd, CEFileEventMask_es mask);

CEResult_t CEPollRemoveFileEventMask(CEPollPtr _Nonnull poll, CEFileId fd, CEFileEventMask_es mask);

//timeout  单位为微妙 数值必须大于0 且 小于等于CEFileEventTimeoutMax
CEResult_t CEPollSetFileEventReadTimeout(CEPollPtr _Nonnull poll, CEFileId fd, CEMicrosecondTime timeout);
CEResult_t CEPollSetFileEventWriteTimeout(CEPollPtr _Nonnull poll, CEFileId fd, CEMicrosecondTime timeout);

CEResult_t CEPollCancelFileEventReadTimeout(CEPollPtr _Nonnull poll, CEFileId fd);
CEResult_t CEPollCancelFileEventWriteTimeout(CEPollPtr _Nonnull poll, CEFileId fd);



#pragma mark - run loop async api

void CEPollAsync(CEPollPtr _Nonnull poll, CCClosureRef _Nonnull closure);

#pragma mark - run loop api

char * _Nonnull CEPollGetApiName(void);

uint32_t CEPollGetSetSize(CEPollPtr _Nonnull poll);

//只有在 CEPollShared 调用之前设置有效
void CEPollUpdateSetSize(uint32_t setsize);




#endif /* CEPoll_h */
