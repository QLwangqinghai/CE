//
//  CEFileEvent.h
//  CoreEvent
//
//  Created by vector on 2018/11/26.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEFileEvent_h
#define CEFileEvent_h

#include "CEBaseType.h"



CEResult_t CEFileEventSourceAdd(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventMask_es mask);
CEResult_t CEFileEventSourceRemove(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventMask_es mask);

//timeout  单位为毫妙 0 表示移除
//int CEFileEventSourceSetReadTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t timeout);
//int CEFileEventSourceSetWriteTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t timeout);


//timeout  单位为毫妙 0 表示移除， 时间最大值为 CEFileEventTimeoutMillisecondMax 毫秒
CEResult_t CEFileEventSourceSetReadTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);
CEResult_t CEFileEventSourceSetWriteTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout);

CEResult_t CEFileEventSourceCancelReadTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);
CEResult_t CEFileEventSourceCancelWriteTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);


CEResult_t CEFileEventSourceUpdateReadTime(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);
CEResult_t CEFileEventSourceUpdateWriteTime(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);

CEResult_t CEFileEventSourceInit(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventHandler_f _Nonnull handler, void * _Nullable clientData);
CEResult_t CEFileEventSourceDeinit(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd);


#endif /* CEFileEvent_h */
