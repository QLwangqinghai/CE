//
//  BCWeakHelper.h
//  BichanWallet
//
//  Created by 王青海 on 2018/8/8.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#ifndef BCWeakHelper_h
#define BCWeakHelper_h


#define WeakSelf __weak typeof(self) weakSelf = self;

#define WeakSelfToStrongSelf __strong typeof(weakSelf) strongSelf = weakSelf;

#define WeakSelfToStrongSelfGuardSelf __strong typeof(weakSelf) strongSelf = weakSelf;\
if (!strongSelf) {\
return;\
}\

#endif /* BCWeakHelper_h */
