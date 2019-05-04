//
//  CEPoolBaseApi.h
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEPoolBaseApi_h
#define CEPoolBaseApi_h


#include "CEBase.h"



typedef CEFileEventMask_es (*CEApiPoolGetEventMask_f)(void * _Nullable context, void * _Nonnull api, int fd);

typedef void (*CEPoolApiCallback_f)(void * _Nullable context, void * _Nonnull api);
typedef void (*CEPoolApiFileEventCallback_f)(void * _Nullable context, void * _Nonnull api, int fd, CEFileEventMask_es mask);

typedef struct _CEPoolApiCallback {
    CEPoolApiFileEventCallback_f _Nonnull fileEventCallback;
    CEPoolApiCallback_f _Nonnull pipeCallback;
} CEPoolApiCallback_s;


typedef int CEPoolApiResult_t;

static const CEPoolApiResult_t CEPoolResultSuccess = 0;
static const CEPoolApiResult_t CEPoolApiResultErrorSystemCall = 2;

void * _Nullable CEApiCreate(unsigned int setsize);


void CEApiResize(void * _Nonnull api, uint32_t setsize);

void CEApiFree(void * _Nonnull api);

CEPoolApiResult_t CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es oldMask, CEFileEventMask_es mask);

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask);

typedef struct __CEApiUpdateEventsItem {
    
} CEApiUpdateEventsItem_t;

void CEApiUpdateEvents(void * _Nonnull api, int * _Nonnull fdPtrs, uint32_t size, CEFileEventMask_es oldMask, CEFileEventMask_es mask);



int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, int setsize, void * _Nullable context, const CEPoolApiCallback_s * _Nonnull callback);

void CEApiWakeUp(void * _Nonnull api);

CEFileEventMask_es CEApiWait(int fd, CEFileEventMask_es mask, int milliseconds);

char * _Nonnull CEApiName(void);




#endif /* CEBaseApi_h */
