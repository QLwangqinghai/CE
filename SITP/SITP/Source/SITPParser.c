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

#include "SITPIndexShift.h"

SITPParserCode const SITPParserCodeSuccess = 0;
SITPParserCode const SITPParserCodeNeedMoreData = 1;
SITPParserCode const SITPParserCodeParamError = 2;
SITPParserCode const SITPParserCodeReadError = 3;

SITPParserCode const SITPParserCodeUnknownDataSubType = 4;
SITPParserCode const SITPParserCodePaddingError = 5;
SITPParserCode const SITPParserCodeLengthByteCountError = 6;
SITPParserCode const SITPParserCodeMessageSubTypeError = 7;
SITPParserCode const SITPParserCodeLengthError = 8;

SITPParserCode const SITPParserCodeUnknownError = UINT32_MAX;



void _SITPParserControlEnqueue(SITPParserPtr _Nonnull parser, SITPParserFieldControl_t control);
SITPParserFieldControl_t _SITPParserControlDequeue(SITPParserPtr _Nonnull parser);



SITPParserCode _SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadFieldDataSubtype(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadSeekFieldContent(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadIndexShift(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);


SITPParserCode SITPByteReaderMemoryRead(void * _Nonnull context, void * _Nonnull buffer, SITPByteRange range) {
    assert(context);
    SITPByteReaderMemoryContext_t * ctx = context;
//    SITPByteBuffer_t * _Nonnull buffers;
//    uint32_t bufferCount;
//    uint32_t lastReadBufferIndex;
    
    SITPByteBuffer_t page = ctx->buffers[ctx->lastReadBufferIndex];
    
    while (page.range.location > range.location || page.range.location + page.range.length < range.location) {
        if (page.range.location > range.location) {
            if (ctx->lastReadBufferIndex > 0) {
                ctx->lastReadBufferIndex -= 1;
            } else {
                return SITPParserCodeReadError;
            }
        } else {
            if (ctx->lastReadBufferIndex + 1 < ctx->bufferCount) {
                ctx->lastReadBufferIndex += 1;
            } else {
                return SITPParserCodeReadError;
            }
        }
        page = ctx->buffers[ctx->lastReadBufferIndex];
    }
    
    SITPByteRange unreaded = range;
    
    while (unreaded.length > 0) {
        SITPByteSize pageRemain = page.range.location + page.range.length - unreaded.location;
        uint8_t * content = page.content;
        if (pageRemain >= unreaded.length) {
            content += unreaded.location - page.range.location;
            memcpy(buffer, content, unreaded.length);
            return SITPParserCodeSuccess;
        } else {
            content += unreaded.location - page.range.location;
            memcpy(buffer, content, pageRemain);
            unreaded.location += pageRemain;
            unreaded.length -= pageRemain;

            if (ctx->lastReadBufferIndex + 1 < ctx->bufferCount) {
                ctx->lastReadBufferIndex += 1;
                page = ctx->buffers[ctx->lastReadBufferIndex];
            } else {
                return SITPParserCodeReadError;
            }
        }
    }
    return SITPParserCodeSuccess;
}



SITPParserCode SITPParserParseData(void * _Nullable context, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range, SITPParserParseCallback_f _Nonnull callback) {
    if (range.length == 0) {
        return SITPParserCodeSuccess;
    }
    
    // do check input
    if (bufferCount <= 0) {
        return SITPParserCodeParamError;
    }
    if (range.location == SITPByteSizeNotFound) {
        return SITPParserCodeParamError;
    }
    if (range.location + range.length < range.location) {
        return SITPParserCodeParamError;
    }
    SITPByteSize location = buffers[0].range.location;
    for (uint32_t i=0; i<bufferCount; i++) {
        SITPByteBuffer_t buffer = buffers[i];
        if (buffer.range.location == SITPByteSizeNotFound) {
            return SITPParserCodeParamError;
        }
        if (buffer.range.location + buffer.range.length < range.location) {
            return SITPParserCodeParamError;
        }
        if (buffer.range.length > 0 && NULL == buffer.content) {
            return SITPParserCodeParamError;
        }
        if (buffer.range.location != location) {
            return SITPParserCodeParamError;
        }
        location += buffer.range.length;
        if (location >= range.location + range.length) {
            break;
        }
    }
    
    SITPParserCode result = SITPParserCodeSuccess;
    SITPByteReaderMemoryContext_t memoryContext = {};
    memoryContext.range = range;
    memoryContext.buffers = buffers;
    memoryContext.bufferCount = bufferCount;
    memoryContext.lastReadBufferIndex = 0;
    
    SITPByteReader_t reader = {
        .context = &memoryContext,
        .read = SITPByteReaderMemoryRead,
    };
    
    SITPParser_t parser = {};
    parser.byteRange = range;
    
    while (parser.byteRange.length > parser.readLength && SITPParserCodeSuccess == result) {
        while (parser.byteRange.length > parser.readLength && 0 == parser.readingIndexs.length) {
            SITPByteRange range = SITPByteRangeMake(parser.readLength + parser.byteRange.location, 1);
            result = _SITPParserReadIndexShift(&parser, reader, range);
            if (SITPParserCodeSuccess == result) {
                parser.readLength += 1;
            } else {
                return result;
            }
        }
        
        while (parser.readingIndexs.length > 0) {
            memset(&(parser.readingField), 0, sizeof(SITPField_t));
            parser.readingField.index = parser.readingIndexs.location;
            
            //read field begin
            SITPParserFieldControl_t readHeadControl = {
                .func = _SITPParserReadFieldHead,
                .length = 1,
            };
            _SITPParserControlEnqueue(&parser, readHeadControl);
            while (parser.controlCount > 0) {
                SITPParserFieldControl_t control = _SITPParserControlDequeue(&parser);
                if (parser.byteRange.length - parser.readLength < control.length) {
                    return SITPParserCodeNeedMoreData;
                }
                SITPByteRange range = SITPByteRangeMake(parser.readLength + parser.byteRange.location, control.length);
                result = control.func(&parser, reader, range);
                if (SITPParserCodeSuccess == result) {
                    parser.readLength += range.length;
                } else {
                    return result;
                }
            }
            callback(context, parser.readingField);
            //read field end
            
            parser.readingIndexs.location += 1;
            parser.readingIndexs.length -= 1;
        }
    }
    return result;
}

SITPParserCode _SITPParserReadIndexShift(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
    assert(parser);
    assert(range.length == 1);
    SITPParserCode result = SITPParserCodeSuccess;
    uint8_t byte = 0;

    result = reader.read(reader.context, &byte, range);
    if (SITPParserCodeSuccess == result) {
        SITPIndexShift_t shift = SITPIndexShiftMake(byte);
        parser->readingIndexs.location += shift.offset;
        parser->readingIndexs.length = shift.length;
    }
    return result;
}


SITPParserCode _SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
    assert(parser);
    assert(range.length == 1);
    SITPField_t * field = &(parser->readingField);
    SITPParserCode result = SITPParserCodeSuccess;
    uint8_t byte = 0;
    
    result = reader.read(reader.context, &byte, range);
    if (SITPParserCodeSuccess == result) {
        uint8_t type = byte >> 3;
        field->type = type;
        switch (type) {
            case SITPTypeCodeSInt:
            case SITPTypeCodeUInt: {
                SITPByteSize length = 8 - (byte & 0x7);
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadSeekFieldContent,
                    .length = length,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            case SITPTypeCodeFloat32: {
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadSeekFieldContent,
                    .length = 4,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            case SITPTypeCodeFloat64: {
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadSeekFieldContent,
                    .length = 8,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            case SITPTypeCodeBool: {
                if (0 == (byte & 0x6)) {
                    field->content.boolValue = (byte & 0x1);
                } else {
                    return SITPParserCodePaddingError;
                }
            }
                break;
            case SITPTypeCodeTimestamp: {
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadSeekFieldContent,
                    .length = 8,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            case SITPTypeCodeHighPrecisionTime: {
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadSeekFieldContent,
                    .length = 12,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            case SITPTypeCodeData:
            case SITPTypeCodeDataArray: {
                uint16_t subtype = byte & 0x3;
                SITPByteSize len = 0;
                if (0 == (byte & 0x4)) {
                    len = 1;
                    subtype = (subtype << 5);
                } else {
                    len = 2;
                    subtype = (subtype << 13);
                }
                field->subtype = subtype;
                
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadFieldDataSubtype,
                    .length = len,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            case SITPTypeCodeString:
            case SITPTypeCodeMessage:
            case SITPTypeCodeSIntArray:
            case SITPTypeCodeUIntArray:
            case SITPTypeCodeStringArray:
            case SITPTypeCodeMessageArray:
            case SITPTypeCodeBoolArray:
            case SITPTypeCodeTimestampArray:
            case SITPTypeCodeHighPrecisionTimeArray:
            case SITPTypeCodeFloat32Array:
            case SITPTypeCodeFloat64Array: {
                SITPByteSize lengthCount = byte & 0x7;
                if (lengthCount > 4) {
                    return SITPParserCodeLengthByteCountError;
                }
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadFieldContentLength,
                    .length = lengthCount,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            default:
                abort();
                break;
        }
//        SITPIndexShift_t shift = SITPIndexShiftMake(byte);
//        parser->readingIndexs.location += shift.offset;
//        parser->readingIndexs.length = shift.length;
    }
    return result;
}

SITPParserCode _SITPParserReadFieldDataSubtype(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
    assert(parser);
    assert(!SITPByteRangeIsInvalid(range));
    assert(range.length == 1 || range.length == 2);
    uint8_t bytes[2] = {};
    
    SITPField_t * field = &(parser->readingField);
    SITPParserCode result = SITPParserCodeSuccess;
    result = reader.read(reader.context, bytes, range);
    if (SITPParserCodeSuccess == result) {
        uint16_t subtype = field->subtype;
        SITPByteSize lengthControl = 0;
        if (range.length == 1) {
            subtype += bytes[0] >> 3;
            lengthControl = bytes[0] & 0x7;
        } else {
            uint16_t tmp = bytes[0];
            tmp = (tmp << 5);
            subtype += tmp;
            subtype += bytes[1] >> 3;
            lengthControl = bytes[1] & 0x7;
        }
        field->subtype = subtype;
        
        if (lengthControl > 4) {
            return SITPParserCodeLengthByteCountError;
        }
        SITPParserFieldControl_t control = {
            .func = _SITPParserReadSeekFieldContent,
            .length = lengthControl,
        };
        _SITPParserControlEnqueue(parser, control);
    }
    return result;
}

SITPParserCode _SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
    assert(parser);
    assert(!SITPByteRangeIsInvalid(range));
    assert(range.length <= 4);
    SITPParserCode result = SITPParserCodeSuccess;

    if (range.length == 0) {
        SITPParserFieldControl_t control = {
            .func = _SITPParserReadSeekFieldContent,
            .length = 0,
        };
        _SITPParserControlEnqueue(parser, control);
    } else {
        uint32_t length = 0;
        result = reader.read(reader.context, &length, range);
        if (SITPParserCodeSuccess == result) {
            if (length > 0) {
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadSeekFieldContent,
                    .length = length,
                };
                _SITPParserControlEnqueue(parser, control);
            } else {
                return SITPParserCodeLengthError;
            }
        }
    }
    return result;
}


SITPParserCode _SITPParserReadSeekFieldContent(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
    assert(!SITPByteRangeIsInvalid(range));
    assert(parser);
    SITPField_t * field = &(parser->readingField);
    field->content.range = range;
    return SITPParserCodeSuccess;
}

void _SITPParserControlEnqueue(SITPParserPtr _Nonnull parser, SITPParserFieldControl_t control) {
    uint32_t index = (parser->controlCount + parser->controlOffset) & 0x7;
    parser->controls[index] = control;
    parser->controlCount += 1;
}
SITPParserFieldControl_t _SITPParserControlDequeue(SITPParserPtr _Nonnull parser) {
    SITPParserFieldControl_t control = parser->controls[parser->controlOffset];
    parser->controlOffset = (parser->controlOffset + 1) & 0x7;
    parser->controlCount -= 1;
    return control;
}
