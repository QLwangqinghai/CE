//
//  CEClosur.h
//  CoreEvent
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEClosure_h
#define CEClosure_h

#include "CEBaseType.h"


uintptr_t CEClosureInit(CEClosureExecute_f _Nonnull execute,
                        CEClosureClearData_f _Nullable clearData,
                        const void * _Nullable data,
                        size_t dataSize,
                        size_t paramsSize,
                        void * _Nullable resultReceiver,
                        size_t resultSize,
                        CEThreadWaiter_s * _Nullable waiter);

void CEClosureGetInfo(uintptr_t closure, CEClosureInfo_s * _Nonnull infoPtr);



#endif /* CEClosur_h */
