//
//  CCByte.h
//  SITP
//
//  Created by vector on 2019/12/30.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCByte_h
#define CCByte_h

#include "CCBase.h"

typedef struct {
    CCUInt8 content[16];
} CCByte16;

typedef struct {
    CCUInt8 content[20];
} CCByte20;

typedef struct {
    CCUInt8 content[32];
} CCByte32;

typedef struct {
    CCUInt8 content[48];
} CCByte48;

typedef struct {
    CCUInt8 content[64];
} CCByte64;

typedef struct {
    CCUInt8 content[128];
} CCByte128;


CCInt CCByteBase64EncodeBytes(const CCUInt8 * _Nonnull bytes, CCInt length, CCChar * _Nonnull outputBuffer, CCInt bufferLength);
CCInt CCByteBase64DecodeBytes(const CCChar * _Nonnull encoded, CCInt length, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);

extern const CCChar __CCBase64ByteToCharMappings[64];

#endif /* CCByte_h */
