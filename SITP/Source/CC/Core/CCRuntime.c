//
//  CCRuntime.c
//  SITP
//
//  Created by vector on 2019/12/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCRuntime_Internal.h"




const CCType CCMetaType = {
    .type = (CCTypeRef)&CCMetaType,
    .size = sizeof(CCType),
    
    
};
