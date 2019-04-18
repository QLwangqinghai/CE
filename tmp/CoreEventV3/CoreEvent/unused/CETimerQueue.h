//
//  CETimerQueue.h
//  CoreEvent
//
//  Created by 王青海 on 2018/11/9.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CETimerQueue_h
#define CETimerQueue_h

#include <stdio.h>



typedef uintptr_t ElemType;

struct _CEHeap //定义堆的顺序存储类型
{
    ElemType * _Nonnull heap; //定义指向动态数组空间的指针
    uint32_t len; //定义保存堆长度的变量,即数组长度，数组下标从0开始
    uint32_t maxSize;    //用于保存初始化时所给的动态数组空间的大小
};




typedef struct _CEHeap CEHeap_s;




#endif /* CETimerQueue_h */
