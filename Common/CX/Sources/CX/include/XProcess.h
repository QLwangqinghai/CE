//
//  XProcess.h
//  
//
//  Created by vector on 2020/4/3.
//

#ifndef XProcess_h
#define XProcess_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"

//返回0成功
extern int XProcessExecutablePath(char * _Nonnull buf, uint32_t * _Nonnull bufsize);


#if defined(__cplusplus)
}  // extern C
#endif


#endif /* XProcess_h */
