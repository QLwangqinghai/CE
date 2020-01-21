//
//  BCRequestStatus.h
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/30.
//

#ifndef BCRequestStatus_h
#define BCRequestStatus_h


typedef NS_ENUM(NSUInteger, BCRequestStatus) {
    BCRequestStatusNone,
    BCRequestStatusRequesting,
    BCRequestStatusFinishSuccess,
    BCRequestStatusFinishError,
};

#endif /* BCRequestStatus_h */
