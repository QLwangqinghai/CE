//
//  LSEventLoop.h
//  LinkServer
//
//  Created by vector on 2020/2/12.
//  Copyright © 2020 vector. All rights reserved.
//

#ifndef LSEventLoop_h
#define LSEventLoop_h

#include "LSTypes.h"

//connectionCapacity 必须是8192的倍数
LSEventLoop_s * _Nonnull LSEventLoopCreate(uint32_t id, uint32_t groupCount);


#endif /* LSEventLoop_h */
