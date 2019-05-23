//
//  CETaskContext.h
//  CoreEvent
//
//  Created by vector on 2019/5/22.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CETaskContext_h
#define CETaskContext_h

#include "CEThreadBaseInternal.h"

CETaskContextPtr _Nonnull CETaskContextGetCurrent(void);

CETaskContext_s CETaskContexPush(void);


#endif /* CETaskContext_h */
