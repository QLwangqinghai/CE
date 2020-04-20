//
//  CCInteger.h
//  CCollection
//
//  Created by vector on 2019/6/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef CCInteger_h
#define CCInteger_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "CCType.h"

//找出为1的最高位
SInteger CCUInt64MostSignificantBit(uint64_t n);
SInteger CCUInt32MostSignificantBit(uint32_t n);


//找出为1的最低位
SInteger CCUInt64LeastSignificantBit(uint64_t n);
SInteger CCUInt32LeastSignificantBit(uint32_t n);


typedef enum {
    CCIntegerEncodingCompress7BitsPerComponent = 0,
    CCIntegerEncodingCompressNoFlag = 1,
} CCIntegerEncoding_e;

SInteger CCUInt64Encode(uint64_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);
SInteger CCUInt32Encode(uint32_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);

SInteger CCSInt64Encode(int64_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);
SInteger CCSInt32Encode(int32_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);

//>0 success
SInteger CCUInt64Decode(uint64_t * _Nonnull n, CCIntegerEncoding_e encoding, const uint8_t * _Nonnull bytes, SInteger bytesLength);
SInteger CCSInt64Decode(int64_t * _Nonnull n, CCIntegerEncoding_e encoding, const uint8_t * _Nonnull bytes, SInteger bytesLength);



SInteger CCUInt64ToBytes(uint64_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);
SInteger CCUInt32ToBytes(uint32_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);
SInteger CCUInt16ToBytes(uint16_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);

SInteger CCSInt64ToBytes(int64_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);
SInteger CCSInt32ToBytes(int32_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);
SInteger CCSInt16ToBytes(int16_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength);


SInteger CCBytesToUInt64(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, uint64_t * _Nonnull n);
SInteger CCBytesToUInt32(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, uint32_t * _Nonnull n);
SInteger CCBytesToUInt16(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, uint16_t * _Nonnull n);

SInteger CCBytesToSInt64(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, int64_t * _Nonnull n);
SInteger CCBytesToSInt32(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, int32_t * _Nonnull n);
SInteger CCBytesToSInt16(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, int16_t * _Nonnull n);

#if defined(__cplusplus)
}  // extern C
#endif

#endif /* CInteger_h */
