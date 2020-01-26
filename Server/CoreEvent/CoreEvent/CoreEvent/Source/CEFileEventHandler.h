//
//  CEFileEventHandler.h
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEFileEventHandler_h
#define CEFileEventHandler_h

#include "CEBaseType.h"


//0 is error
uint32_t CEFileEventHandlerRegister(CEFileEventHandler_f _Nonnull handler);
CEFileEventHandler_f _Nullable CEFileEventHandlerGet(uint32_t identifier);


#endif /* CEFileEventHandler_h */
