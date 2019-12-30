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

#define CCByteBase64EncodeBlock 3
#define CCByteBase64DecodeBlock 4


CCInt CCByteBase64EncodeBytes(const CCUInt8 * _Nonnull bytes, CCInt length, CCChar * _Nonnull outputBuffer, CCInt bufferLength, CCBool needPadding);
CCInt CCByteBase64DecodeBytes(const CCChar * _Nonnull encoded, CCInt length, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);



#endif /* CCByte_h */
