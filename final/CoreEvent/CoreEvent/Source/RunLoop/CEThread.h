//
//  CEThread.h
//  CoreEvent
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEThread_h
#define CEThread_h

#include "CERunLoopBase.h"












typedef struct _CEThreadConfig {
    pthread_t _Nullable thread;
    size_t stacksize;//0 use default
    
#warning 待完善
    int schedPriority;
} CEThreadConfig_s;





struct _CEThreadWaiter;

typedef struct _CEThreadWaiter * CEThreadWaiterRef;




CEThreadRef _Nullable CEThreadGetCurrent(void);

void CEThreadWaiterWait(CEThreadWaiterRef _Nonnull waiter);
void CEThreadWaiterWakeUp(CEThreadWaiterRef _Nonnull waiter);







#endif /* CEThread_h */
