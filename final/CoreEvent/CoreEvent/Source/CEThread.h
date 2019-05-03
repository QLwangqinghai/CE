//
//  CEThread.h
//  CoreEvent
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEThread_h
#define CEThread_h

#include "CEBase.h"


typedef struct _CEThread {
    pthread_t _Nullable thread;
} CEThread_s;
typedef CEThread_s * CEThreadRef;


typedef struct _CEThreadConfig {
    pthread_t _Nullable thread;
} CEThreadConfig_s;



typedef struct _CERunLoop {
    pthread_t _Nullable thread;
} CERunLoop_s;
typedef CERunLoop_s * CERunLoopRef;

struct _CEThreadWaiter;

typedef struct _CEThreadWaiter * CEThreadWaiterRef;


//pthread_key_t CERunLoopThreadKeyShared(void);
CEThreadWaiterRef _Nonnull CEThreadGetWaiter(void);


void CEThreadWaiterWait(CEThreadWaiterRef _Nonnull waiter);
void CEThreadWaiterWakeUp(CEThreadWaiterRef _Nonnull waiter);







#endif /* CEThread_h */
