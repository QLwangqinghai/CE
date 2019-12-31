//
//  CCInteger.h
//  CCollection
//
//  Created by vector on 2019/6/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCInteger_h
#define CCInteger_h

#include "CCType.h"

//找出为1的最高位
CCInt CCUInt64MostSignificant(CCUInt64 n);
CCInt CCUInt32MostSignificant(CCUInt32 n);


//找出为1的最低位
CCInt CCUInt64LeastSignificant(CCUInt64 n);
CCInt CCUInt32LeastSignificant(CCUInt32 n);

#endif /* CInteger_h */
