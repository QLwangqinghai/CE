//
//  CEPoll.h
//  CoreEvent
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#ifndef CEPoll_h
#define CEPoll_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "CEBase.h"
#include <CCFoundation/CCClosure.h>

#pragma mark - share
CEPollPtr _Nonnull CEPollShared(void);

uint32_t CEPollRegisterHandler(CEPollPtr _Nonnull poll, CCPtr _Nullable context, CEPollFileEventCallback_f _Nonnull handleFileEvent);

#pragma mark - time event api


CCBool CEPollTimerQueueIsFull(CEPollPtr _Nonnull poll);

/**
 * mode CETimeEventModeDelay、CETimeEventModeRepeatFixedRate、CETimeEventModeRepeatFixedDelay
 * deadline 第一次触发时间
 * repeating 间隔时间 (mode 为  CETimeEventModeRepeatFixedRate、CETimeEventModeRepeatFixedDelay 时有效)
 * closure timer 会作为input传入
 * timerQueue 满了 时会崩溃
 * 返回一个retain的对象
 */
CETimeEventId _Nonnull CEPollAddTimeEvent(CEPollPtr _Nonnull poll,
                                           uint32_t mode,
                                           CEMicrosecondTime deadline,
                                           CEMicrosecondTime repeating,
                                           CCClosureRef _Nonnull closure);

void CEPollCancelTimeEvent(CEPollPtr _Nonnull poll, CETimeEventId _Nonnull ref);
CCBool CEPollIsValidTimeEvent(CEPollPtr _Nonnull poll, CETimeEventId _Nonnull ref);


#pragma mark - file event api

CEResult_t CEPollAddFile(CEPollPtr _Nonnull poll, uint32_t handleId, int fd, uint32_t context);
CEResult_t CEPollRemoveFile(CEPollPtr _Nonnull poll, int fd);

CEResult_t CEPollAddFileEventMask(CEPollPtr _Nonnull poll, int fd, CEFileEventMask_es mask);
CEResult_t CEPollRemoveFileEventMask(CEPollPtr _Nonnull poll, int fd, CEFileEventMask_es mask);


#pragma mark - run loop async api

void CEPollAsync(CEPollPtr _Nonnull poll, CCClosureRef _Nonnull closure);

#pragma mark - run loop api

char * _Nonnull CEPollGetApiName(void);

uint32_t CEPollGetSetSize(CEPollPtr _Nonnull poll);

//只有在 CEPollShared 调用之前设置有效
void CEPollUpdateSetSize(uint32_t setsize);


#if defined(__cplusplus)
}
#endif

#endif /* CEPoll_h */
