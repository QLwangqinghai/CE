//
//  LSFileHandler.h
//  LinkServer
//
//  Created by vector on 2020/2/6.
//  Copyright © 2020 vector. All rights reserved.
//

#ifndef LSFileHandler_h
#define LSFileHandler_h

#include "LSTypes.h"
#import <CoreEvent/CoreEvent.h>
#import <CCFoundation/CCFoundation.h>
#import <CCFoundation/CCNet.h>


void LSFileHandlerInit(LSFileHandler_s * _Nonnull handler, uint32_t id, LSManager_s * _Nonnull manager);

void LSFileHandlerSetConnectReadTimeout(LSFileHandler_s * _Nonnull handler, uint32_t id, LSManager_s * _Nonnull manager);



////timeout  单位为微妙 数值必须大于0 且 小于等于CEFileEventTimeoutMax
//CEResult_t CEPollSetFileEventReadTimeout(CEPollPtr _Nonnull poll, CEFileId fd, CEMicrosecondTime timeout);
//CEResult_t CEPollSetFileEventWriteTimeout(CEPollPtr _Nonnull poll, CEFileId fd, CEMicrosecondTime timeout);
//
//CEResult_t CEPollCancelFileEventReadTimeout(CEPollPtr _Nonnull poll, CEFileId fd);
//CEResult_t CEPollCancelFileEventWriteTimeout(CEPollPtr _Nonnull poll, CEFileId fd);
//
//





#endif /* LSFileHandler_h */
