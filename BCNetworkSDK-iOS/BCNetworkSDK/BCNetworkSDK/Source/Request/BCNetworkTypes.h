//
//  BCNetworkTypes.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/30.
//  Copyright © 2018年 王青海. All rights reserved.
//

#ifndef BCNetworkTypes_h
#define BCNetworkTypes_h


#import "BCError.h"

typedef void(^BCAPISuccessHandle)(id response);
typedef void(^BCAPIErrorHandle)(BCError *error);


#endif /* BCNetworkTypes_h */
