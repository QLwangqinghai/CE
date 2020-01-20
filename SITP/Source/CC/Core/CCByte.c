//
//  CCByte.c
//  SITP
//
//  Created by vector on 2019/12/30.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCByte.h"

//"-" / "." / "_" / "~"

//static const CCChar __CCBase64Padding = '_';

static const CCUInt8 __CCBase64CharToByteMappings[128] __attribute__((aligned(128))) = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 63, 255, 255, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255,
    255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 62,
    255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255,
};

const CCChar __CCBase64ByteToCharMappings[64] __attribute__((aligned(64))) = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '~', '-',
};

static inline CCUInt8 __CCByteBase64DecodeByte(CCChar c) {
    assert(c > 0);
    if (c >= 128) {
        return 255;
    } else {
        return __CCBase64CharToByteMappings[c];
    }
}


static inline CCChar __CCByteBase64EncodeByte(CCUInt8 byte) {
    assert(byte < 64);
    return __CCBase64ByteToCharMappings[byte];
}



CCInt CCByteBase64EncodeBytes(const CCUInt8 * _Nonnull bytes, CCInt length, CCChar * _Nonnull outputBuffer, CCInt bufferLength) {
    assert(length >= 0);
    if (length > 0) {
        assert(bytes);
    } else if (length == 0) {
        return 0;
    }
    if (length > 0 && bufferLength > 0) {
        assert(outputBuffer);
    }
    
#if CCBuild64Bit
    assert(length <= 6917529027641081853LL);
#else
    assert(length <= 1610612733L);
#endif
    
    CCInt outputLength = (length/3)*4;
    
    if (length % 3 == 1) {
        outputLength += 2;
    } else if (length % 3 == 2) {
        outputLength += 3;
    }
    if (bufferLength < outputLength) {
        return -1;
    }
    CCUInt8 currentByte = 0;
    CCInt outputIndex = 0;
    CCInt index=0;
    CCInt upper = length / 3 * 3;
    for (; index<upper; index += 3) {
        CCInt index1 = index + 1;
        CCInt index2 = index + 2;

        outputBuffer[outputIndex] = __CCByteBase64EncodeByte(bytes[index] >> 2);
        outputIndex ++;
        currentByte = ((bytes[index] << 4) & 0x3f);

        currentByte |= (bytes[index1] >> 4);
        outputBuffer[outputIndex] = __CCByteBase64EncodeByte(currentByte);
        outputIndex ++;
        currentByte = ((bytes[index1] << 2) & 0x3f);
        currentByte |= (bytes[index2] >> 6);
        outputBuffer[outputIndex] = __CCByteBase64EncodeByte(currentByte);
        outputIndex ++;
        outputBuffer[outputIndex] = __CCByteBase64EncodeByte((bytes[index2] & 0x3F));
        outputIndex ++;
    }

    if (length % 3 == 1) {
        outputBuffer[outputIndex] = __CCByteBase64EncodeByte(bytes[index] >> 2);
        outputIndex ++;
        currentByte = (bytes[index] & 0x3);

        outputBuffer[outputIndex] = __CCByteBase64EncodeByte(currentByte);
        outputIndex ++;
    } else if (length % 3 == 2) {
        outputBuffer[outputIndex] = __CCByteBase64EncodeByte(bytes[index] >> 2);
        outputIndex ++;
        currentByte = ((bytes[index] << 4) & 0x3f);

        currentByte |= (bytes[index + 1] >> 4);
        outputBuffer[outputIndex] = __CCByteBase64EncodeByte(currentByte);
        outputIndex ++;
        currentByte = ((bytes[index + 1] << 2) & 0x3f);
        outputBuffer[outputIndex] = __CCByteBase64EncodeByte(currentByte);
        outputIndex ++;
    }
    return outputIndex;
}

CCInt CCByteBase64DecodeBytes(const CCChar * _Nonnull encoded, CCInt length, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    assert(length >= 0);
    if (length > 0) {
        assert(encoded);
    } else if (length == 0) {
        return 0;
    }
    if (length > 0 && bufferLength > 0) {
        assert(outputBuffer);
    }
    CCInt outputLength = (length/4)*3;
    if (length % 4 == 1) {
        return -2;
    } else if (length % 4 == 2) {
        outputLength += 1;
    } else if (length % 4 == 3) {
        outputLength += 2;
    }
    if (bufferLength < outputLength) {
        return -1;
    }
        
    CCUInt8 currentByte = 0;
    CCInt outputIndex = 0;
    CCInt index=0;
    CCInt upper = length / 4 * 4;

    for (; index<upper; index += 4) {
        CCInt index1 = index + 1;
        CCInt index2 = index + 2;
        CCInt index3 = index + 3;

        CCUInt8 v = __CCByteBase64DecodeByte(encoded[index]);
        if (v >= 64) {
            return -4;
        }
        currentByte = (v << 2);
        
        v = __CCByteBase64DecodeByte(encoded[index1]);
        if (v >= 64) {
            return -4;
        }
        currentByte |= (v >> 4);
        outputBuffer[outputIndex] = currentByte;
        outputIndex ++;
        currentByte = (v << 4);
        
        
        v = __CCByteBase64DecodeByte(encoded[index2]);
        if (v >= 64) {
            return -4;
        }
        currentByte |= (v >> 2);
        outputBuffer[outputIndex] = currentByte;
        outputIndex ++;
        currentByte = (v << 6);

        v = __CCByteBase64DecodeByte(encoded[index3]);
        if (v >= 64) {
            return -4;
        }
        currentByte |= v;
        outputBuffer[outputIndex] = currentByte;
        outputIndex ++;
    }
    
    if (index % 4 == 0) {
    } else if (index % 4 == 2) {
        CCUInt8 v = __CCByteBase64DecodeByte(encoded[index]);
        if (v >= 64) {
            return -4;
        }
        currentByte = (v << 2);
        
        v = __CCByteBase64DecodeByte(encoded[index + 1]);
        if (v >= 64) {
            return -4;
        }
        if ((v & 0xf) != 0) {
            return -5;
        }
        currentByte |= (v >> 4);
        outputBuffer[outputIndex] = currentByte;
        outputIndex ++;
    } else if (index % 4 == 3) {
        CCInt index1 = index + 1;
        CCInt index2 = index + 2;
        CCUInt8 v = __CCByteBase64DecodeByte(encoded[index]);
        if (v >= 64) {
            return -4;
        }
        currentByte = (v << 2);
        
        v = __CCByteBase64DecodeByte(encoded[index1]);
        if (v >= 64) {
            return -4;
        }
        currentByte |= (v >> 4);
        outputBuffer[outputIndex] = currentByte;
        outputIndex ++;
        currentByte = (v << 4);
        
        
        v = __CCByteBase64DecodeByte(encoded[index2]);
        if (v >= 64) {
            return -4;
        }
        if ((v & 0x3) != 0) {
            return -5;
        }
        currentByte |= (v >> 2);
        outputBuffer[outputIndex] = currentByte;
        outputIndex ++;
    } else {
        abort();
    }

    return outputIndex;
}
