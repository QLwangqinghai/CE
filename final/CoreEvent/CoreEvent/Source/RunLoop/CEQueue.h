//
//  CEQueue.h
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEQueue_h
#define CEQueue_h

#include <CoreEvent/CEThreadBase.h>


extern CETypeRef _Nonnull CETypeQueue;;

void CEQueueSync(CEQueueRef _Nonnull queuePtr,
                 CEPtr _Nonnull object,
                 CETaskFinish_f _Nullable finish,
                 CEFunction_f _Nonnull execute,
                 CETaskParamRef _Nullable param,
                 CETaskParamRef _Nullable result);

void CEQueueAsync(CEQueueRef _Nonnull queuePtr,
                  CEPtr _Nonnull object,
                  CETaskFinish_f _Nullable finish,
                  CEFunction_f _Nonnull execute,
                  CETaskParamRef _Nullable param);


void CEConcurrentQueueBarrierSync(CEQueueRef _Nonnull queuePtr,
                                  CEPtr _Nonnull object,
                                  CETaskFinish_f _Nullable finish,
                                  CEFunction_f _Nonnull execute,
                                  CETaskParamRef _Nullable param,
                                  CETaskParamRef _Nullable result);

void CEConcurrentQueueBarrierAsync(CEQueueRef _Nonnull queuePtr,
                                   CEPtr _Nonnull object,
                                   CETaskFinish_f _Nullable finish,
                                   CEFunction_f _Nonnull execute,
                                   CETaskParamRef _Nullable param);


CEQueueRef _Nonnull CESerialQueueCreate(const char * _Nullable label, CEQueuePriority_t priority);

CEQueueRef _Nonnull CEQueueSharedMainQueue(void);
void CEQueueMain(void);


#endif /* CEQueue_h */
