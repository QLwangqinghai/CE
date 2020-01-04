//
//  SITPParser.c
//  SITP
//
//  Created by vector on 2019/12/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "SITPParser.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "CCBase.h"

#include "SITPIndexShift.h"

static CCUInt8 const SITPTypeCodeBoolTrue = 0xC4;
static CCUInt8 const SITPTypeCodeBoolFalse = 0x84;


static SITPParserCode const SITPParserCodeUnknownError = 0;
static SITPParserCode const SITPParserCodeNeedMoreData = 1;
static SITPParserCode const SITPParserCodeParamError = 2;
static SITPParserCode const SITPParserCodePaddingError = 3;
static SITPParserCode const SITPParserCodeUnknownDataSubtype = 4;
static SITPParserCode const SITPParserCodeContentLengthControlError = 5;
static SITPParserCode const SITPParserCodeContentLengthError = 6;
static SITPParserCode const SITPParserCodeIndexCountError = 7;
static SITPParserCode const SITPParserCodeUnknownMessageEncodeError = 8;
static SITPParserCode const SITPParserCodeUnknownTypeError = 9;

static const SITPParserError_t __SITPParserErrors[10] = {
    {
        .code = SITPParserCodeUnknownError,
        .message = "SITP.Parser.UnknownError",
    },
    {
        .code = SITPParserCodeNeedMoreData,
        .message = "SITP.Parser.NeedMoreData",
    },
    {
        .code = SITPParserCodeParamError,
        .message = "SITP.Parser.ParamError",
    },
    {
        .code = SITPParserCodePaddingError,
        .message = "SITP.Parser.PaddingError",
    },
    {
        .code = SITPParserCodeUnknownDataSubtype,
        .message = "SITP.Parser.UnknownDataSubtype",
    },
    {
        .code = SITPParserCodeContentLengthControlError,
        .message = "SITP.Parser.ContentLengthControlError",
    },
    {
        .code = SITPParserCodeContentLengthError,
        .message = "SITP.Parser.ContentLengthError",
    },
    {
        .code = SITPParserCodeIndexCountError,
        .message = "SITP.Parser.IndexCountError",
    },
    {
        .code = SITPParserCodeUnknownMessageEncodeError,
        .message = "SITP.Parser.UnknownMessageEncodeError",
    },
    {
        .code = SITPParserCodeUnknownTypeError,
        .message = "SITP.Parser.UnknownTypeError",
    },
};

SITPParserErrorRef _Nullable __SITPParserErrorWithCode(CCInt code) {
    if (code > 0 && code < 10) {
        return &(__SITPParserErrors[code]);
    } else {
        return &(__SITPParserErrors[0]);
    }
}


typedef struct {
    SITPByteRange range;
    SITPByteBuffer_t * _Nonnull buffers;
    uint32_t bufferCount;
    uint32_t currentIndex;
    SITPByteSize location;
} SITPByteUnsafeReader_t;


//SITPParserCode const SITPParserCodeNeedMoreData = -1;
//SITPParserCode const SITPParserCodeParamError = -2;
//SITPParserCode const SITPParserCodeReadError = -3;
//
//SITPParserCode const SITPParserCodeUnknownDataSubType = -4;
//SITPParserCode const SITPParserCodePaddingError = -5;
//SITPParserCode const SITPParserCodeContentLengthControlError = -6;
//SITPParserCode const SITPParserCodeSubtypeError = -7;
//SITPParserCode const SITPParserCodeLengthError = -8;
//SITPParserCode const SITPParserCodeIndexCountError = -9;
//SITPParserCode const SITPParserCodeDataError = -10;
//
//
//SITPParserCode const SITPParserCodeUnknownError = INT32_MIN;


//SITPParserCode SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteSize location, SITPField_t * _Nonnull field, SITPByteSize * _Nonnull usedLength);

//void _SITPParserControlEnqueue(SITPParserPtr _Nonnull parser, SITPParserFieldControl_t control);
//SITPParserFieldControl_t _SITPParserControlDequeue(SITPParserPtr _Nonnull parser);


