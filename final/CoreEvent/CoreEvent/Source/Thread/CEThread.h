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

typedef struct _CEThreadConfig {
    pthread_t _Nullable thread;
    uint32_t stacksize;//0 use default
    float schedPriority;//[-1, 1]
} CEThreadConfig_s;

_Bool CEIsMainThread(void);


struct _CEThreadWaiter;

typedef struct _CEThreadWaiter * CEThreadWaiterRef;




CEThreadRef _Nullable CEThreadGetCurrent(void);

void CEThreadWaiterWait(CEThreadWaiterRef _Nonnull waiter);
void CEThreadWaiterWakeUp(CEThreadWaiterRef _Nonnull waiter);







#endif /* CEThread_h */
