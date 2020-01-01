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
CCInt CCUInt64MostSignificantBit(CCUInt64 n);
CCInt CCUInt32MostSignificantBit(CCUInt32 n);


//找出为1的最低位
CCInt CCUInt64LeastSignificantBit(CCUInt64 n);
CCInt CCUInt32LeastSignificantBit(CCUInt32 n);


typedef enum {
    CCIntegerEncodingCompress7BitsPerComponent = 0,
    CCIntegerEncodingCompressNoFlag = 1,
} CCIntegerEncoding_e;

CCInt CCUInt64Encode(CCUInt64 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);
CCInt CCUInt32Encode(CCUInt32 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);

CCInt CCSInt64Encode(CCSInt64 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);
CCInt CCSInt32Encode(CCSInt32 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);

CCInt CCUInt64Decode(CCUInt64 * _Nonnull n, CCIntegerEncoding_e encoding, const CCUInt8 * _Nonnull bytes, CCInt bytesLength);
CCInt CCSInt64Decode(CCSInt64 * _Nonnull n, CCIntegerEncoding_e encoding, const CCUInt8 * _Nonnull bytes, CCInt bytesLength);



CCInt CCUInt64ToBytes(CCUInt64 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);
CCInt CCUInt32ToBytes(CCUInt32 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);
CCInt CCUInt16ToBytes(CCUInt16 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);

CCInt CCSInt64ToBytes(CCSInt64 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);
CCInt CCSInt32ToBytes(CCSInt32 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);
CCInt CCSInt16ToBytes(CCSInt16 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength);


CCInt CCBytesToUInt64(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCUInt64 * _Nonnull n);
CCInt CCBytesToUInt32(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCUInt32 * _Nonnull n);
CCInt CCBytesToUInt16(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCUInt16 * _Nonnull n);

CCInt CCBytesToSInt64(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCSInt64 * _Nonnull n);
CCInt CCBytesToSInt32(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCSInt32 * _Nonnull n);
CCInt CCBytesToSInt16(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCSInt16 * _Nonnull n);


#endif /* CInteger_h */