SITPParserErrorRef _Nullable _SITPParserReadFieldHead(SITPByteUnsafeReader_t * _Nonnull reader, SITPField_t * _Nonnull field);
SITPParserErrorRef _Nullable _SITPParserReadCompressFieldHead(SITPByteUnsafeReader_t * _Nonnull reader, SITPField_t * _Nonnull field);


//SITPParserCode _SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

//SITPParserCode _SITPParserReadFieldDataSubtype(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);
//
//SITPParserCode _SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);
//
//SITPParserCode _SITPParserReadSeekFieldContent(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);
//
//SITPParserCode _SITPParserReadIndexShift(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);


SITPParserErrorRef _Nullable _SITPByteUnsafeReaderRead(SITPByteUnsafeReader_t * _Nonnull reader, void * _Nonnull buffer, SITPByteSize length) {
    assert(reader);
    if (length < 0) {
        __SITPParserErrorWithCode(SITPParserCodeParamError);
    }
    if (length == 0) {
        return NULL;
    }
    SITPByteUnsafeReader_t * ctx = reader;
//    SITPByteBuffer_t * _Nonnull buffers;
//    uint32_t bufferCount;
//    uint32_t lastReadBufferIndex;
    
    if (reader->currentIndex >= reader->bufferCount) {
        return __SITPParserErrorWithCode(SITPParserCodeUnknownError);
    }
    SITPByteBuffer_t page = reader->buffers[ctx->currentIndex];
    if (page.range.location > reader->location || page.range.location + page.range.length < reader->location) {
        return __SITPParserErrorWithCode(SITPParserCodeUnknownError);
    }
//    while (page.range.location + page.range.length < reader->location) {
//        if (page.range.location > reader->location) {
//            if (ctx->lastReadBufferIndex > 0) {
//                ctx->lastReadBufferIndex -= 1;
//            } else {
//                return SITPParserCodeReadError;
//            }
//        } else {
//            if (ctx->lastReadBufferIndex + 1 < ctx->bufferCount) {
//                ctx->lastReadBufferIndex += 1;
//            } else {
//                return SITPParserCodeReadError;
//            }
//        }
//        page = ctx->buffers[ctx->lastReadBufferIndex];
//    }
    
    SITPByteRange unreaded = SITPByteRangeMake(reader->location, length);
    
    while (unreaded.length > 0) {
        SITPByteSize pageRemain = page.range.location + page.range.length - unreaded.location;
        uint8_t * content = page.content;
        if (pageRemain == unreaded.length) {
            content += unreaded.location - page.range.location;
            memcpy(buffer, content, unreaded.length);
            reader->currentIndex += 1;
            reader->location = unreaded.location + unreaded.length;
            return NULL;
        } else if (pageRemain > unreaded.length) {
            content += unreaded.location - page.range.location;
            memcpy(buffer, content, unreaded.length);
            reader->location = unreaded.location + unreaded.length;
            return NULL;
        } else {
            content += unreaded.location - page.range.location;
            memcpy(buffer, content, pageRemain);
            unreaded.location += pageRemain;
            unreaded.length -= pageRemain;
            reader->currentIndex += 1;
            if (reader->currentIndex >= reader->bufferCount) {
                return __SITPParserErrorWithCode(SITPParserCodeUnknownError);
            }
        }
    }
    return NULL;
}

CCBool __SITPByteUnsafeReaderInit(SITPByteUnsafeReader_t * _Nonnull reader, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range) {
    uint32_t currentBufferIndex = 0;
    SITPByteSize location = buffers[0].range.location;
    for (uint32_t i=0; i<bufferCount; i++) {
      SITPByteBuffer_t buffer = buffers[i];
      if (buffer.range.location == SITPByteSizeNotFound) {
          return false;
      }
      if (buffer.range.location + buffer.range.length < buffer.range.location) {
          return false;
      }
      if (buffer.range.length > 0 && NULL == buffer.content) {
          return false;
      }
      if (buffer.range.location != location) {
          return false;
      }
      if (buffer.range.location <= range.location && buffer.range.location + buffer.range.length > range.location) {
          currentBufferIndex = i;
      }
      location += buffer.range.length;
      if (location >= range.location + range.length) {
          break;
      }
    }

    reader->range = range;
    reader->buffers = buffers;
    reader->bufferCount = bufferCount;
    reader->currentIndex = currentBufferIndex;
    reader->location = range.location;
    return true;
}


