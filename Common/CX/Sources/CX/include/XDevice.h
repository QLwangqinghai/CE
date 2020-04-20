//
//  XDevice.h
//  
//
//  Created by vector on 2020/4/3.
//

#ifndef XDevice_h
#define XDevice_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"

/**
 * 系统启动时长
 */
extern XTimeInterval XDeviceUptime(void);

//Avaliable CPU processor count
extern XUInt XDeviceProcessorCount(void);


extern XSize XDeviceCachelineSize(void);


#if defined(__cplusplus)
}  // extern C
#endif



#endif /* XDevice_h */
