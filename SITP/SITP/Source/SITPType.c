//
//  SITPType.c
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "SITPType.h"
#include <string.h>
#include<memory.h>

SITPField_t const SITPFieldInvalid = {};

_Bool SITPFieldIsInvalid(SITPField_t field) {
    return memcmp(&field, &SITPFieldInvalid, sizeof(SITPField_t)) == 0;
}
