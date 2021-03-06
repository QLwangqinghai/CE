//
//  CEThread.h
//  CoreEvent
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEThread_h
#define CEThread_h

#include "CEThreadBase.h"

CEThreadRef _Nonnull CEThreadGetCurrent(void);


static const float CEThreadConfigSchedPriorityMin = -1.0f;
static const float CEThreadConfigSchedPriorityMax = 1.0f;
static const float CEThreadConfigSchedPriorityDefault = 0.0f;


_Bool CEIsMainThread(void);


CEThreadRef _Nullable CEThreadCreate(CEThreadConfig_s config,
                                     void (* _Nonnull main)(void * _Nullable),
                                     void * _Nullable params,
                                     void (* _Nullable paramsDealloc)(void * _Nonnull));


CEThreadRef _Nonnull CEMainThreadShared(void);




#endif /* CEThread_h */
