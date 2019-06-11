//
//  CESource.h
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CESource_h
#define CESource_h

#include "CEThreadBaseInternal.h"


CESource_s * _Nonnull CESourceCreate(CEQueue_s * _Nonnull queue, CEPtr _Nonnull context, CESourceAppendTask_f _Nonnull append);

void CESourceDestroy(CESource_s * _Nonnull source);


#endif /* CESource_h */
