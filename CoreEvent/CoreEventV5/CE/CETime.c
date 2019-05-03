//
//  CETime.c
//  CoreEvent
//
//  Created by 王青海 on 2018/11/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CETime.h"
#include "CSTime.h"


uint64_t CESystemBootInterval(void) {    
    struct timespec ts = CSBootInterval();
    return ts.tv_nsec / 1000ull + ts.tv_sec * 1000000ull;
}



