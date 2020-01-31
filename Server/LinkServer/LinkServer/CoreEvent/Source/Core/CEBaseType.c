//
//  CEBaseType.c
//  CoreEvent
//
//  Created by CaiLianfeng on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#include "CEBaseType.h"

const CEFileType CEFileTypeTcpClient = 0;
const CEFileType CEFileTypeTcpServer = 1;
const CEFileType CEFileTypeLocalClient = 2;
const CEFileType CEFileTypeLocalServer = 3;


const CEFileEventMask_es CEFileEventMaskNone = 0;
const CEFileEventMask_es CEFileEventMaskReadable = 1;
const CEFileEventMask_es CEFileEventMaskWritable = 2;

const CEFileEventMask_es CEFileEventMaskReadWritable = 3;