SITPParserParseMessageResult_t SITPParserParseMessage(SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range, SITPParserErrorRef _Nullable * _Nonnull errorRef) {
    assert(errorRef);
    if (errorRef) {
        *errorRef = NULL;
    }
    SITPParserErrorRef error = NULL;

    SITPParserParseMessageResult_t result = {};
    result.erroredFieldIndex = SITPIndexNotFound;
    if (range.length == 0) {
        return result;
    }
    
    // do check input
    if (bufferCount <= 0) {
        goto onParamError;
    }
    if (range.location == SITPByteSizeNotFound) {
        goto onParamError;
    }
    if (range.location + range.length < range.location) {
        goto onParamError;
    }
    
    SITPByteUnsafeReader_t reader = {};
    if (!__SITPByteUnsafeReaderInit(&reader, buffers, bufferCount, range)) {
        goto onParamError;
    }

    SITPMessageEncoding_e encodeType = 0;
    SITPIndex indexCount = SITPMessageEncodingError;
    do {
        uint8_t indexCountByte = 0;
        for (int i=0; i<3; i++) {
            error = _SITPByteUnsafeReaderRead(&reader, &indexCountByte, 1);
            if (error) {
                goto onError;
            }
            if (0 == i) {
                encodeType = ((indexCountByte >> 2) & 0x1F);
                indexCount = (indexCountByte & 0x3);
                if (SITPMessageEncodingIsInvalid(encodeType)) {
                    error = __SITPParserErrorWithCode(SITPParserCodeUnknownMessageEncodeError);
                    goto onError;
                }
            } else {
                indexCount = indexCount << 7;
                indexCount += (indexCountByte & 0x7f);
            }
            if ((indexCountByte & 0x80) == 0) {
                break;
            } else {
                if (i == 2) {
                    error = __SITPParserErrorWithCode(SITPParserCodeIndexCountError);
                    goto onError;
                }
            }
        }
    } while (0);
    result.fields = CCAllocate(sizeof(SITPField_t) * indexCount);
    result.fieldCount = indexCount;
    SITPIndex fieldIndex = 0;
    CCIndex fieldBufferOffset = 0;
    while (fieldBufferOffset < indexCount) {
        uint8_t byte = 0;
        *errorRef = _SITPByteUnsafeReaderRead(&reader, &byte, 1);
        if (*errorRef) {
            goto onError;
        }
        SITPIndexShift_t shift = SITPIndexShiftMake(byte);
        fieldIndex += shift.offset;
        if (fieldIndex + shift.length > SITPMessageMaxIndex) {
            goto onError;
        }
        for (SITPIndex i=0; i<shift.length; i++) {
            result.fields[fieldBufferOffset].index = fieldIndex + i;
            fieldBufferOffset += 1;
        }
        fieldIndex += shift.length;
    }
    
    fieldBufferOffset = 0;
    
    SITPParserErrorRef _Nullable (*readFieldHeadFunc)(SITPByteUnsafeReader_t * _Nonnull reader, SITPField_t * _Nonnull field);

    switch (encodeType) {
        case SITPMessageEncodingNormal:
            readFieldHeadFunc = _SITPParserReadFieldHead;
            break;
        case SITPMessageEncodingCompressFieldContentLength:
            readFieldHeadFunc = _SITPParserReadCompressFieldHead;
            break;
        default:
            abort();
            break;
    }
    
    
    while (fieldBufferOffset < indexCount) {
        error = readFieldHeadFunc(&reader, &(result.fields[fieldBufferOffset]));
        if (error) {
            goto onError;
        }
        fieldBufferOffset += 1;
    }
    result.readLength = (SITPSize)(reader.location - range.location);
    return result;

onError:
    if (errorRef) {
        *errorRef = error;
    }
    result.readLength = (SITPSize)(reader.location - range.location);
    return result;

onParamError:
    error = __SITPParserErrorWithCode(SITPParserCodeParamError);
    if (errorRef) {
        *errorRef = error;
    }
    return result;
}


