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
                 CEObjectRelease_f _Nullable release,
                 CEFunction_f _Nonnull execute,
                 CETaskParamRef _Nonnull param,
                 CETaskParamRef _Nullable result);

void CEQueueAsync(CEQueueRef _Nonnull queuePtr,
                  CEPtr _Nonnull object,
                  CEObjectRelease_f _Nullable release,
                  CEFunction_f _Nonnull execute,
                  CETaskParamRef _Nonnull param);


void CEConcurrentQueueBarrierSync(CEQueueRef _Nonnull queuePtr,
                                  CEPtr _Nonnull object,
                                  CEObjectRelease_f _Nullable release,
                                  CEFunction_f _Nonnull execute,
                                  CETaskParamRef _Nonnull param,
                                  CETaskParamRef _Nullable result);

void CEConcurrentQueueBarrierAsync(CEQueueRef _Nonnull queuePtr,
                                   CEPtr _Nonnull object,
                                   CEObjectRelease_f _Nullable release,
                                   CEFunction_f _Nonnull execute,
                                   CETaskParamRef _Nonnull param);


CEQueueRef _Nonnull CEQueueSharedMainQueue(void);
void CEQueueMain(void);


#endif /* CEQueue_h */
