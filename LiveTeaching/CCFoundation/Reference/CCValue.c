//
//  CCValue.c
//  SITP
//
//  Created by vector on 2019/12/27.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCValue.h"

const CCValueTypeCode CCValueTypeCodeString = 1;
const CCValueTypeCode CCValueTypeCodeTaggedString = 2;
const CCValueTypeCode CCValueTypeCodeConstantString = 0xC1;
const CCValueTypeCode CCValueTypeCodeData = 0x88;
const CCValueTypeCode CCValueTypeCodeTaggedData = 0x89;
const CCValueTypeCode CCValueTypeCodeConstantData = 0xC2;