//SITPParserCode SITPParserParseData(void * _Nullable context, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range, SITPParserParseCallback_f _Nonnull callback) {
//    if (range.length == 0) {
//        return SITPParserCodeSuccess;
//    }
//    
//    // do check input
//    if (bufferCount <= 0) {
//        return SITPParserCodeParamError;
//    }
//    if (range.location == SITPByteSizeNotFound) {
//        return SITPParserCodeParamError;
//    }
//    if (range.location + range.length < range.location) {
//        return SITPParserCodeParamError;
//    }
//    SITPByteSize location = buffers[0].range.location;
//    for (uint32_t i=0; i<bufferCount; i++) {
//        SITPByteBuffer_t buffer = buffers[i];
//        if (buffer.range.location == SITPByteSizeNotFound) {
//            return SITPParserCodeParamError;
//        }
//        if (buffer.range.location + buffer.range.length < range.location) {
//            return SITPParserCodeParamError;
//        }
//        if (buffer.range.length > 0 && NULL == buffer.content) {
//            return SITPParserCodeParamError;
//        }
//        if (buffer.range.location != location) {
//            return SITPParserCodeParamError;
//        }
//        location += buffer.range.length;
//        if (location >= range.location + range.length) {
//            break;
//        }
//    }
//    
//    SITPParserCode result = SITPParserCodeSuccess;
//    SITPByteReaderMemoryContext_t memoryContext = {};
//    memoryContext.range = range;
//    memoryContext.buffers = buffers;
//    memoryContext.bufferCount = bufferCount;
//    memoryContext.lastReadBufferIndex = 0;
//    
//    SITPByteReader_t reader = {
//        .context = &memoryContext,
//        .read = SITPByteReaderMemoryRead,
//    };
//    
//    SITPParser_t parser = {};
//    parser.byteRange = range;
//    
//    while (parser.byteRange.length > parser.readLength && SITPParserCodeSuccess == result) {
//        while (parser.byteRange.length > parser.readLength && 0 == parser.readingIndexs.length) {
//            SITPByteRange range = SITPByteRangeMake(parser.readLength + parser.byteRange.location, 1);
//            result = _SITPParserReadIndexShift(&parser, reader, range);
//            if (SITPParserCodeSuccess == result) {
//                parser.readLength += 1;
//            } else {
//                return result;
//            }
//        }
//        
//        while (parser.readingIndexs.length > 0) {
//            memset(&(parser.readingField), 0, sizeof(SITPField_t));
//            parser.readingField.index = parser.readingIndexs.location;
//            
//            //read field begin
//            SITPParserFieldControl_t readHeadControl = {
//                .func = _SITPParserReadFieldHead,
//                .length = 1,
//            };
//            _SITPParserControlEnqueue(&parser, readHeadControl);
//            while (parser.controlCount > 0) {
//                SITPParserFieldControl_t control = _SITPParserControlDequeue(&parser);
//                if (parser.byteRange.length - parser.readLength < control.length) {
//                    return SITPParserCodeNeedMoreData;
//                }
//                SITPByteRange range = SITPByteRangeMake(parser.readLength + parser.byteRange.location, control.length);
//                result = control.func(&parser, reader, range);
//                if (SITPParserCodeSuccess == result) {
//                    parser.readLength += range.length;
//                } else {
//                    return result;
//                }
//            }
//            callback(context, parser.readingField);
//            //read field end
//            
//            parser.readingIndexs.location += 1;
//            parser.readingIndexs.length -= 1;
//        }
//    }
//    return result;
//}

//SITPParserCode _SITPParserReadIndexShift(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
//    assert(parser);
//    assert(range.length == 1);
//    SITPParserCode result = SITPParserCodeSuccess;
//    uint8_t byte = 0;
//
//    result = reader.read(reader.context, &byte, range);
//    if (SITPParserCodeSuccess == result) {
//        SITPIndexShift_t shift = SITPIndexShiftMake(byte);
//        parser->readingIndexs.location += shift.offset;
//        parser->readingIndexs.length = shift.length;
//    }
//    return result;
//}

