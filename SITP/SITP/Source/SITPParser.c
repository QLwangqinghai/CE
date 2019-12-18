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
SITPParserCode const SITPParserCodeCustomStringEncodeNotSupport = 5;
SITPParserCode const SITPParserCodeLengthByteCountError = 6;
SITPParserCode const SITPParserCodeMessageSubTypeError = 7;

SITPParserCode const SITPParserCodeUnknownError = UINT32_MAX;



void _SITPParserControlEnqueue(SITPParserPtr _Nonnull parser, SITPParserFieldControl_t control);
SITPParserFieldControl_t _SITPParserControlDequeue(SITPParserPtr _Nonnull parser);



SITPParserCode _SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadFieldSubtype(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadFieldDataHead(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadSeekFieldContent(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);

SITPParserCode _SITPParserReadIndexShift(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);




//SITPParserCode SITPParserReadFieldHead(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range) {
//    return SITPParserCodeSuccess;
//}
//
//SITPParserCode SITPParserReadFieldSubtype(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range) {
//    return SITPParserCodeSuccess;
//}
//
//SITPParserCode SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range) {
//    return SITPParserCodeSuccess;
//}
//
//SITPParserCode SITPParserReadSeekFieldContent(SITPParserPtr _Nonnull parser, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range) {
//    return SITPParserCodeSuccess;
//}

SITPParserCode SITPByteReaderMemoryRead(void * _Nonnull context, uint8_t * _Nonnull buffer, SITPByteRange range) {
    assert(context);
    SITPByteReaderMemoryContext_t * memoryContext = context;
    
    return SITPParserCodeSuccess;
}



SITPParserCode SITPParserParseData(void * _Nullable context, SITPByteBuffer_t * _Nonnull buffers, uint32_t bufferCount, SITPByteRange range, SITPParserParseCallback_f _Nonnull callback) {
    if (range.length == 0) {
        return SITPParserCodeSuccess;
    }
    
    // do check input
    
    
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
            if (parser.byteRange.length <= parser.readLength) {
                return SITPParserCodeNeedMoreData;
            }
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

//    SITPIndex index;
//    uint32_t type: 8;
//    uint32_t subtype: 8;//data、 string 时有用
//    uint32_t contentControl: 16;
//    SITPByteRange contentRange;
    
    result = reader.read(reader.context, &byte, range);
    if (SITPParserCodeSuccess == result) {
        uint8_t type = (byte & 0xf0) >> 4;
        field->type = type;
        switch (type) {
            case SITPTypeCodeSInt:
            case SITPTypeCodeUInt: {
                SITPByteSize length = 16 - (byte & 0xf);
                field->contentRange = SITPByteRangeMake(range.location + range.length, length);
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadSeekFieldContent,
                    .length = length,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            case SITPTypeCodeFloat32: {
                field->contentRange = SITPByteRangeMake(range.location + range.length, 4);
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadSeekFieldContent,
                    .length = 4,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            case SITPTypeCodeFloat64: {
                field->contentRange = SITPByteRangeMake(range.location + range.length, 8);
                SITPParserFieldControl_t control = {
                    .func = _SITPParserReadSeekFieldContent,
                    .length = 8,
                };
                _SITPParserControlEnqueue(parser, control);
            }
                break;
            case SITPTypeCodeBool: {
                field->contentRange = SITPByteRangeMake(range.location + range.length, 0);
                field->contentControl = (byte & 0xf);
            }
                break;
            case SITPTypeCodeData: {
                uint8_t subtypeControl = (byte & 0xf);
                if (0xf == subtypeControl) {
                    SITPParserFieldControl_t control = {
                        .func = _SITPParserReadFieldDataHead,
                        .length = 1,
                    };
                    _SITPParserControlEnqueue(parser, control);
                } else {
                    //check subtype
                    SITPByteSize len = SITPDataSubtypeGetLength(subtypeControl);
                    if (len <= 0) {
                        return SITPParserCodeUnknownDataSubType;
                    }
                    field->subtype = subtypeControl;
                    field->contentRange = SITPByteRangeMake(range.location + range.length, len);
                    SITPParserFieldControl_t control = {
                        .func = _SITPParserReadSeekFieldContent,
                        .length = len,
                    };
                    _SITPParserControlEnqueue(parser, control);
                }
            }
                break;
            case SITPTypeCodeString: {
                uint8_t subtypeControl = ((byte >> 3) & 0x1);
                if (0 == subtypeControl) {
                    SITPByteSize len = byte & 0x7;
                    if (len > 4) {
                        return SITPParserCodeLengthByteCountError;
                    }
                    field->subtype = subtypeControl;
                    SITPParserFieldControl_t control = {
                        .func = _SITPParserReadFieldContentLength,
                        .length = len,
                    };
                    _SITPParserControlEnqueue(parser, control);
                } else {
                    return SITPParserCodeCustomStringEncodeNotSupport;
                }
            }
                break;
            case SITPTypeCodeMessage: {
                uint8_t subtypeControl = ((byte >> 3) & 0x1);
                if (0 == subtypeControl) {
                    SITPByteSize len = byte & 0x7;
                    if (len > 4) {
                        return SITPParserCodeLengthByteCountError;
                    }
                    field->subtype = subtypeControl;
                    SITPParserFieldControl_t control = {
                        .func = _SITPParserReadFieldContentLength,
                        .length = len,
                    };
                    _SITPParserControlEnqueue(parser, control);
                } else {
                    return SITPParserCodeMessageSubTypeError;
                }
            }
                break;
            default:
                abort();
                break;
        }
        
        
        
        
        
        
        
        
        
        SITPIndexShift_t shift = SITPIndexShiftMake(byte);
        parser->readingIndexs.location += shift.offset;
        parser->readingIndexs.length = shift.length;
    }
    return result;
    
}


SITPParserCode _SITPParserReadFieldDataHead(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range) {
    
}

//SITPParserCode _SITPParserReadFieldSubtype(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);
//
//SITPParserCode _SITPParserReadFieldContentLength(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);
//
//SITPParserCode _SITPParserReadSeekFieldContent(SITPParserPtr _Nonnull parser, SITPByteReader_t reader, SITPByteRange range);
