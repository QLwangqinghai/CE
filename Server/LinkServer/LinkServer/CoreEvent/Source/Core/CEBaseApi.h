//
//  CEBaseApi.h
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEBaseApi_h
#define CEBaseApi_h

#include "CEBaseType.h"

typedef void (*CEApiPoolCallback_f)(void * _Nullable context, void * _Nonnull api);
typedef void (*CEApiPoolFileEventCallback_f)(void * _Nullable context, void * _Nonnull api, int fd, CEFileEventMask_es mask);

typedef struct _CEApiPoolCallback {
    CEApiPoolFileEventCallback_f _Nonnull fileEventCallback;
    CEApiPoolCallback_f _Nonnull pipeCallback;
} CEApiPoolCallback_s;


typedef int CEApiResult_t;

static const CEApiResult_t CEApiResultSuccess = 0;
static const CEApiResult_t CEApiResultErrorSystemCall = 2;


size_t CEApiCreateGetEventSize(void);

CCPtr _Nonnull CEApiCreate(void);

void CEApiFree(void * _Nonnull api);

CEApiResult_t CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es oldMask, CEFileEventMask_es mask);

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask);

typedef struct __CEApiUpdateEventsItem {
    
} CEApiUpdateEventsItem_t;

void CEApiUpdateEvents(void * _Nonnull api, int * _Nonnull fdPtrs, uint32_t swangqiangize, CEFileEventMask_es oldMask, CEFileEventMask_es mask);



int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, CCPtr _Nonnull buffer, uint32_t bufferSize, void * _Nullable context, const CEApiPoolCallback_s * _Nonnull callback);

void CEApiWakeUp(void * _Nonnull api);

CEFileEventMask_es CEApiWait(int fd, CEFileEventMask_es mask, int milliseconds);

char * _Nonnull CEApiName(void);




#endif /* CEBaseApi_h */
