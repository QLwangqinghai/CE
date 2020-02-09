//
//  CEBaseApi.h
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEBaseApi_h
#define CEBaseApi_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "CEBaseType.h"

typedef void (*CEApiPollCallback_f)(void * _Nullable context, void * _Nonnull api);
typedef _Bool (*CEApiPollFileEventFilterMapper_f)(void * _Nullable context, void * _Nonnull api, void * _Nonnull buffer, int fd, CEFileEventMask_es mask);
typedef struct _CEApiPoolCallback {
    CEApiPollFileEventFilterMapper_f _Nonnull filterMapper;
    CEApiPollCallback_f _Nonnull pipeCallback;
} CEApiPollCallback_s;


typedef int CEApiResult_t;

static const CEApiResult_t CEApiResultSuccess = 0;
static const CEApiResult_t CEApiResultErrorSystemCall = 2;


size_t CEApiGetEventItemSize(void);

CCPtr _Nonnull CEApiCreate(uint32_t setSize);

void CEApiFree(void * _Nonnull api);

void CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es oldMask, CEFileEventMask_es mask);

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask);

typedef struct __CEApiUpdateEventsItem {
    
} CEApiUpdateEventsItem_t;

void CEApiUpdateEvents(void * _Nonnull api, int * _Nonnull fdPtrs, uint32_t size, CEFileEventMask_es oldMask, CEFileEventMask_es mask);


int CEApiPoll(void * _Nonnull api, CEMicrosecondTime timeout, CCPtr _Nonnull buffer, size_t bufferSize, uint32_t itemSize, void * _Nullable context, const CEApiPollCallback_s * _Nonnull callback);

void CEApiWakeUp(void * _Nonnull api);

CEFileEventMask_es CEApiWait(int fd, CEFileEventMask_es mask, int milliseconds);

char * _Nonnull CEApiName(void);


#if defined(__cplusplus)
}
#endif

#endif /* CEBaseApi_h */
