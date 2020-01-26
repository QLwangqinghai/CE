//
//  SITPIndexShift.h
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef SITPIndexShift_t_h
#define SITPIndexShift_t_h

#include "SITPType.h"


SITPIndexShift_t SITPIndexShiftMake(uint8_t byte);
void SITPIndexShiftUpdate(SITPIndexShift_t * _Nonnull shift, uint8_t byte);


#endif /* SITPIndexShift_t_h */
