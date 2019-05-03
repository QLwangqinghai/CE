//
//  CETime.h
//  CoreEvent
//
//  Created by 王青海 on 2018/11/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CETime_h
#define CETime_h

#include <stdio.h>

#include <sys/time.h>
#include <sys/times.h>


struct timespec CESystemBootInterval(void);


//单位微妙
uint64_t CESystemBootMicroseconds(void);


#endif /* CETime_h */
