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



//队列状态
struct _CEQueue {
    CEQueueBase_t base;
    char * _Nonnull label;
    CESourceRef _Nonnull source;
    CEThreadRef _Nonnull thread;
    
};





//00 休眠 01执行 10休眠->执行 11执行->休眠

/*
 全部task count
 正在执行中的 task count
 
 */


//高1 位 表示执行中的count， 其余为任务个数 running = countInfos != 0

struct _CESource {
#if CEBuild64Bit
    //高14 位 表示执行中的count， 其余为任务个数
    _Atomic(uint_fast64_t) countInfos;// 0->1 do wake; 1->0 do wait
#else
    //高8 位 表示执行中的count， 其余为任务个数
    _Atomic(uint_fast32_t) countInfos;// 0->1 do wake; 1->0 do wait
#endif
    uint32_t concurrency: 7;//[1-127]
    uint32_t concurrencyBitCount: 3;//[1-255]

    
    
};


#endif /* CERunLoopBase_h */
