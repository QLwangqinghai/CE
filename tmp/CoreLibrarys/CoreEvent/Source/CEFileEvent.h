//
//  CEFileEvent.h
//  CoreEvent
//
//  Created by vector on 2018/11/26.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEFileEvent_h
#define CEFileEvent_h

#include <CoreEvent/CEBaseTypeInternal.h>


//timeout  单位为毫妙 数值必须大于0 且 小于等于CEFileEventTimeoutMillisecondMax
CEResult_t CEFileEventSourceSetReadTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);
CEResult_t CEFileEventSourceSetWriteTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);

CEResult_t CEFileEventSourceCancelReadTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t CEFileEventSourceCancelWriteTimeout(CEPoolRef _Nonnull pool, CEFileDescription_s fd);

CEResult_t CEFileEventSourceUpdateReadTime(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEResult_t CEFileEventSourceUpdateWriteTime(CEPoolRef _Nonnull pool, CEFileDescription_s fd);

CEFileEvent_s * _Nullable CEFileEventSourceInit(CEPoolRef _Nonnull pool, CEFileDescription_s fd, CEFileEventHandler_f _Nonnull handler, void * _Nullable clientData);
CEResult_t CEFileEventSourceDeinit(CEPoolRef _Nonnull pool, CEFileEvent_s * _Nonnull fe);


CEFileEventRef _Nullable CEPoolGetFileEvent(CEPoolRef _Nonnull pool, CEFileDescription_s fd);
CEFileEventRef _Nullable CEPoolGetFileEventByIndex(CEPoolRef _Nonnull pool, int index);


#endif /* CEFileEvent_h */
