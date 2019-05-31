//
//  CERunLoopBase.h
//  CoreEvent
//
//  Created by vector on 2019/5/6.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CERunLoopBase_h
#define CERunLoopBase_h

#include "CEThreadBaseInternal.h"
#include "CEParam.h"







//00 休眠 01执行 10休眠->执行 11执行->休眠

/*
 全部task count
 正在执行中的 task count
 
 */


//高1 位 表示执行中的count， 其余为任务个数 running = countInfos != 0



#endif /* CERunLoopBase_h */
