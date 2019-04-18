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

typedef int CEApiResult_t;

static const CEApiResult_t CEApiResultSuccess = 0;
static const CEApiResult_t CEApiResultErrorSystemCall = 2;

void * _Nullable CEApiCreate(uint32_t setsize);


void CEApiResize(void * _Nonnull api, uint32_t setsize);

void CEApiFree(void * _Nonnull api);

CEApiResult_t CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es oldMask, CEFileEventMask_es mask);

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask);



int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, int setsize, void * _Nullable context, const CEApiPoolCallback_s * _Nonnull callback);


CEApiResult_t CEApiUpdateTimer(void * _Nonnull api, uint64_t when);

CEApiResult_t CEApiRemoveTimer(void * _Nonnull api);


void CEApiWakeUp(void * _Nonnull api);

CEFileEventMask_es CEApiWait(int fd, CEFileEventMask_es mask, int milliseconds);

char * _Nonnull CEApiName(void);




#endif /* CEBaseApi_h */
