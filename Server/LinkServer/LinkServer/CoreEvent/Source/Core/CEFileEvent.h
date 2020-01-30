//
//  CEFileEvent.h
//  CoreEvent
//
//  Created by vector on 2018/11/26.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEFileEvent_h
#define CEFileEvent_h

#include "CEPollInternal.h"


//timeout  单位为毫妙 数值必须大于0 且 小于等于CEFileEventTimeoutMillisecondMax
CEResult_t CEFileEventSourceSetReadTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);
CEResult_t CEFileEventSourceSetWriteTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);

CEResult_t CEFileEventSourceCancelReadTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);
CEResult_t CEFileEventSourceCancelWriteTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);

CEResult_t CEFileEventSourceUpdateReadTime(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);
CEResult_t CEFileEventSourceUpdateWriteTime(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);

CEFileEvent_s * _Nullable CEFileEventSourceInit(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventHandler_f _Nonnull handler, void * _Nullable clientData);
CEResult_t CEFileEventSourceDeinit(CERunLoop_s * _Nonnull eventLoop, CEFileEvent_s * _Nonnull fe);


#endif /* CEFileEvent_h */