//SITPParserCode __SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range, SITPByteSize * _Nonnull size) {
//    assert(parser);
//    assert(size);
//    assert(!SITPByteRangeIsInvalid(range));
//    assert(range.length <= 4);
//    assert(range.length > 0);
//    SITPParserCode result = SITPParserCodeSuccess;
//
//    uint32_t length = 0;
//    result = reader.read(reader.context, &length, range);
//    if (SITPParserCodeSuccess != result) {
//        return result;
//    }
//
//#if CCBuildBigEndian
//    uint8_t * bytes = (uint8_t *)(&length);
//    uint8_t tmp = bytes[0];
//    bytes[0] = bytes[3];
//    bytes[3] = tmp;
//
//    tmp = bytes[1];
//    bytes[1] = bytes[2];
//    bytes[2] = tmp;
//#endif
//    if (length <= 0) {
//        return SITPParserCodeLengthError;
//    }
//    *size = length;
//    return result;
//}

SITPParserErrorRef _Nullable _SITPParserReadFieldHead(SITPByteUnsafeReader_t * _Nonnull reader, SITPField_t * _Nonnull field) {
    assert(field);
    assert(reader);
    SITPParserErrorRef error = NULL;
    uint8_t type = 0;
    error = _SITPByteUnsafeReaderRead(reader, &type, 1);
    if (error) {
        return error;
    }
    if (type == SITPTypeCodeBoolTrue) {
        field->type = SITPTypeCodeBool;
        field->boolValue = 0x1;
        field->contentLength = 0;
        return NULL;
    } else if (type == SITPTypeCodeBoolFalse) {
        field->type = SITPTypeCodeBool;
        field->boolValue = 0x0;
        field->contentLength = 0;
        return NULL;
    } else {
        field->type = type;
    }

    field->type = type;
    switch (type) {
        case SITPTypeCodeSInt:
        case SITPTypeCodeUInt: {
            field->contentLength = 8;
            return NULL;
        }
            break;
        case SITPTypeCodeFloat32: {
            field->contentLength = 4;
            return NULL;
        }
            break;
        case SITPTypeCodeFloat64: {
            field->contentLength = 8;
            return NULL;
        }
            break;
        case SITPTypeCodeBool: {
            return __SITPParserErrorWithCode(SITPParserCodeUnknownTypeError);
        }
            break;
        case SITPTypeCodeTime: {
            field->contentLength = 8;
            return NULL;
        }
            break;
        case SITPTypeCodeByte16: {
            field->contentLength = 16;
            return NULL;
        }
            break;
        case SITPTypeCodeData:
        case SITPTypeCodeDataArray:
        case SITPTypeCodeString:
        case SITPTypeCodeMessage:
        case SITPTypeCodeSIntArray:
        case SITPTypeCodeUIntArray:
        case SITPTypeCodeStringArray:
        case SITPTypeCodeMessageArray:
        case SITPTypeCodeBoolArray:
        case SITPTypeCodeTimeArray:
        case SITPTypeCodeFloat32Array:
        case SITPTypeCodeFloat64Array:
        case SITPTypeCodeByte16Array: {
            uint32_t length = 0;
            error = _SITPByteUnsafeReaderRead(reader, &length, 4);
            if (error) {
                return error;
            }
#if CCBuildBigEndian
            uint8_t * bytes = (uint8_t *)(&length);
            uint8_t tmp = bytes[0];
            bytes[0] = bytes[3];
            bytes[3] = tmp;

            tmp = bytes[1];
            bytes[1] = bytes[2];
            bytes[2] = tmp;
#endif
            field->contentLength = length;
            return NULL;
        }
            break;
        default:
            return __SITPParserErrorWithCode(SITPParserCodeUnknownTypeError);
            break;
    }
    return NULL;

onPaddingError:
    return __SITPParserErrorWithCode(SITPParserCodePaddingError);
}


