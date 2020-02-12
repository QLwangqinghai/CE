//
//  LSManager.h
//  LinkServer
//
//  Created by vector on 2020/2/6.
//  Copyright © 2020 vector. All rights reserved.
//

#ifndef LSManager_h
#define LSManager_h

#include "LSTypes.h"


LSEventLoop_s * _Nonnull LSManagerGetEventLoop(LSManager_s * _Nonnull manager, uint32_t index);

LSManager_s * _Nonnull LSManagerShared(void);


#endif /* LSManager_h */
