//
//  SITPFrame.h
//  CoreLibrarys
//
//  Created by vector on 2018/12/20.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef SITPFrame_h
#define SITPFrame_h

#include <stdio.h>


static const uint64_t FrameFlagHasPadding = 0x1;


typedef struct __Frame {
    uint64_t length: 16;//2
    uint64_t id: 24;//5
    uint32_t level: 8;//6
    uint32_t time: 24;//9
    uint64_t flag: 8;//10
    
    uint8_t sign[8]


    uint16_t length;
    uint8_t type;
    uint8_t flag;
    uint32_t id: 24;
    uint32_t level: 8;
    uint32_t time: 24;
    
    
} Frame_s;








#endif /* SITPFrame_h */