SITPParserErrorRef _Nullable _SITPParserReadCompressFieldHead(SITPByteUnsafeReader_t * _Nonnull reader, SITPField_t * _Nonnull field) {
    assert(field);
    assert(reader);
    SITPParserErrorRef error = NULL;
    uint8_t byte = 0;
    error = _SITPByteUnsafeReaderRead(reader, &byte, 1);
    if (error) {
        return error;
    }
    uint8_t type = byte >> 3;
    field->type = type;
    switch (type) {
        case SITPTypeCodeSInt:
        case SITPTypeCodeUInt: {
            SITPSize length = 8 - (byte & 0x7);
            field->contentLength = length;
            return error;
        }
            break;
        case SITPTypeCodeFloat32: {
            if (0 != (byte & 0x7)) {
                goto onPaddingError;
            }
            field->contentLength = 4;
            return error;
        }
            break;
        case SITPTypeCodeFloat64: {
            if (0 != (byte & 0x7)) {
                goto onPaddingError;
            }
            field->contentLength = 8;
            return error;
        }
            break;
        case SITPTypeCodeBool: {
            if (0 == (byte & 0x6)) {
                field->boolValue = (byte & 0x1);
                field->contentLength = 0;
            } else {
                goto onPaddingError;
            }
            return error;
        }
            break;
        case SITPTypeCodeTime: {
            if (0 != (byte & 0x7)) {
                goto onPaddingError;
            }
            field->contentLength = 8;
            return error;
        }
            break;
        case SITPTypeCodeByte16: {
            if (0 != (byte & 0x7)) {
                goto onPaddingError;
            }
            field->contentLength = 16;
            return error;
        }
            break;
        case SITPTypeCodeData:
        case SITPTypeCodeDataArray:
        case SITPTypeCodeString:
        case SITPTypeCodeMessage:
        case SITPTypeCodeSIntArray:
        case SITPTypeCodeUIntArray:
        case SITPTypeCodeStringArray:
        case SITPTypeCodeMessageArray:
        case SITPTypeCodeBoolArray:
        case SITPTypeCodeTimeArray:
        case SITPTypeCodeFloat32Array:
        case SITPTypeCodeFloat64Array:
        case SITPTypeCodeByte16Array: {
            SITPByteSize lengthCount = byte & 0x7;
            if (lengthCount > 4) {
                return __SITPParserErrorWithCode(SITPParserCodeContentLengthControlError);
            } else if (lengthCount == 0) {
                field->contentLength = 0;
                return error;
            } else {
                uint32_t length = 0;
                error = _SITPByteUnsafeReaderRead(reader, &length, lengthCount);
                if (error) {
                    return error;
                }
#if CCBuildBigEndian
                uint8_t * bytes = (uint8_t *)(&length);
                uint8_t tmp = bytes[0];
                bytes[0] = bytes[3];
                bytes[3] = tmp;

                tmp = bytes[1];
                bytes[1] = bytes[2];
                bytes[2] = tmp;
#endif
                field->contentLength = length;
                return error;
            }
        }
            break;
        default: {
            return __SITPParserErrorWithCode(SITPParserCodeUnknownTypeError);
        }
            break;
    }
    return NULL;

onPaddingError:
    return __SITPParserErrorWithCode(SITPParserCodePaddingError);
}



