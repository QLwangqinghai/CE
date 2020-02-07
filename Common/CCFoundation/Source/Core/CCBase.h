//
//  CCBase.h
//  CCollection
//
//  Created by vector on 2019/6/26.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCBase_h
#define CCBase_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "CCInteger.h"
#include "CCType.h"
#include "CCConfig.h"
#include "CCAllocator.h"

#pragma mark - mach

#if !defined(MIN)
    #define MIN(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#endif

#if !defined(MAX)
    #define MAX(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif

#pragma mark - CCIndex

//2的n幂次方对齐
static inline CCIndex CCPowerAlign2(CCIndex capacity) {
    CCIndex initialCapacity = capacity >> 1;
    initialCapacity |= (initialCapacity >> 1);
    initialCapacity |= (initialCapacity >> 2);
    initialCapacity |= (initialCapacity >> 4);
    initialCapacity |= (initialCapacity >> 8);
    initialCapacity |= (initialCapacity >> 16);
    
#if CCBuild64Bit
    initialCapacity |= (initialCapacity >> 32);
#endif
    initialCapacity += 1;
    return initialCapacity;
}


#pragma mark - CCByte

void CCMemoryCopy(void * _Nonnull dst, const void * _Nonnull src, size_t size);

extern CCInt CCByteBase64EncodeBytes(const CCUInt8 * _Nonnull bytes, CCInt length, CCChar * _Nonnull outputBuffer, CCInt bufferLength);
extern CCInt CCByteBase64DecodeBytes(const CCChar * _Nonnull encoded, CCInt length, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);

extern const CCChar __CCBase64ByteToCharMappings[64];

#pragma mark - CCMicrosecondTime

extern CCMicrosecondTime CCMicrosecondTimeGetCurrent(void);
extern CCMicrosecondTime CCBootInterval(void);

//返回0成功
int CCGetExecutablePath(char * _Nonnull buf, uint32_t * _Nonnull bufsize);

#if defined(__cplusplus)
}  // extern C
#endif

#endif /* CCBase_h */
