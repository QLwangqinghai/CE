//
//  CoreSystem.h
//  CoreSystem
//
//  Created by vector on 2018/12/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//



#define __CoreSystemVersion 1000000ul


#define __CoreSystemVersionMajor 1ul
#define __CoreSystemVersionMinor 0ul
#define __CoreSystemVersionPatchleve 0ul


#include <CoreSystem/CSTarget.h>
#include <CoreSystem/CSBase.h>
#include <CoreSystem/CSTime.h>
#include <CoreSystem/CSMalloc.h>

#include <CoreSystem/CSAnsiCHeader.h>
#include <CoreSystem/CSPosixHeader.h>

#include <CoreSystem/CSSocket.h>
#include <CoreSystem/CSPosixHeader.h>



extern uint32_t const CoreSystemVersion;
extern uint32_t const CoreSystemVersionMajor;
extern uint32_t const CoreSystemVersionMinor;
extern uint32_t const CoreSystemVersionPatchleve;