//SITPParserCode _SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
//    assert(parser);
//    assert(range.length == 1);
//    SITPField_t * field = &(parser->readingField);
//    SITPParserCode result = SITPParserCodeSuccess;
//    uint8_t byte = 0;
//
//    result = reader.read(reader.context, &byte, range);
//    if (SITPParserCodeSuccess != result) {
//        return result;
//    }
//
//    uint8_t type = byte >> 3;
//    field->type = type;
//    switch (type) {
//        case SITPTypeCodeSInt:
//        case SITPTypeCodeUInt: {
//            SITPByteSize length = 8 - (byte & 0x7);
//            SITPParserFieldControl_t control = {
//                .func = _SITPParserReadSeekFieldContent,
//                .length = length,
//            };
//            _SITPParserControlEnqueue(parser, control);
//        }
//            break;
//        case SITPTypeCodeFloat32: {
//            SITPParserFieldControl_t control = {
//                .func = _SITPParserReadSeekFieldContent,
//                .length = 4,
//            };
//            _SITPParserControlEnqueue(parser, control);
//        }
//            break;
//        case SITPTypeCodeFloat64: {
//            SITPParserFieldControl_t control = {
//                .func = _SITPParserReadSeekFieldContent,
//                .length = 8,
//            };
//            _SITPParserControlEnqueue(parser, control);
//        }
//            break;
//        case SITPTypeCodeBool: {
//            if (0 == (byte & 0x6)) {
//                field->content.boolValue = (byte & 0x1);
//            } else {
//                return SITPParserCodePaddingError;
//            }
//        }
//            break;
//        case SITPTypeCodeTime: {
//            SITPParserFieldControl_t control = {
//                .func = _SITPParserReadSeekFieldContent,
//                .length = 8,
//            };
//            _SITPParserControlEnqueue(parser, control);
//        }
//            break;
//        case SITPTypeCodeData:
//        case SITPTypeCodeDataArray: {
//            uint16_t subtype = byte & 0x3;
//            range.location += range.length;
//            if (0 == (byte & 0x4)) {
//                range.length = 1;
//                subtype = (subtype << 5);
//            } else {
//                range.length = 2;
//                subtype = (subtype << 13);
//            }
//            uint8_t bytes[2] = {};
//            result = reader.read(reader.context, bytes, range);
//            if (SITPParserCodeSuccess != result) {
//                return result;
//            }
//
//            SITPByteSize lengthControl = 0;
//            if (range.length == 1) {
//                subtype += (bytes[0] >> 3) & 0x1F;
//                lengthControl = bytes[0] & 0x7;
//            } else {
//                uint16_t tmp = bytes[0];
//                tmp = (tmp << 5);
//                subtype += tmp;
//                subtype += (bytes[1] >> 3) & 0x1F;
//                lengthControl = bytes[1] & 0x7;
//            }
//            field->subtype = subtype;
//
//            if (lengthControl > 4) {
//                return SITPParserCodeLengthByteCountError;
//            }
//
//
//            //check lengthControl
//
//            //read length
//
//
//        }
//            break;
//        case SITPTypeCodeString:
//        case SITPTypeCodeMessage:
//        case SITPTypeCodeSIntArray:
//        case SITPTypeCodeUIntArray:
//        case SITPTypeCodeStringArray:
//        case SITPTypeCodeMessageArray:
//        case SITPTypeCodeBoolArray:
//        case SITPTypeCodeTimeArray:
//        case SITPTypeCodeFloat32Array:
//        case SITPTypeCodeFloat64Array: {
//            SITPByteSize lengthCount = byte & 0x7;
//            if (lengthCount > 4) {
//                return SITPParserCodeLengthByteCountError;
//            }
//            SITPParserFieldControl_t control = {
//                .func = _SITPParserReadFieldContentLength,
//                .length = lengthCount,
//            };
//            _SITPParserControlEnqueue(parser, control);
//        }
//            break;
//        default:
//            abort();
//            break;
//    }
////        SITPIndexShift_t shift = SITPIndexShiftMake(byte);
////        parser->readingIndexs.location += shift.offset;
////        parser->readingIndexs.length = shift.length;
//    return result;
//}


//SITPParserCode SITPParserReadFieldDataSubtype(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
//    assert(parser);
//    assert(!SITPByteRangeIsInvalid(range));
//    assert(range.length == 1 || range.length == 2);
//    uint8_t bytes[2] = {};
//
//    SITPField_t * field = &(parser->readingField);
//    SITPParserCode result = SITPParserCodeSuccess;
//    result = reader.read(reader.context, bytes, range);
//    if (SITPParserCodeSuccess == result) {
//        uint16_t subtype = field->subtype;
//        SITPByteSize lengthControl = 0;
//        if (range.length == 1) {
//            subtype += bytes[0] >> 3;
//            lengthControl = bytes[0] & 0x7;
//        } else {
//            uint16_t tmp = bytes[0];
//            tmp = (tmp << 5);
//            subtype += tmp;
//            subtype += bytes[1] >> 3;
//            lengthControl = bytes[1] & 0x7;
//        }
//        field->subtype = subtype;
//
//        if (lengthControl > 4) {
//            return SITPParserCodeLengthByteCountError;
//        }
//        SITPParserFieldControl_t control = {
//            .func = _SITPParserReadSeekFieldContent,
//            .length = lengthControl,
//        };
//        _SITPParserControlEnqueue(parser, control);
//    }
//    return result;
//}

