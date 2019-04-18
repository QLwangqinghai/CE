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




void * _Nullable CEApiCreate(int setsize, void * _Nonnull context, CEApiPoolGetEventMask_f _Nonnull func);



int CEApiResize(void * _Nonnull api, int setsize);

void CEApiFree(void * _Nonnull api);

int CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask);

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask);



int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, int maxfd, int setsize, void * _Nullable context, CEApiPoolCallback_f _Nonnull callback);

char * _Nonnull CEApiName(void);




#endif /* CEBaseApi_h */