//SITPParserCode SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
//    assert(parser);
//    assert(!SITPByteRangeIsInvalid(range));
//    assert(range.length <= 4);
//    SITPParserCode result = SITPParserCodeSuccess;
//
//    if (range.length == 0) {
//        SITPParserFieldControl_t control = {
//            .func = _SITPParserReadSeekFieldContent,
//            .length = 0,
//        };
//        _SITPParserControlEnqueue(parser, control);
//    } else {
//        uint32_t length = 0;
//        result = reader.read(reader.context, &length, range);
//        if (SITPParserCodeSuccess == result) {
//            if (length > 0) {
//                SITPParserFieldControl_t control = {
//                    .func = _SITPParserReadSeekFieldContent,
//                    .length = length,
//                };
//                _SITPParserControlEnqueue(parser, control);
//            } else {
//                return SITPParserCodeLengthError;
//            }
//        }
//    }
//    return result;
//}





//SITPParserCode _SITPParserReadFieldDataSubtype(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
//    assert(parser);
//    assert(!SITPByteRangeIsInvalid(range));
//    assert(range.length == 1 || range.length == 2);
//    uint8_t bytes[2] = {};
//
//    SITPField_t * field = &(parser->readingField);
//    SITPParserCode result = SITPParserCodeSuccess;
//    result = reader.read(reader.context, bytes, range);
//    if (SITPParserCodeSuccess == result) {
//        uint16_t subtype = field->subtype;
//        SITPByteSize lengthControl = 0;
//        if (range.length == 1) {
//            subtype += bytes[0] >> 3;
//            lengthControl = bytes[0] & 0x7;
//        } else {
//            uint16_t tmp = bytes[0];
//            tmp = (tmp << 5);
//            subtype += tmp;
//            subtype += bytes[1] >> 3;
//            lengthControl = bytes[1] & 0x7;
//        }
//        field->subtype = subtype;
//
//        if (lengthControl > 4) {
//            return SITPParserCodeLengthByteCountError;
//        }
//        SITPParserFieldControl_t control = {
//            .func = _SITPParserReadSeekFieldContent,
//            .length = lengthControl,
//        };
//        _SITPParserControlEnqueue(parser, control);
//    }
//    return result;
//}

//SITPParserCode _SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
//    assert(parser);
//    assert(!SITPByteRangeIsInvalid(range));
//    assert(range.length <= 4);
//    SITPParserCode result = SITPParserCodeSuccess;
//
//    if (range.length == 0) {
//        SITPParserFieldControl_t control = {
//            .func = _SITPParserReadSeekFieldContent,
//            .length = 0,
//        };
//        _SITPParserControlEnqueue(parser, control);
//    } else {
//        uint32_t length = 0;
//        result = reader.read(reader.context, &length, range);
//        if (SITPParserCodeSuccess == result) {
//            if (length > 0) {
//                SITPParserFieldControl_t control = {
//                    .func = _SITPParserReadSeekFieldContent,
//                    .length = length,
//                };
//                _SITPParserControlEnqueue(parser, control);
//            } else {
//                return SITPParserCodeLengthError;
//            }
//        }
//    }
//    return result;
//}


//SITPParserCode _SITPParserReadSeekFieldContent(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
//    assert(!SITPByteRangeIsInvalid(range));
//    assert(parser);
//    SITPField_t * field = &(parser->readingField);
//    field->content.range = range;
//    return SITPParserCodeSuccess;
//}
//
//void _SITPParserControlEnqueue(SITPParserPtr _Nonnull parser, SITPParserFieldControl_t control) {
//    uint32_t index = (parser->controlCount + parser->controlOffset) & 0x7;
//    parser->controls[index] = control;
//    parser->controlCount += 1;
//}
//SITPParserFieldControl_t _SITPParserControlDequeue(SITPParserPtr _Nonnull parser) {
//    SITPParserFieldControl_t control = parser->controls[parser->controlOffset];
//    parser->controlOffset = (parser->controlOffset + 1) & 0x7;
//    parser->controlCount -= 1;
//    return control;
//}
